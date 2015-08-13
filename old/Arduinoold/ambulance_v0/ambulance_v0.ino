
#include <SimpleTimer.h>
SimpleTimer timer;

const int board=0; //0 uno, 1 mega


const  int p_solarPanel = A0;
const  int p_lbattery   = A1; 
const  int p_vbattery   = A2; 
const  int p_solarCont  = A3; 

const  int p_ignition   = 2; //d2, interrupt 0
const  int p_pirAlarm   = 3; //d3, interrupt 1
const  int p_pirTamper  = 4;

//uln2003-1
const  int rel_contactor = 5; 
const  int rel_sink      = 6;
const  int rel_phone     = 7;

//uln2003-2
const  int rel_solarL    = 8;
const  int rel_solarV    = 9;
const  int rel_spare     = 10;

const  int rel_sirenOn   = 11;
const  int rel_sirenOff  = 12;

//ATTTACH TO INTERRUPT LATER
const  int alarmSwitch  = 13;

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
const double f0=0.02600;
const double f1=0.01532;
const double f2=0.01580;
const double f3=0.01555;
const double f4=0.01445;

const int ignitionDelay=2000, alarmDelay=3000, nalarmsDelay=5, sirenDuration=2000;

volatile int v_pirAlarm, v_pirTamper, v_ignition, nalarms;
volatile bool ignitiondelay, isTampered=false, isAlarmed=false;
volatile unsigned long ignitionTime, alarmTime;


unsigned long sinceIgnition, sinceAlarm;
double v_solarPanel , v_lbattery  , v_vbattery , v_solarCont;
bool alarmArmed=true;
int i, alarmctr=0;


    void setup()
    {
    Serial.begin(9600); //initialize serial communication
    
        //relay driver
     for (i = 5; i < 13; i++) pinMode(i, OUTPUT);
    
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



 
    }





    void loop()
    {
readVoltages();

if(ignitionTime>0)
{
  
  sinceIgnition=millis()-ignitionTime;
  if(sinceIgnition>ignitionDelay) {
    digitalWrite(rel_contactor, HIGH);
    sinceIgnition=0;
  }
 }
else sinceIgnition=0;


if(alarmArmed)
{
 // isTampered=((digitalRead(p_pirTamper)==LOW)||(digitalRead(p_pirAlarm)==LOW));
        
if(isTampered||alarmTime>0)
{
  sinceAlarm=millis()-alarmTime;
  

  if(alarmctr>5) alarmArmed=false;
  if(isTampered||(sinceAlarm>alarmDelay&&nalarms>nalarmsDelay)) {
    
    alarmctr++;
    soundSiren();
  delay(sirenDuration);
    soundSiren();
  delay(2*sirenDuration);
    shutSiren();
     
    sinceAlarm=0;
    nalarms=0;
  }
 }
else sinceAlarm=0;
}


writeVoltages();

delay(500);

    }
    
    
    
    
    
    
     void soundSiren() {
      digitalWrite(rel_sirenOn, HIGH);   
  delay(500);   
  digitalWrite(rel_sirenOn, LOW);
}

 void shutSiren() {
      digitalWrite(rel_sirenOff, HIGH);   
  delay(500);   
  digitalWrite(rel_sirenOff, LOW);
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
  digitalWrite(rel_contactor, LOW);
  ignitionTime=0;
}
    }
    
    void pirAlarmTrigger()
    {
      if(alarmArmed){
isAlarmed=((digitalRead(p_pirTamper)==LOW)||(digitalRead(p_pirAlarm)==LOW));

if(isAlarmed) 
{
 nalarms++;
  alarmTime=millis();
}
else
{
  alarmTime=0;
}
      } }
      
    
void writeVoltages()
{
     String msg = "SolarPanel : ";
msg+=printDouble(v_solarPanel, 100);
msg+= "V, Solar Controller: ";
msg+=printDouble(v_solarCont, 100);
msg += "V, Leis.Bat.: ";
msg+=printDouble(v_lbattery, 100);
msg += "V, Veh.Bat.: ";
msg+=printDouble(v_vbattery, 100);
msg += "V";
Serial.println(msg); 
    }
    
    
    void readVoltages()
    {

    double sensorValue = double(analogRead(p_solarPanel));
    v_solarPanel=sensorValue * f0;
    
    sensorValue = double(analogRead(p_lbattery));
    v_lbattery=sensorValue * f1;

    sensorValue = double(analogRead(p_vbattery));
    v_vbattery=sensorValue * f2;
    
    sensorValue = double(analogRead(p_solarCont));
    v_solarCont=sensorValue * f3;
    
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

