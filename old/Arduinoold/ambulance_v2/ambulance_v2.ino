#include <SimpleTimer.h>
SimpleTimer t;

const int debug=1; //0 uno, 1 mega

//voltages
const  int p_solarPanel   = A0; //blue
//const  int p_spare      = A2; //yellow
const  int p_vbattery     = A1; //orange
const  int p_lbattery     = A3; //green
 
 //interrupts
const  int p_pirAlarm     = 2; //d2, interrupt 0
const  int p_pirTamper    = 3; //d3, interrupt 1

//uln2003-1
const  int rel_Contactor  = 4; //ok
const  int rel_Sink       = 5;
const  int rel_Phone      = 6; //ok
//const  int rel_Spare    = 7;

//uln2003-2
const  int rel_solarV     = 8;
const  int rel_solarL     = 9;

//rel5v
const  int rel_sirenOn   = 10;
const  int rel_sirenOff  = 11;
const  int rel_DH11      = 12;
const  int rel_spare5v   = 13;


//G
/*const  int p_phoneRx       = 14;
const  int p_phoneTx       = 15; 
const  int p_ustapEcho     = 16; 
const  int p_ustapTrigger  = 17;*/

//interrupts
const  int p_ignition     = 18; //in red tape interrupt5
const  int p_alarmSwitch  = 19; // purple  interrupt4
const  int p_isolator     = 20; // grey  interrupt3
const  int p_handbrake    = 21; // white  interrupt2

//G:
/*const  int p_usrearTrigger = 31; 
const  int p_usrearEcho    = 33;*/
//uln2003-3
const  int rel_sirenPower = 35; 
//const  int rel_Spare    = 37;
//const  int rel_Spare    = 39;
const  int p_alarmLED    = 41;
const  int p_LED2    = 43;
//const  int p_LED3    = 45;
//const  int p_LED4    = 47;
//const  int p_LED5    = 49;

const  int p_contactor  = 51; // white

//voltage multipliers
const double f0=0.02600,f1=0.01532, f2=0.01580,
             f3=0.01555, f4=0.01445, v_solarPanelMin=4.0;

const int ignitionDelay=2000, alarmpowerDelay=100, ntriggersDelay=1, sirenDuration=5000, 
          maxnalarms=4, solarswitchInterval=6000000, solarDelay=5000, interruptDelay=500,
          durationAlarmedLED=2000;

volatile int v_pirAlarm, v_pirTamper, v_ignition, ntriggers;
volatile bool ignitiondelay, ignitionON, isolatorCLOSED, handbrakeDOWN;
volatile unsigned long ignitionTime,alarmLEDTime, elapsedAlarmLED;


unsigned long sinceIgnition;
double v_solarPanel , v_lbattery  , v_vbattery , v_solarCont;
bool alarmArmed=false, alarmSwitched=false, isTriggered=false, alarmSwitchedON=false;
int i, nalarms=0;


    void setup()
    {
    Serial.begin(9600); //initialize serial communication
    
    t.setInterval(solarswitchInterval, switchSolar);
    
     //relay driver
     //uln2003-1
     for (i = 4;  i < 8 ; i++) {pinMode(i, OUTPUT); digitalWrite(i, LOW);}
     //uln2003-2
     for (i = 8;  i < 10; i++) {pinMode(i, OUTPUT); digitalWrite(i, LOW);}
     //5V relay
     for (i = 10; i < 14; i++) {pinMode(i, OUTPUT); digitalWrite(i, HIGH);}
     //uln2003-3
      {pinMode(rel_sirenPower, OUTPUT); digitalWrite(rel_sirenPower, LOW);}
      {pinMode(p_LED2, OUTPUT); digitalWrite(p_LED2, LOW);}
      {pinMode(p_alarmLED, OUTPUT); digitalWrite(p_alarmLED, LOW);}

    
    //G:
    /*pinMode(p_ustapEcho, INPUT);
    pinMode(p_usrearEcho, INPUT);
    pinMode(p_ustapTrigger, OUTPUT);
    pinMode(p_usrearTrigger, OUTPUT);*/
    
    pinMode(p_ignition, INPUT);  
    pinMode(p_pirAlarm, INPUT);
    pinMode(p_pirTamper, INPUT);
    
    pinMode(p_alarmSwitch, INPUT);
    digitalWrite(p_alarmSwitch, HIGH);
    
    pinMode(p_isolator, INPUT);
    digitalWrite(p_isolator, HIGH);
    
    pinMode(p_contactor, INPUT);
    digitalWrite(p_contactor, HIGH);
    
    pinMode(p_handbrake, INPUT);
    digitalWrite(p_handbrake, HIGH);
    
    attachInterrupt(0, pirAlarmTriggered,    CHANGE); //p_pirAlarm 
    attachInterrupt(1, pirTamperTriggered,    CHANGE); //p_pirTamper //MAY BE MAKE LOW LATER
    attachInterrupt(2, handbrakeTriggered,    CHANGE); //p_handbrake
    attachInterrupt(3, isolatorTriggered,    CHANGE); //p_isolator
    attachInterrupt(4, alarmSwitchTriggered, CHANGE); //p_alarmSwitch
    attachInterrupt(5, ignitionTriggered,    CHANGE); //p_ignition
    
    
   // switchSolar();
       
    }


/////////////////////////////////////////////////////////////////////////////


    void loop()
    {
      

       t.run();
      
    if(handbrakeDOWN)  {lightLED(p_alarmLED,2);  Serial.println("handbrakeDOWN");}
    if(isolatorCLOSED)  {lightLED(p_alarmLED,3);  Serial.println("isolatorCLOSED");}
    if(ignitionON)  {lightLED(p_alarmLED,4); Serial.println("ignitionON");}
    if(alarmSwitched) { lightLED(p_alarmLED,4); Serial.println("alarmSwitched");}

if(alarmSwitched){alarmSwitched=false; delay(interruptDelay); alarmSwitchedON=(digitalRead(p_alarmSwitch)==LOW);}

if(alarmSwitchedON)
{
  if(!alarmArmed)
  {
    lightLED(p_alarmLED,4,alarmpowerDelay, 8*alarmpowerDelay);
    lightLED(p_alarmLED,4,alarmpowerDelay, 4*alarmpowerDelay);
    lightLED(p_alarmLED,8,alarmpowerDelay,2*alarmpowerDelay);
    
alarmArmed=true;
nalarms=0;
ntriggers=0;
alarmLEDTime=millis();
digitalWrite(rel_sirenPower, LOW);
//Serial.println("alarm armed");
  }
  else
  {
  elapsedAlarmLED=millis()-alarmLEDTime;
    if(elapsedAlarmLED>durationAlarmedLED){lightLED(p_alarmLED,1); alarmLEDTime=millis();} 
  
  }
}
else 
{
  alarmArmed=false; 
  digitalWrite(rel_sirenPower, HIGH);
 // Serial.println("alarm disarmed");
}


Serial.print(digitalRead(p_ignition));
Serial.print(digitalRead(p_pirAlarm));
Serial.print(digitalRead(p_pirTamper));
 Serial.print(" nalarms:"); Serial.print(nalarms);
 Serial.print(" isTriggered:");Serial.print(isTriggered);
 Serial.print(" ntriggers:"); Serial.println(ntriggers);

  
  
if(alarmArmed&&nalarms<maxnalarms)
{
  
 isTriggered=((digitalRead(p_pirTamper)==LOW)||(digitalRead(p_pirAlarm)==LOW));
  
if(isTriggered)
{
  lightLED(p_alarmLED,4); 
  if((digitalRead(p_pirTamper)==LOW)||ntriggers>ntriggersDelay) playSiren();
 

 }
}
/*

// switch off solar panels in case of ignition
if(isolatorCLOSED) 
{
  if(ignitionTime>0)
{
  
  sinceIgnition=millis()-ignitionTime;
  if(sinceIgnition>ignitionDelay) {
    digitalWrite(rel_Contactor, HIGH);
    sinceIgnition=0;
  }
 }
else sinceIgnition=0;


  if(ignitionON)
  {
  digitalWrite(rel_solarL, LOW);
  digitalWrite(rel_solarV, LOW);
  
  ignitionTime=millis();
  }
  else ignitionTime=0;
}
else
{
  digitalWrite(rel_solarL, HIGH);
  digitalWrite(rel_solarV, LOW);
  digitalWrite(rel_Contactor, LOW);
}
*/

    }
    
///////////////////////////////////////////////////////////////////////////////////////////
      //interrupt handlers
   void alarmSwitchTriggered()
    {
      alarmSwitched=true;
    }
   
    void handbrakeTriggered()
    {
delay(interruptDelay);
handbrakeDOWN=digitalRead(p_handbrake)==LOW;
    }

void isolatorTriggered()
    {
delay(interruptDelay);
isolatorCLOSED=digitalRead(p_isolator)==LOW;
    }

void ignitionTriggered()
    {
delay(interruptDelay);
ignitionON=digitalRead(p_ignition)==HIGH;
    }
    
    void pirAlarmTriggered()
    {
      if(!alarmArmed) return;
      if((digitalRead(p_pirTamper)==LOW)||(digitalRead(p_pirAlarm)==LOW)) ntriggers++;
    }
    
        void pirTamperTriggered()
        {
      if(!alarmArmed) return;
      if((digitalRead(p_pirTamper)==LOW)||(digitalRead(p_pirAlarm)==LOW)) ntriggers++;
    }
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////
    
      void switchSolar() {
        
        int whichBatteryIsCharged=0;
        
        if      (digitalRead(rel_solarV)==HIGH) whichBatteryIsCharged=1;
        else if (digitalRead(rel_solarL)==HIGH) whichBatteryIsCharged=2;
        
        digitalWrite(rel_solarL, LOW);
        digitalWrite(rel_solarV, LOW);
        
        Serial.println("waiting to read solar");
        
        delay(solarDelay);
        
        readVoltages();
        
        if(v_solarPanel>v_solarPanelMin)
        {
        if(whichBatteryIsCharged==0||whichBatteryIsCharged==1) 
        {
          Serial.println("will charge solarL"); 
          digitalWrite(rel_solarL, HIGH);
        }
        else if(whichBatteryIsCharged==2) 
        {
          Serial.println("will charge solarV");
          digitalWrite(rel_solarV, HIGH);
          
        }
        }

 
  writeVoltages();
}   
  
           void lightLED(int pin, int times, long millison, long millisoff) {
           
           i=0;
           
           while(i<times&&i<10){
  digitalWrite(pin, HIGH);   
  delay(millison);  
  digitalWrite(pin, LOW);
  delay(millisoff);
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


 void playSiren() {
    
    nalarms++;
  digitalWrite(rel_sirenOn, LOW);   
  delay(500);  
  digitalWrite(rel_sirenOn, HIGH);
  delay(sirenDuration);
  digitalWrite(rel_sirenOn, LOW);   
  delay(500);  
  digitalWrite(rel_sirenOn, HIGH);
  delay(2*sirenDuration);
  digitalWrite(rel_sirenOff, LOW);   
  delay(500);  
  digitalWrite(rel_sirenOff, HIGH);
     
    ntriggers=0;
  }

    
    
void writeVoltages()
{
     String msg = "SolarPanel : ";
msg+=printDouble(v_solarPanel, 100);
msg += "V, L-Bat.: ";
msg+=printDouble(v_lbattery, 100);
msg += "V, V-Bat.: ";
msg+=printDouble(v_vbattery, 100);
msg += "V";
Serial.println(msg); 
    }
    
    
    void readVoltages()
    {

    double sensorValue = double(analogRead(p_solarPanel));
    v_solarPanel=sensorValue * f0;
    
    sensorValue = double(analogRead(p_vbattery));
    v_vbattery=sensorValue * f1;
    
    sensorValue = double(analogRead(p_lbattery));
    v_lbattery=sensorValue * f3;


    

    
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


/*
 void listentoCall() {
      Serial1.println("AT+CPAS");
}

    void listentoSMS() {
      Serial1.println("AT+CPMS=\"ME\"");
      delay(1000);
      Serial1.println("AT+CMGR=1");

}

    void deleteSMS() {
      Serial1.println("AT+CMGD=1");
}


       void answerCall() {
      Serial1.println("ATA");
} 

    void sendSMS(String msgTxt ) {
      
String msg = "AT*EAPP=0,0,\"";
msg+=msgTxt;
msg += "\"";
Serial1.println(msg);

delay(3000);

Serial1.println("AT+CKPD=\"S\"");

delay(3000);

Serial1.println("AT+CKPD=\"07772259300\"");

delay(3000);

Serial1.println("AT+CKPD=\"S\"");
}

*/

