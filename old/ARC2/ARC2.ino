#include "AmbulanceC.h"
#include <EEPROM.h>
#include "Phone.h"
#include "MyString.h"
#include "TimerList.h"
#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include "Tristar.h"
#include <tristarSlave.h>

modbusDevice regBank;
tristarSlave slave;
TimerList t;

 TimerList::MyTimer *timerReset;
const unsigned long resetInterval=120000; //=43200000;

Phone phone;
AmbulanceC arc;

const bool prnt=false;


void setup()
{
//new interrupts
/* Pin to interrupt map:67

PCINT 00 01 02 03 = PCIE0 = pcmsk0
D     53 52 51 50 = PCIE0 = pcmsk0
*/
	//new interrupts
MCUCR = (1<<ISC00) | (1<<ISC01);
PCICR |= (1<<PCIE0);
PCMSK0 |= (1<<PCINT0);
PCMSK0 |= (1<<PCINT1);
PCMSK0 |= (1<<PCINT2);
PCMSK0 |= (1<<PCINT3);

int i;
regBank.setId(1);
for(i = 0 ; i < 29 ; i++) regBank.add(i+30001); 
for( i = 0 ; i < 29 ; i++) regBank.add(i+40001); 
slave._device = &regBank; 

if(prnt) Serial.begin(9600); 

slave.setBaud(9600); 
slave.setSerial(0);  

arc.initializePins();

RESETALL();

}


///////loop////////////////////////////////////////////////////////////////////////////////
void loop()
{
t.run();

if(!arc.detachAll){
	arc.run();

	if(phone.enabled()) phone.run();

	if(slave.enabled())  slave.run();

	
}

} //loop



void RESETALL(){
int i;
	if(prnt){
Serial.println();
Serial.println("*********************************");
Serial.println("******RESETTING******************");
Serial.println("*********************************");
	}
	checkmem();


if(prnt) Serial.println("******TIMERS*********************");
t.reset();
timerReset=t.setInterval(resetInterval, RESETALL);
t.setInterval(2000,checkmem); 
checkmem();

if(prnt) Serial.println("******PHONE**********************");
phone.reset(true, true, 9600);
checkmem();

if(prnt) Serial.println("******REGBANK**********************");
for(i = 0 ; i < 29 ; i++) regBank.set(i+30001,0);
for( i = 0 ; i < 29 ; i++) regBank.set(i+40001,0);
checkmem();

if(prnt) Serial.println("******BOARD**********************");
arc.reset();
checkmem();

if(prnt){
Serial.println("*********************************");
Serial.println("******FINISHED******************");
Serial.println("*********************************");
Serial.println();
	}
}

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void checkmem(){
	if(prnt) {
		Serial.println();
		Serial.print("Free memory:"); 
		Serial.println(freeRam ());
	}
}



ISR(PCINT0_vect) {arc.itrptSrc=99;}


byte READEEPROM(int index){return EEPROM.read(index);}

void REPORTMEMORY(int byteOffset){slave.writeMemorytoString(byteOffset,&phone.SMS);}
void REPORTALARMS(int byteOffset){slave.writeAlarmstoString(byteOffset,&phone.SMS);}
void TIMEOUT(){phone.timeout();}
void CHECKPHONE(){phone.initializePhone();}
void CHECKMONITOR(){phone.initializeMonitor();}

void BLINKSCENE(){arc.blinkScene();}
void BLINKALLED(){arc.blinkAlLED();}
void ALLEDL(){arc.AlLEDL();}
void PLAYSRN(){arc.playSrn();}
void SRNON(){arc.srnOn();}
void SRNONL(){arc.srnOnL();}
void SRNOFF(){arc.srnOff();}
void SRNOFFL(){arc.srnOffL();}
void FREESRNSTATUS(){arc.freesrnStatus();}
void EMERGENCYOFF(){arc.EmergencyOFF();}
void MANAGEALARM(){arc.manageAlarm();}
void PIRTAMPINTERRUPT(){arc.pirTampInterrupt();}
void PIRALINTERRUPT(){arc.pirAlInterrupt();}
void TOTALINTERRUPT(){arc.TotAlInterrupt();}
void PERALINTERRUPT(){arc.PerAlInterrupt();}
void DOORSWINTERRUPT(){arc.DoorSwInterrupt();}
void REPORTAL(MyString * _SMS){arc.reportAl(_SMS);}
void REPORTSTATE(MyString * _SMS){arc.reportState(_SMS);}
void REPORTOUTPUTS(MyString *_SMS){arc.reportOutputs(_SMS);}
void LISTENING(bool isListening){ 
	if(isListening)arc.StatusLEDOFF();
	else arc.StatusLEDOFF();
	}
void SUCCESS(){
	arc.StatusLEDON();
	delay(50);
	arc.StatusLEDOFF();
	delay(50);
	arc.StatusLEDON();
	delay(50);
	arc.StatusLEDOFF();}





void REPORTTIMERSINCE(MyString * _SMS){
  _SMS->append("since. "); 
	if(timerReset!=NULL) _SMS->append("tmrRset: "); if(prnt) Serial.print("timerReset: "); 
	_SMS->appendTime(timerReset->since()); // 9+8+27=44
}
void REPORTTIMERTOGO(MyString * _SMS){//43
  _SMS->append("togo. ");
	if(timerReset!=NULL) _SMS->append("tmrRset: "); if(prnt) Serial.print("timerReset: "); 
	_SMS->appendTime(resetInterval-timerReset->since());
}


void DISABLESLAVE(){slave.disable();}
void ENABLESLAVE() {slave.enable();}
void WRITECOMMAND(int nexe) {slave.writeCommand(nexe);}


void TAPOFF(){arc.TapOff();}


