#define SERIAL_TX_BUFFER_SIZE 256
#define SERIAL_RX_BUFFER_SIZE 256

#include "AmbulanceC.h"
#include <EEPROM.h>
#include "Phone.h"
#include "MyString.h"
#include "TimerList.h"
#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include "Timeout.h"
#include "Tristar.h"
#include <tristarSlave.h>

modbusDevice regBank;
tristarSlave slave;
TimerList t;

 TimerList::MyTimer *timerReset=NULL, *timerMemory=NULL;
 const int CRITICAL_MEMORY=2000;
const unsigned long resetInterval=43200000; 
const word baud=9600;
const byte TRISTAR14= 0;
const byte TRISTAR12= 1;
const byte ARD= 2;

Phone phone;
AmbulanceC arc;

const bool prnt=true;
const bool dbg=true;
const bool prin=false;
const bool prout=false;
const bool ISPHONE=true;
const bool ISMONITOR=true;



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

arc.initializePins();

attachInterrupt(0, PIRTAMPINTERRUPT, CHANGE); 
attachInterrupt(1, PIRALINTERRUPT,   CHANGE); 
attachInterrupt(3, DOORSWINTERRUPT,  CHANGE); 
attachInterrupt(4, PERALINTERRUPT,   CHANGE); 
attachInterrupt(5, TOTALINTERRUPT,   CHANGE); 

int i;
regBank.setId(1);
for(i = 0 ; i < 33 ; i++) {regBank.add(i+30001);  regBank.set(i+30001,0);}
for(i = 0 ; i < 33 ; i++) {regBank.add(i+40001);  regBank.set(i+40001,0);}
slave._device = &regBank; 

Serial.begin(baud);
Serial2.begin(baud);
Serial3.begin(baud);

delay(1000);

slave.setBaud(baud); 
phone.setBaud(baud); 

RESETALL();

}


///////loop////////////////////////////////////////////////////////////////////////////////
void loop()
{

t.run();

	arc.run();

	if(phone.enabled()) phone.run();

	if(slave.enabled())  slave.run();


	

} //loop

void SILENCE(){

	if(dbg) {Serial.println("SILENCE!");}

	arc.lastItrptLevel=arc.ItrptLevel;
	arc.lastAlLevel=arc.AlLevel;

	arc.ItrptLevel=_NONE;

	phone.disable();
	slave.disable();

}
void MINIMIZE(){

	if(dbg) {Serial.println("MINIMIZE!");}

	arc.lastItrptLevel=arc.ItrptLevel;
	arc.lastAlLevel=arc.AlLevel;

	arc.ItrptLevel=_LIMITED;

	phone.disable();
	slave.disable();
}
void RESTORE(){

	if(dbg) {Serial.println("RESTORE!");}

	arc.setItrptLevel(arc.lastItrptLevel);
	arc.setAlLevel(arc.lastAlLevel);

	arc.manageAlarm();

	phone.enable();
	slave.enable();
}

///////reset////////////////////////////////////////////////////////////////////////////////

void RESETALL(){
int i;

arc.StatusLEDON();

	if(prnt){
Serial.println();
Serial.println("*********************************");
Serial.println("******RESETTING******************");
Serial.println("*********************************");
	}


if(prnt) Serial.println("******TIMERS*********************");
t.reset();
timerReset=t.setInterval(resetInterval, RESETALL);
timerMemory=t.setInterval(1000,checkmem);

if(prnt) Serial.println("******PHONE**********************");
phone.reset(ISPHONE, ISMONITOR);

if(prnt) Serial.println("******BOARD**********************");
arc.reset();

if(prnt) Serial.println("******SLAVE**********************");
slave.reset();

if(prnt){
Serial.println("*********************************");
Serial.println("******FINISHED******************");
Serial.println("*********************************");
Serial.println();
	}

if(prnt) {Serial.print("Free memory:"); 	Serial.println(freeRam ());}

SUCCESS();
}

///////memory////////////////////////////////////////////////////////////////////////////////

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void checkmem(){
	if(prnt) {
		int fram=freeRam ();
		if(fram<CRITICAL_MEMORY) {
			timerMemory->disable();
			
			RESETALL(); 
			
			return;}
		else
		{
			//if(dbg) {Serial.print("Free memory:"); 	Serial.println(fram);}
		}
		return;
	}
}



///////isr////////////////////////////////////////////////////////////////////////////////

ISR(PCINT0_vect) {arc.itrptSrc=99;}


byte READEEPROM(int index){return EEPROM.read(index);}

void REPORTMEMORY(byte id){slave.writeMemorytoString(id,&phone.SMS);
if(prnt) slave.printMemory(id);
}
void REPORTALARMS(byte id){slave.writeAlarmstoString(id,&phone.SMS);
if(prnt) slave.printMemory(id);
}




void BLINKSCENE(){arc.blinkScene();}


void BLINKALLED(){arc.blinkAlLED();}
void BLINKALLED2(){arc.blinkAlLED2();}
void ALLEDL(){arc.AlLEDL();}
void ALLEDH(){arc.AlLEDH();}



void BLINKSTATUSLED(){arc.blinkStatusLED();}
void STATUSLEDOFF(){arc.StatusLEDOFF();}
void ENABLEBLINKCONV(){	
arc.enableblinkStatusLED();}
void DISABLEBLINKCONV(){
	arc.disableblinkStatusLED();
	SUCCESS();
}


void PLAYSRN(){arc.playSrn();}
void SRNON(){arc.srnOn();}
void SRNONL(){arc.srnOnL();}
void SRNOFF(){arc.srnOff();
}

void RESETSRN(){arc.resetSrn();}
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
	if(isListening)arc.StatusLEDON();
	else arc.StatusLEDOFF();
	}
void SUCCESS(){

	arc.StatusLEDOFF();
	delay(500);
	arc.StatusLEDON();
	delay(200);
	arc.StatusLEDOFF();
	delay(200);
	arc.StatusLEDON();
	delay(200);
	arc.StatusLEDOFF();
	if(dbg) Serial.println("SUCCESS!");

}





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

void TRISTARSERIALTIMEOUT(){slave.serialtimeout();}
void TRISTARCONVTIMEOUT(){slave.convtimeout();}


void TAPOFF(){arc.TapOff();}




void SERIALTIMEOUT(){phone.serialtimeout();}
void CONVTIMEOUT(){phone.convtimeout();}
void CHECKPHONE(){phone.startPhone();}
void CHECKMONITOR(){phone.startMonitor();}
void RESETSLAVE(){slave.reset();}