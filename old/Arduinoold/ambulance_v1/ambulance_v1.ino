
#include <SimpleTimer.h>
SimpleTimer timerSolar;

const int board=0; //0 uno, 1 mega


const  int p_solarPanel   = A0;
const  int p_vbattery     = A1; 
const  int p_lbattery     = A3;
 

const  int p_ignition     = 2; //d2, interrupt 0
const  int p_pirAlarm     = 3; //d3, interrupt 1
const  int p_pirTamper    = 4;



//uln2003-2
const  int rel_solarV     = 5;
const  int rel_sirenPower = 6; 
const  int rel_solarL     = 7;

//uln2003-1
const  int rel_Contactor    = 8; //ok
//const  int rel_Phone    = 22; //ok
//const  int rel_Sink   = 23;


//ATTTACH TO INTERRUPT LATER
const  int p_alarmSwitch  = 9;

const  int rel_sirenOn   = 10;
const  int rel_sirenOff  = 11;
const  int rel_DH11      = 12;
const  int rel_spare5v   = 13;

/*if(board==1) //mega
{
//G:
const  int p_ustapEcho     = 14; 
const  int p_ustapTrigger  = 15; 
const  int p_usrearTrigger = 16; 
const  int p_usrearEcho    = 17; 
const  int p_phoneRx       = 18; 
const  int p_phoneTx       = 19; 
}*/


//voltage multipliers
const double f0=0.02600,f1=0.01532, f2=0.01580,
             f3=0.01555, f4=0.01445, v_solarPanelMin=4.0;

const int ignitionDelay=2000, alarmpowerDelay=3000, ntriggersDelay=1, sirenDuration=5000, maxnalarms=4, solarCheckInterval=6000000, solarDelay=30000;

volatile int v_pirAlarm, v_pirTamper, v_ignition, ntriggers;
volatile bool ignitiondelay;
volatile unsigned long ignitionTime;


unsigned long sinceIgnition;
double v_solarPanel , v_lbattery  , v_vbattery , v_solarCont;
bool alarmArmed=false, alarmSwitched=false, isTriggered=false;
int i, nalarms=0;


    void setup()
    {
    Serial.begin(9600); //initialize serial communication
    
    timerSolar.setInterval(solarCheckInterval, checkSolar);
    
     //relay driver
     for (i = 5; i < 9; i++) {pinMode(i, OUTPUT); digitalWrite(i, LOW);}
     for (i = 10; i < 14; i++) {pinMode(i, OUTPUT); digitalWrite(i, HIGH);}
    
    //G:
 /*   pinMode(p_ustapEcho, INPUT);
    pinMode(p_usrearEcho, INPUT);
    pinMode(p_ustapTrigger, OUTPUT);
    pinMode(p_usrearTrigger, OUTPUT);*/
    
    pinMode(p_ignition, INPUT);  
    pinMode(p_pirAlarm, INPUT);
    pinMode(p_pirTamper, INPUT);
    pinMode(p_alarmSwitch, INPUT);
    digitalWrite(p_alarmSwitch, HIGH);
    
    attachInterrupt(0, ignitionTrigger, CHANGE);
    attachInterrupt(1, pirAlarmTrigger, CHANGE); 
    
    checkSolar();
    }





    void loop()
    {

      timerSolar.run();

delay(500);

alarmSwitched=(digitalRead(p_alarmSwitch)==LOW);

if(alarmSwitched)
{
  if(!alarmArmed)
  {
delay(alarmpowerDelay);
alarmArmed=true;
nalarms=0;
ntriggers=0;
digitalWrite(rel_sirenPower, LOW);
Serial.println("alarm armed");
  }
}
else 
{
  alarmArmed=false; 
  digitalWrite(rel_sirenPower, HIGH);
  Serial.println("alarm disarmed");
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
  if((digitalRead(p_pirTamper)==LOW)||ntriggers>ntriggersDelay) {
    
    nalarms++;
    soundSiren();
  delay(sirenDuration);
    soundSiren();
  delay(2*sirenDuration);
    shutSiren();
     
    ntriggers=0;
  }
 }
}






/*if(ignitionTime>0)
{
  
  sinceIgnition=millis()-ignitionTime;
  if(sinceIgnition>ignitionDelay) {
    digitalWrite(rel_contactor, HIGH);
    sinceIgnition=0;
  }
 }
else sinceIgnition=0;
*/


    }
    
    
    
      void checkSolar() {
        
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
    
    
     void soundSiren() {
  digitalWrite(rel_sirenOn, LOW);   
  delay(500);  
  digitalWrite(rel_sirenOn, HIGH);
}

 void shutSiren() {
  digitalWrite(rel_sirenOff, LOW);   
  delay(500);  
  digitalWrite(rel_sirenOff, HIGH);
}
  

void ignitionTrigger()
    {
v_ignition=digitalRead(p_ignition);
if(v_ignition==HIGH) 
{
  digitalWrite(rel_solarL, HIGH);
  digitalWrite(rel_solarV, HIGH);
  
  ignitionTime=millis();
}
else
{
  digitalWrite(rel_solarL, LOW);
  digitalWrite(rel_solarV, LOW);
  digitalWrite(rel_Contactor, LOW);
  ignitionTime=0;
}
    }
    
    void pirAlarmTrigger()
    {
      if(!alarmArmed) return;
      
if((digitalRead(p_pirTamper)==LOW)||(digitalRead(p_pirAlarm)==LOW)) ntriggers++;

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

