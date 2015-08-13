#include <SimpleTimer.h>
#include <SoftwareSerial.h>

const int lengthSMS=134, delayPhone=1; //for K700i
//const int lengthSMS=132, delayPhone=5; //for R520m

//voltages
const  int pv_SolPan   = A0; //blue
const  int pv_BattV     = A1; //orange
//const  int pv_spare      = A2; //yellow
const  int pv_BattL     = A3; //green
//const  int pv_spare2     = A4; //yellow
//A8x->A15x

//Block 1:
// serial0, might use for second phone
//const  int p_phone2Tx   = 0; //Rx0
//const  int p_phone2Rx   = 1; //Tx0
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
const  int p_HbrkSwSw    = 20; //  interrupt3 purple from ROV5
const  int p_AlSw  = 21; //  interrupt2 green from RUC5

// Block 4:
//const  int p_B6  = 23; // UTB6
const  int p_SolSw  = 25; // B2->SW3->A2->GND
//const  int p_B3  = 27; // UTB3
//const  int p_B8  = 29; //UTB8

//Empty
//22x, 24x, ..., 28x
// 30x->39x

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

SoftwareSerial Phone(10, 11); // RX, TX 10 Blue, 11 Brown


SimpleTimer timer;

    void setup()
    {

    Serial.begin(9600); //initialize serial communication
    Phone.begin(9600); //initialize serial communication
   /* Serial2.begin(9600); //initialize serial communication
    Serial3.begin(9600); //initialize serial communication*/
    
    pinMode(13, OUTPUT);
    
   // timer.setInterval(5000, checkStatus);
//timer.setInterval(5000, listentoCall);
timer.setInterval(10000, readSMS);

Phone.println("AT+CPMS=\"ME\"");
deleteSMS();
//reportAll();
    }

    void loop()
    {

      timer.run();
     // timerSMS.run();
      
      //readSerial(content);
      
      //void readSerial(String content)

char fromPhone[160], CIND[10]="+CIND: /0", CMGR[10]="+CMGR: /0", smsReceived[16]="0791447728008/0", cmd[4];
  char character;
  int nfromPhone, i,j, jCIND=0, jCMGR;
  int battLevel,sigLevel,battWarning, newMessage, newCall;
  
  nfromPhone=0;
 // while(1){
  while(Phone.available()) 
  {
      character = Phone.read();
      //if(character=='\n') {
      //Serial.println("Enter");
  //break;}
      fromPhone[nfromPhone++]=character;
      delay(delayPhone);
 
  }
  //  else break;
 // }
  
   for( i=0; i<nfromPhone; i++)     Serial.print(fromPhone[i]);

  /*if(nfromPhone>10) 
  {
 
      for( i=0; i<nfromPhone; i++)     Serial.print(fromPhone[i]);
      
  for( i=0; i<nfromPhone-10; i++)
  {
    jCIND=0, jCMGR=0;
    for( j=0; j<10; j++)
    {
  if(fromPhone[i+j]==CIND[j]) jCIND++;
  if(fromPhone[i+j]==CMGR[j]) jCMGR++;
    }
    if(jCIND==7||jCMGR==7)break;
}
//Serial.print("endof");Serial.println(nfromPhone);Serial.println();
}

  
    
    if(nfromPhone==lengthSMS&&jCMGR==7) {
  jCMGR=i;
  i=jCMGR+14;
  while( fromPhone[i]!='\n'&&i<nfromPhone) i++;
  for( j=i-5; j<i; j++) cmd[j-i+5]=fromPhone[j]; 
  
  encodeCommand(cmd);

  deleteSMS();
    }
  
  
  
  
  
  
  if(jCIND==7) 
  {
     battLevel=atoi(&fromPhone[9]);
     sigLevel=atoi(&fromPhone[11]);
     battWarning=atoi(&fromPhone[13]);
     newMessage=atoi(&fromPhone[21]);
     newCall=atoi(&fromPhone[23]);
     Serial.print("battLevel:"); Serial.print(battLevel);
     Serial.print(" sigLevel:"); Serial.print(sigLevel);
     Serial.print(" battWarning:"); Serial.print(battWarning);
     Serial.print(" newMessage:"); Serial.print(newMessage);
     Serial.print(" newCall:"); Serial.println(newCall);
     jCIND=0;

}

 
  if(newMessage>0) 
 */ 
// readSMS();
//  delay(2000);

 
  
  
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
  
  //const  int p_phone2Tx   = 0; //Rx0
//const  int p_phone2Rx   = 1; //Tx0
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
msg+=digitalRead(p_HbrkSwSw);
msg+=", 21:"; //  interrupt2 green from RUC5
msg+=digitalRead(p_AlSw);

// Block 4:
//const  int p_B6  = 23; // UTB6
msg+=", 25:";
msg+=digitalRead(p_SolSw);
//const  int p_B3  = 27; // UTB3
//const  int p_B8  = 29; //UTB8

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
  
  //const  int p_phone2Tx   = 0; //Rx0
//const  int p_phone2Rx   = 1; //Tx0
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
msg+=",p_HbrkSwSw20_I"; //  interrupt3 purple from ROV5
msg+=",p_AlSw21_I"; //  interrupt2 green from RUC5

// Block 4:
//const  int p_B6  = 23; // UTB6
msg+=",p_SolSw25_I";
//const  int p_B3  = 27; // UTB3
//const  int p_B8  = 29; //UTB8

//Empty
//22x, 24x, ..., 28x
// 30x->39x

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


    void listentoCall() {
      Phone.println("AT+CPAS");
}

    void checkStatus() {
      Phone.println("AT+CIND?");
}

    void readSMS() {
      Phone.println("AT+CPMS=\"ME\"");
      delay(1000);
      Phone.println("AT+CMGR=1");


}

    void deleteSMS() {
      Phone.println("AT+CMGD=1");
}


       void answerCall() {
      Phone.println("ATA");
} 

    void sendSMS(String msgTxt ) {
      
String msg = "AT*EAPP=0,0,\"";
msg+=msgTxt;
msg += "\"";
Phone.println(msg);

delay(3000);

Phone.println("AT+CKPD=\"S\"");

delay(3000);

Phone.println("AT+CKPD=\"07772259300\"");

delay(3000);

Phone.println("AT+CKPD=\"S\"");
}

  


