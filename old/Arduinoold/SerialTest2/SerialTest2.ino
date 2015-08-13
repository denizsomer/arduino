#include <SimpleTimer.h>
SimpleTimer t, tPhone;

//voltages
const  int pv_SolPan   = A0; //blue
const  int pv_BattV     = A1; //orange
//const  int pv_spare      = A2; //yellow
const  int pv_BattL     = A3; //green
//const  int pv_spare2     = A4; //yellow
//A8x->A15x

//Block 1:
//const  int p_phone2Tx   = 0; //Rx0 B3  -> UTB3
//const  int p_phone2Rx   = 1; //Tx0 B8  -> //UTB8
 //interrupts
const  int p_pirAl     = 2; //interrupt 0 G2/G7->Volt. Reducer
const  int p_pirTamp    = 3; //interrupt 1 G2/G7->Volt. Reducer
const  int p_ContSw    = 4; //grey from ROV57

//uln2003-2
const  int r_Phone      = 5; //ULN2-3
const  int r_Sink       = 6; //ULN2-2
const  int r_Cont  = 7; //ULN2-1

//Block 2:
//uln2003-3
const  int r_SolL     = 8; //ULN3-4
const  int r_SolV     = 9; //ULN3-3
//rel5v
const  int r_SrnOn   = 10;
const  int r_SrnOff  = 11;
const  int r_DH11      = 12;
//const  int r_spare5v = 13;

// Block 3:
//const  int p_phoneRx     = 14; //G11 ->ROG1
//const  int p_phoneTx     = 15; //G10 ->ROG5
//const  int p_ustapEcho   = 16; //G4 ->ROG2
//const  int p_ustapTrigger= 17; //G3 ->ROG6
//interrupts
const  int p_IsoSw     = 18; //  interrupt5 white from ROV5
const  int p_Ign     = 19; //  interrupt4 blue A6->Volt. Reducer
const  int p_HbrkSw    = 20; //  interrupt3 purple from ROV5
const  int p_AlSw  = 21; //  interrupt2 green from RUC5

//22x,23x,

// Block 4:
//const  int p_B6  = 24; // UTB6
const  int p_SolSw  = 25; // B2->SW3->A2->GND


//Empty
// 26x->39x

//Block 5:
//uln2003-1
const  int r_AlPow = 40; //ULN1-1
//const  int r_Spare    = 41; //ULN1-2
//const  int p_LED5       = 42;
//const  int r_Spare    = 43; //ULN1-3
//const  int p_LED4       = 44;
//const  int p_usrTrigger = 45; // G8->ROG3
//const  int p_LED3       = 46;
//const  int p_usrEcho    = 47; //G6->ROG7
//const  int p_LED2       = 48;
//49x, spare at RUC
const  int p_AlLED    = 50;
//const  int p_C7        = 51; //RUC7
//52x, 53x


//voltage multipliers
const double f0=0.02600,f1=0.01532, f2=0.01580,
             f3=0.01555, f4=0.01445, v_SolPanMin=4.0;

const int IgnDelay=2000, AlPowDelay=100, ntriggersDelay=1, SrnDuration=5000, 
          maxnAls=4, SolSwInterval=6000000, SolDelay=5000, interruptDelay=500,
          durationAledLED=2000, phoneInterval=1000, maxindex=250;

volatile int v_pirAl, v_pirTamp, v_Ign, ntriggers;
volatile bool Igndelay, IgnON, IsoSwCLOSED, HbrkSwDOWN;
volatile unsigned long IgnTime=0,AlLEDTime=0, elapsedAlLED=0, phoneTime=0;


unsigned long sinceIgn;
double v_SolPan , v_BattL  , v_BattV , v_SolCont;
bool AlArmed=false, AlSwed=false, isTriggered=false, AlSwedON=false, SolSwedON=false,
 isCIND=false,isCMGR=false,isCPMS=false, delayOnce=true;
int nAls=0, whichBattIsCharged=0;

//phone
const int lengthSMS=134, delayPhone=1; //for K700i
//const int lengthSMS=132, delayPhone=5; //for R520m
char phnData[maxindex],  CIND[10]="+CIND: /0", 
//CMGR[10]="+CMGR: /0", 
CMGR[10]="0791447/0",
CPMS[10]="+CPMS: /0",smsReceived[16]="0791447728008/0", cmd[4];
  char character;
  int i,j, jCIND, jCMGR, jCPMS, index;
  int phnBattLev, phnSigLev, phnBattWarn, phnCharger, phnService, 
  phnSounder, newMessage, newCall;


    void setup()
    {
    Serial.begin(9600); //initialize serial communication
    Serial3.begin(9600); //initialize serial communication
    
    t.setInterval(SolSwInterval, toggleSol);
    tPhone.setInterval(phoneInterval*2, checkStatus);
    
     //initialize outputs
     //uln2003-1
     pinMode(r_AlPow, OUTPUT); digitalWrite(r_AlPow, LOW);
     //uln2003-2
     pinMode(r_Phone, OUTPUT); digitalWrite(r_Phone, LOW);
     pinMode(r_Sink, OUTPUT); digitalWrite(r_Sink, LOW);
     pinMode(r_Cont, OUTPUT); digitalWrite(r_Cont, LOW);
     //uln2003-3
     pinMode(r_SolL, OUTPUT); digitalWrite(r_SolL, LOW);
     pinMode(r_SolV, OUTPUT); digitalWrite(r_SolV, LOW);
     //5V relay
     for (i = 10; i < 14; i++) {pinMode(i, OUTPUT); digitalWrite(i, HIGH);}
     pinMode(p_AlLED, OUTPUT); digitalWrite(p_AlLED, LOW);
    
    //G:
    /*pinMode(p_ustapEcho, INPUT);
    pinMode(p_usrearEcho, INPUT);
    pinMode(p_ustapTrigger, OUTPUT);
    pinMode(p_usrearTrigger, OUTPUT);*/
    
    pinMode(p_Ign, INPUT);  
    pinMode(p_pirAl, INPUT);
    pinMode(p_pirTamp, INPUT);
    
    //Listening, pull-ups
    pinMode(p_AlSw, INPUT);    digitalWrite(p_AlSw, HIGH);
    pinMode(p_IsoSw, INPUT);    digitalWrite(p_IsoSw, HIGH);
    pinMode(p_ContSw, INPUT);    digitalWrite(p_ContSw, HIGH);
    pinMode(p_HbrkSw, INPUT);    digitalWrite(p_HbrkSw, HIGH);
    pinMode(p_SolSw, INPUT);    digitalWrite(p_SolSw, HIGH);
    
    attachInterrupt(0, pirAlTriggered,    CHANGE); //p_pirAl 
    attachInterrupt(1, pirTampTriggered,    CHANGE); //p_pirTamp //MAY MAKE LOW LATER
    attachInterrupt(2, AlSwTriggered, CHANGE); //p_AlSw
    attachInterrupt(3, HbrkSwTriggered,    CHANGE); //p_HbrkSw
    attachInterrupt(4, IgnTriggered,    CHANGE); //p_Ign
    attachInterrupt(5, IsoSwTriggered,    CHANGE); //p_IsoSw
  
  prepareStorage();
   }


/////////////////////////////////////////////////////////////////////////////


    void loop()
    {
      
  
       t.run();
       tPhone.run();
      
    if(HbrkSwDOWN)  {lightLED(p_AlLED,2);  Serial.println("HbrkSwDOWN");}
    if(IsoSwCLOSED)  {lightLED(p_AlLED,3);  Serial.println("IsoSwCLOSED");}
    if(IgnON)  {lightLED(p_AlLED,4); Serial.println("IgnON");}
    if(AlSwed) { lightLED(p_AlLED,4); Serial.println("AlSwed");}
    
    
 // Sol
    SolSwedON=digitalRead(p_SolSw)==LOW;
      if(SolSwedON)      {
       if(whichBattIsCharged==0)    chargeSolL();
       }
      else      {
        if(whichBattIsCharged!=0)  shutSol();
      }
    
//ARM Al
if(AlSwed){AlSwed=false; delay(interruptDelay); AlSwedON=(digitalRead(p_AlSw)==LOW);}
if(AlSwedON)
{
  if(!AlArmed)
  {
    lightLED(p_AlLED,4,AlPowDelay, 8*AlPowDelay);
    lightLED(p_AlLED,4,AlPowDelay, 4*AlPowDelay);
    lightLED(p_AlLED,8,AlPowDelay,2*AlPowDelay);
AlArmed=true;
nAls=0;
ntriggers=0;
AlLEDTime=millis();
digitalWrite(r_AlPow, LOW);
Serial.println("Al armed");
  }
  else
  {
  elapsedAlLED=millis()-AlLEDTime;
    if(elapsedAlLED>durationAledLED){lightLED(p_AlLED,1); AlLEDTime=millis();} 
  }
}
else 
{
  AlArmed=false; 
  digitalWrite(r_AlPow, HIGH);
 // Serial.println("Al disarmed");
}
//Serial.print(digitalRead(p_pirAl));
//Serial.print(digitalRead(p_pirTamp));
// Serial.print(" nAls:"); Serial.print(nAls);
// Serial.print(" isTriggered:");Serial.print(isTriggered);
// Serial.print(" ntriggers:"); Serial.println(ntriggers);

//EXECUTE Al
if(AlArmed&&nAls<maxnAls)
{
 isTriggered=((digitalRead(p_pirAl)==LOW)||(digitalRead(p_pirTamp)==LOW)||HbrkSwDOWN||IsoSwCLOSED);
  
if(isTriggered)
{
  lightLED(p_AlLED,4); 
  if(ntriggers>ntriggersDelay||(digitalRead(p_pirTamp)==LOW)||HbrkSwDOWN||IsoSwCLOSED) playSrn();
 }
}


// Ign
// Sw off Sol Pans in case of Ign
  if(IgnON)
  {
  shutSol();
  IgnTime=millis();
  }
  else {
    IgnTime=0;
   if(SolSwedON&&whichBattIsCharged==0) chargeSolL();
  digitalWrite(r_Cont, LOW);
  }
  
  if(IgnTime>0)
  {
  sinceIgn=millis()-IgnTime;
  if(sinceIgn>IgnDelay) {
  digitalWrite(r_Cont, HIGH);
  sinceIgn=0;
  }
  }
  
  //Serial3.println("AT");
  //      Serial3.println("AT+CPAS");
  //    Serial3.println("AT+CIND?");
    // Serial3.println("AT+CPMS=\"ME\"");
      
     // checkStatus();
    
    // if (delayOnce) {delay(2000); delayOnce=false;}
  
 
      bool ended=false, ending=false, starting=false, started=false;
     // Serial.println(millis()-phoneTime);
  while((millis()-phoneTime>phoneInterval)&&Serial3.available()) 
  {
      character = Serial3.read();

//start of packet
      if(character=='\n')  starting=true; 
      if(character=='+'&&starting&&!started) 
      {
      started=true; 
      starting=false;
         index = 0;
         phnData[index] = '\0';
      Serial.println(" started");
    }
          if(started&&index < maxindex-1)
         {
            phnData[index++] = character;
            phnData[index] = '\0';
         }
         
        
//end of packet
       if(character=='O') ending=true;
      if(character=='K'&&ending==true) {
      ended=true; 
    ending=false; 
    phoneTime=0;
  break;
}

         
  }
     
     if(started) for( int k=0; k<index; k++)     Serial.print(phnData[k]);
     
  if(started&&ended) {
    for( i=0; i<index; i++)     Serial.print(phnData[i]);
    Serial.println();Serial.print("endof");Serial.println(index);
    started=false; ended=false;
        
  

 isCIND=false;
 isCMGR=false; 
 isCPMS=false;
 
  if(index>10) 
  {      
  for( i=0; i<index-10; i++)
  {
    jCIND=0; jCMGR=0; jCPMS=0;
    for( j=0; j<10; j++)
    {
  if(phnData[i+j]==CIND[j]) jCIND++;
  if(phnData[i+j]==CMGR[j]) jCMGR++;
  if(phnData[i+j]==CPMS[j]) jCPMS++;
    }
    if(jCIND==7) {isCIND=true; jCIND=i; Serial.println("isCIND"); break;}
    if(jCMGR==7) {isCMGR=true; jCMGR=i; Serial.print(jCMGR);Serial.println("isCMGR"); break;}
    if(jCPMS==7) {isCPMS=true; Serial.println("isCPMS"); break;}
    Serial.print(i);
    Serial.print(",");Serial.print(jCIND);
    Serial.print(",");Serial.print(jCMGR);
    Serial.print(",");Serial.println(jCPMS);
  }

} //index>10
}//started and ended

if(isCPMS) {readSMS(); isCPMS=false;}
if(isCMGR) {for( i=jCMGR; i<index; i++) Serial.print(phnData[i]);
Serial.println();
}
  
    
    if(index==lengthSMS&&isCMGR) {
  i=jCMGR+14;
  while( phnData[i]!='\n'&&i<index) i++;
  for( j=i-5; j<i; j++) cmd[j-i+5]=phnData[j]; 
  
  encodeCommand(cmd);

  deleteSMS();
    }
  
  
  
      phnBattLev=0;
     phnSigLev=0;
     phnBattWarn=0;
     phnCharger=0;
     phnService=0;
     phnSounder=0;
     newMessage=0;
     newCall=0;
   
  
  if(isCIND) 
  {
    i=jCIND;
    while( phnData[i]!=','&&i<index) i++;
    i--;
    
     phnBattLev=atoi(&phnData[i]);
     phnSigLev=atoi(&phnData[i+2]);
     phnBattWarn=atoi(&phnData[i+4]);
     phnCharger=atoi(&phnData[i+6]);
     phnService=atoi(&phnData[i+8]);
     phnSounder=atoi(&phnData[i+10]);
     newMessage=atoi(&phnData[i+12]);
     newCall=atoi(&phnData[i+14]);
     Serial.print("phnBattLev:"); Serial.print(phnBattLev);
     Serial.print(" phnSigLev:"); Serial.print(phnSigLev);
     Serial.print(" phnBattWarn:"); Serial.print(phnBattWarn);
     Serial.print(" phnCharger:"); Serial.println(phnCharger);
     Serial.print(" phnService:"); Serial.print(phnService);
     Serial.print(" phnSounder:"); Serial.print(phnSounder);
     Serial.print(" newMessage:"); Serial.print(newMessage);
     Serial.print(" newCall:"); Serial.println(newCall);
     isCIND=false;
   }

 
  if(newMessage>0) {tPhone.disable(0);  prepareStorage();}
  
  
  
    /*if(battLevel<3) digitalWrite(r_DH11, LOW);
  else if(battLevel>4) digitalWrite(r_DH11, HIGH);*/
  
    } //loop
    
  
    
    
///////////////////////////////////////////////////////////////////////////////////////////
      //interrupt handlers
   void AlSwTriggered()
    {
      AlSwed=true;
    }
   
    void HbrkSwTriggered()
    {
delay(interruptDelay);
HbrkSwDOWN=digitalRead(p_HbrkSw)==LOW;
    }

void IsoSwTriggered()
    {
delay(interruptDelay);
IsoSwCLOSED=digitalRead(p_IsoSw)==LOW;
    }

void IgnTriggered()
    {
delay(interruptDelay);
IgnON=(IsoSwCLOSED&&digitalRead(p_Ign)==HIGH);
    }
    
    void pirAlTriggered()
    {
      if(!AlArmed) return;
      if((digitalRead(p_pirTamp)==LOW)||(digitalRead(p_pirAl)==LOW)) ntriggers++;
    }
    
        void pirTampTriggered()
        {
      if(!AlArmed) return;
      if((digitalRead(p_pirTamp)==LOW)||(digitalRead(p_pirAl)==LOW)) ntriggers++;
    }
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////
    

  
           void lightLED(int pin, int times, long millIsoSwn, long millIsoSwff) {
           
           i=0;
           
           while(i<times&&i<10){
  digitalWrite(pin, HIGH);   
  delay(millIsoSwn);  
  digitalWrite(pin, LOW);
  delay(millIsoSwff);
  i++;
           }
}
         
         
           
         void lightLED(int pin, int times) {
           
           i=0;
           
           while(i<times&&i<10){
  digitalWrite(pin, HIGH);   
  delay(100);  
  digitalWrite(pin, LOW);
  delay(100);
  i++;
           }
}


 void playSrn() {
    
    nAls++;
  digitalWrite(r_SrnOn, LOW);   
  delay(500);  
  digitalWrite(r_SrnOn, HIGH);
  delay(SrnDuration);
  digitalWrite(r_SrnOn, LOW);   
  delay(500);  
  digitalWrite(r_SrnOn, HIGH);
  delay(2*SrnDuration);
  digitalWrite(r_SrnOff, LOW);   
  delay(500);  
  digitalWrite(r_SrnOff, HIGH);
     
    ntriggers=0;
  }

    
    
void writeVoltages()
{
     String msg = "SolPan : ";
msg+=printDouble(v_SolPan, 100);
msg += "V, L-Bat.: ";
msg+=printDouble(v_BattL, 100);
msg += "V, V-Bat.: ";
msg+=printDouble(v_BattV, 100);
msg += "V";
Serial.println(msg); 
    }
    
    
    void readVoltages()
    {

    double sensorValue = double(analogRead(pv_SolPan));
    v_SolPan=sensorValue * f0;
    
    sensorValue = double(analogRead(pv_BattV));
    v_BattV=sensorValue * f1;
    
    sensorValue = double(analogRead(pv_BattL));
    v_BattL=sensorValue * f3;


    

    
    }
    
    String printDouble( double val, unsigned int precision){
// prints val with number of decimal places determine by precision
// NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
// example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)

String number;

    number= String (int(val));  //prints the int part
    number.concat("."); // print the decimal point
    
    unsigned int frac;
    if(val >= 0)
        frac = (val - int(val)) * precision;
    else
        frac = (int(val)- val ) * precision;
    number.concat(frac) ;
    
    return number;
} 

      void toggleSol() {
        
        if(!SolSwedON) return;

        digitalWrite(r_SolL, LOW);
        digitalWrite(r_SolV, LOW);
        
        Serial.println("waiting to read Sol");
        
        delay(SolDelay);
        
        readVoltages();
        
        if(v_SolPan>v_SolPanMin)
        {
        if(whichBattIsCharged==0||whichBattIsCharged==2) chargeSolL();
        else if(whichBattIsCharged==1)                      chargeSolV();
        }
  writeVoltages();
}   

   void chargeSolL(){
  Serial.println("will charge SolL"); 
  digitalWrite(r_SolL, HIGH);
  digitalWrite(r_SolV, LOW);
  whichBattIsCharged=1;
   }
   
      void chargeSolV(){
  Serial.println("will charge SolL"); 
  digitalWrite(r_SolV, HIGH);
  digitalWrite(r_SolL, LOW);
  whichBattIsCharged=2;
   }
         
   void shutSol(){
  Serial.println("shut down Sol");
  digitalWrite(r_SolL, LOW);
  digitalWrite(r_SolV, LOW);
  whichBattIsCharged=0;
  }


void encodeCommand(char* cmd)		
{

int num;

     if( cmd[1] =='0') num =  0;
else if( cmd[1] =='1') num = 10;
else if( cmd[1] =='2') num = 20;
else if( cmd[1] =='3') num = 30;
else if( cmd[1] =='4') num = 40;
else if( cmd[1] =='5') num = 50;
else if( cmd[1] =='6') num = 60;
else if( cmd[1] =='7') num = 70;
else if( cmd[1] =='8') num = 80;
else if( cmd[1] =='9') num = 90;
else		return;
	
		
if( cmd[3] =='8')	{if( cmd[0] =='3')	num+=0;
	                if( cmd[0] =='B')	num+=1;}
else if( cmd[3] =='9')	{if( cmd[0] =='3')	num+=2;
	                if( cmd[0] =='B')	num+=3;}
else if( cmd[3] =='A')	{if( cmd[0] =='3')	num+=4;
	                if( cmd[0] =='B')	num+=5;}
else if( cmd[3] =='B')	{if( cmd[0] =='3')	num+=6;
	                if( cmd[0] =='B')	num+=7;}
else if( cmd[3] =='C')	{if( cmd[0] =='3')	num+=8;
	                if( cmd[0] =='B')	num+=9;}
else		return;

if(num<5||num==14||num==15||(num>17&&num<22)) {Serial.println("protected pin"); return;}

// toggle digital pins
if(num<54) {digitalWrite(num,!digitalRead(num)); 
Serial.print("Pin ");Serial.print(num); Serial.print(" is ");  Serial.println(digitalRead(num)); return;}
else if(num==90) reportReports();
else if(num==94) reportPins();
else if(num==95) reportal();
else if(num==96) reportVoltages();
else if(num==97) reportVariables();
else if(num==98) reportDigital();
else if(num==99) reportAll();	

}		

void reportReports(){
  String msg = "";
msg+="reportPins(94),";
msg+="reportal(95),";
msg+="reportVoltages(96),";
msg+="reportVariables(97,)";
msg+="reportDigital(98),";
msg+="reportAll(99)";

Serial.println(msg);
Serial.println(msg.length());
}

void reportAll(){
reportal();
reportVoltages();
reportVariables();
reportDigital();
}
void reportal(){}
void reportVoltages(){}
void reportVariables(){};

void reportDigital(){

String msg = "";

  Serial.println(msg);
  
//const  int p_phone2Tx   = 0; //Rx0 B3  -> UTB3
//const  int p_phone2Rx   = 1; //Tx0 B8  -> //UTB8
 //interrupts
msg+="02:"; //interrupt 0 G2/G7->Volt. Reducer
msg+=digitalRead(p_pirAl);
msg+=", 03:"; //interrupt 1 G2/G7->Volt. Reducer
msg+=digitalRead(p_pirTamp);
msg+=", 04:"; //grey from ROV57
msg+=digitalRead(p_ContSw);

//uln2003-2
msg+=", 05:"; //ULN2-3
msg+=digitalRead(r_Phone);
msg+=", 06:"; //ULN2-2
msg+=digitalRead(r_Sink);
msg+=", 07:"; //ULN2-1
msg+=digitalRead(r_Cont);

//Block 2:
//uln2003-3
msg+=", 08:"; //ULN3-4
msg+=digitalRead(r_SolL);
msg+=", 09:"; //ULN3-3
msg+=digitalRead(r_SolV);
//rel5v
msg+=", 10:";
msg+=digitalRead(r_SrnOn);
msg+=", 11:";
msg+=digitalRead(r_SrnOff);
msg+=", 12:";
msg+=digitalRead(r_DH11);
//const  int r_spare5v = 13;

// Block 3:
//const  int p_phoneRx(14); //G11 ->ROG1
//const  int p_phoneTx     = 15; //G10 ->ROG5
//const  int p_ustapEcho   = 16; //G4 ->ROG2
//const  int p_ustapTrigger= 17; //G3 ->ROG6
//interrupts
msg+=", 18:"; //  interrupt5 white from ROV5
msg+=digitalRead(p_IsoSw);
msg+=", 19:"; //  interrupt4 blue A6->Volt. Reducer
msg+=digitalRead(p_Ign);
msg+=", 20:"; //  interrupt3 purple from ROV5
msg+=digitalRead(p_HbrkSw);
msg+=", 21:"; //  interrupt2 green from RUC5
msg+=digitalRead(p_AlSw);

// Block 4:
//const  int p_B6  = 23; // UTB6
msg+=", 25:";
msg+=digitalRead(p_SolSw);


//Empty
//22x, 24x, ..., 28x
// 30x->39x

//Block 5:
//uln2003-1
msg+=", 40:";
msg+=digitalRead(r_AlPow);
//const  int r_Spare    = 41; //ULN1-2
//const  int p_LED5       = 42;
//const  int r_Spare    = 43; //ULN1-3
//const  int p_LED4       = 44;
//const  int p_usrTrigger = 45; // G8->ROG3
//const  int p_LED3       = 46;
//const  int p_usrEcho    = 47; //G6->ROG7
//const  int p_LED2       = 48;
//49x, spare at RUC
msg+=", 50:";
msg+=digitalRead(p_AlLED);

Serial.println(msg);
Serial.println(msg.length());

  
}


void reportPins(){
String msg="";

  Serial.println(msg);
  
//const  int p_phone2Tx   = 0; //Rx0 B3  -> UTB3
//const  int p_phone2Rx   = 1; //Tx0 B8  -> //UTB8
 //interrupts
msg+="p_pirAl02_I"; //interrupt 0 G2/G7->Volt. Reducer
msg+=",p_pirTamp03_I"; //interrupt 1 G2/G7->Volt. Reducer
msg+=",p_ContSw04_I"; //grey from ROV57

//uln2003-2
msg+=",r_Phone05_O"; //ULN2-3
msg+=",r_Sink06_O"; //ULN2-2
msg+=",r_Cont07_O"; //ULN2-1

//Block 2:
//uln2003-3
msg+=",r_SolL08_O"; //ULN3-4
msg+=",r_SolV09_O"; //ULN3-3
//rel5v
msg+=",r_SrnOn10_O";
msg+=",r_SrnOff11_O";
msg+=",r_DH1112_O";

//const  int r_spare5v = 13;

// Block 3:
//const  int p_phoneRx(14); //G11 ->ROG1
//const  int p_phoneTx     = 15; //G10 ->ROG5
//const  int p_ustapEcho   = 16; //G4 ->ROG2
//const  int p_ustapTrigger= 17; //G3 ->ROG6
//interrupts
msg+=",p_IsoSw18_I"; //  interrupt5 white from ROV5
msg+=",p_Ign19_I"; //  interrupt4 blue A6->Volt. Reducer
msg+=",p_HbrkSw20_I"; //  interrupt3 purple from ROV5
msg+=",p_AlSw21_I"; //  interrupt2 green from RUC5

//22x, 23x

// Block 4:
//const  int p_B6  = 24; // UTB6
msg+=",p_SolSw25_I";



//Empty

// 26x->39x

//Block 5:
//uln2003-1
msg+=",r_AlPow40_O";
//const  int r_Spare    = 41; //ULN1-2
//const  int p_LED5       = 42;
//const  int r_Spare    = 43; //ULN1-3
//const  int p_LED4       = 44;
//const  int p_usrTrigger = 45; // G8->ROG3
//const  int p_LED3       = 46;
//const  int p_usrEcho    = 47; //G6->ROG7
//const  int p_LED2       = 48;
//49x, spare at RUC
msg+=",p_AlLED50_O";

Serial.println(msg);
Serial.println(msg.length());
}

    void deleteSMS() {
      prepareStorage();
      
      Serial3.println("AT+CMGD=0");
}


    void listentoCall() {
      Serial3.println("AT+CPAS");
}

    void checkStatus() {
      
      Serial3.println("AT+CIND?");
      phoneTime=millis();
      Serial.println("checking Status");
}

    void prepareStorage() {
      Serial.println("preparing Storage");
      Serial3.println("AT+CPMS=\"ME\"");
     phoneTime=millis();
    }
      
    void readSMS() {
      Serial.println("reading SMS");
      //Serial3.println("AT+CMGR=1");
      Serial3.println("AT+CMGL=4");
     phoneTime=millis();
}


       void answerCall() {
      Serial3.println("ATA");
} 

    void sendSMS(String msgTxt ) {
      
String msg = "AT*EAPP=0,0,\"";
msg+=msgTxt;
msg += "\"";
Serial3.println(msg);

delay(3000);

Serial3.println("AT+CKPD=\"S\"");

delay(3000);

Serial3.println("AT+CKPD=\"07772259300\"");

delay(3000);

Serial3.println("AT+CKPD=\"S\"");
}


    void deleteALLSMS() {
      
      delay(3000);
      
Serial3.println("AT+CKPD=\">\"");
delay(500);

Serial3.println("AT+CKPD=\">\"");
delay(500);

Serial3.println("AT+CKPD=\"v\"");
delay(500);

Serial3.println("AT+CKPD=\"v\"");
delay(500);

Serial3.println("AT+CKPD=\"S\"");

delay(1000);

Serial3.println("AT+CKPD=\"S\"");

delay(1000);

Serial3.println("AT+CKPD=\"v\"");
delay(500);
Serial3.println("AT+CKPD=\"v\"");
delay(500);
Serial3.println("AT+CKPD=\"v\"");
delay(500);
Serial3.println("AT+CKPD=\"v\"");
delay(500);
Serial3.println("AT+CKPD=\"v\"");
delay(500);

Serial3.println("AT+CKPD=\"S\"");

delay(1000);

Serial3.println("AT+CKPD=\"S\"");
}

  


