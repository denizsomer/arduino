#define SERIAL_TX_BUFFER_SIZE 256
#define SERIAL_RX_BUFFER_SIZE 256

#include "AmbulanceP.h"
#include "MyString.h"
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>
#include "TimerList.h"
#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include "Timeout.h"
#include "Tristar.h"
#include <tristarMaster.h>

modbusDevice regBank;
tristarMaster master;
TimerList t;
AmbulanceP arp;
//
 TimerList::MyTimer *timerReset, *timerMemory=NULL ;
 const int CRITICAL_MEMORY=1000;
const unsigned long resetInterval=43200000; //3600000; //every hour
const word baud=9600;
const byte TRISTAR14= 0;
const byte TRISTAR12= 1;
const byte ARD= 2;

const bool prnt=false;
const bool dbg=false;
const bool prin=false;
const bool prout=false;


bool SDInitialized=false;

void setup()
{
	int i;
//new interrupt
//MCUCR = (1<<ISC00) | (1<<ISC01);
//PCICR |= (1<<PCIE0);
//PCMSK0 |= (1<<PCINT0); // 53
//PCMSK0 |= (1<<PCINT1); // 52
//PCMSK0 |= (1<<PCINT2); // 51
//PCMSK0 |= (1<<PCINT3); // 50

arp.initializePins();

attachInterrupt(0, BATL1INTERRUPT,      CHANGE); 
attachInterrupt(1, BATL2INTERRUPT,      CHANGE); 
attachInterrupt(4, IGNINTERRUPT,        CHANGE); 
attachInterrupt(3, CONTWATCHINTERRUPT,  CHANGE);
attachInterrupt(2, SOLSWINTERRUPT,      CHANGE); 

//serials begin here:
Serial.begin(baud);
Serial2.begin(baud);
Serial3.begin(baud);
master.setBaud(baud); 

regBank.setId(1);
for(i = 0 ; i < 33 ; i++) {regBank.add(i+30001);  regBank.set(i+30001,0);}
for(i = 0 ; i < 33 ; i++) {regBank.add(i+40001);  regBank.set(i+40001,0);}

master._device = &regBank;  



RESETALL();

}


void loop()
{
	//Serial.println("**");
t.run();
arp.run();
//master.run();
} 



///////reset ////////////////////////////////////////////////////////////////////////////////

void RESETALL(){

	arp.StatusLEDON();

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
if(prnt) Serial.println("******SD**********************");
resetSD();

if(prnt) Serial.println("******BOARD**********************");
arp.reset();

if(prnt) Serial.println("******MASTER**********************");
master.reset();


	if(prnt){
Serial.println("*********************************");
Serial.println("******FINISHED******************");
Serial.println("*********************************");
Serial.println();}

	SUCCESS();

	if(prnt) {Serial.print("Free memory:"); 	Serial.println(freeRam ());}


LOGTRISTAR();
}

///////loop////////////////////////////////////////////////////////////////////////////////

//ISR(PCINT0_vect) {arp.itrptSrc=99;}

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
		else{
			//if(dbg) {Serial.print("Free memory:"); 	Serial.println(fram);}
		}
		return;
	}
}
void resetSD()
{

 if(prnt)   Serial.print("Initializing SD card...");
  
  // see if the card is present and can be initialized:
  if (!SD.begin(arp.o_chipSelect)) {
    if(prnt)  Serial.println("Card failed, or not present");
    // don't do anything more:
    
    return;
  }
  if(prnt)  Serial.println("card initialized.");
  SDInitialized=true;
}



///////externs ////////////////////////////////////////////////////////////////////////////////
void WRITEMEMORYTOSD(byte id)
{

	if(prnt) master.printMemory(id);

	if(!SDInitialized){
		resetSD();
		if(prnt) Serial.println("SD card not initialized.");
		return;
	}

	MyString datastr;

	const char Source[3][11]={"ondort.txt","onikii.txt","arduni.txt"};

	if(id==TRISTAR14) SD.open("ondort.txt", FILE_WRITE);
	else if(id==TRISTAR12) SD.open("onikii.txt", FILE_WRITE);
	else if(id==ARD) SD.open("arduni.txt", FILE_WRITE);
	else return;


	File  dataFile = SD.open(Source[id], FILE_WRITE);
	if (!dataFile) {if(prnt) Serial.println("error opening file"); 
	return;}


	if(prnt) Serial.println("started writing to sd");
			bool alarms= master.writeMemorytoString(id,&datastr);
dataFile.println();
			dataFile.print(datastr.asCharArray()); datastr.free();

			if(alarms){
			dataFile.print(" ");

			master.writeAlarmstoString(id,&datastr);

			dataFile.print(datastr.asCharArray()); datastr.free();
			dataFile.println();
			}

			dataFile.close();

			if(prnt) Serial.println("finished writing to sd");
		 
		
  
  return;
}


byte READEEPROM(int index){return EEPROM.read(index);}


void SW180ON(){arp.SW180On();}
void SW80ON(){arp.SW80On();}
void CONTLEDL(){arp.ContLEDL();}
void BATTERYLEDL(){arp.BatteryLEDL();}
void CONTLEDH(){arp.ContLEDH();}
void BATTERYLEDH(){arp.BatteryLEDH();}
void BLINKBATTERYLED2(){arp.blinkBatteryLED2();}
void BLINKBATTERYLED(){arp.blinkBatteryLED();}
void BLINKCONTLED2(){arp.blinkContLED2();}
void BLINKCONTLED(){arp.blinkContLED();}
void READWRITEVOLTAGES(){arp.readwriteVoltages();}
void BATL1INTERRUPT(){arp.BatL1Interrupt();}
void BATL2INTERRUPT(){arp.BatL2Interrupt();}
void IGNINTERRUPT(){arp.IgnInterrupt();}
void CONTWATCHINTERRUPT(){arp.ContWatchInterrupt();}
void SOLSWINTERRUPT(){arp.SolSwInterrupt();}

void LOGTRISTAR(){
	master.logTristar(TRISTAR14);
		t.setTimeout(15000, LOGTRISTAR12);

}

void LOGTRISTAR12(){
	master.logTristar(TRISTAR12);
}

void SERIALTIMEOUT(){master.serialtimeout();}
void CONVTIMEOUT(){	master.convtimeout();}




void SUCCESS(){
	arp.StatusLEDOFF();
	delay(500);
	arp.StatusLEDON();
	delay(200);
	arp.StatusLEDOFF();
	delay(200);
	arp.StatusLEDON();
	delay(200);
	arp.StatusLEDOFF();
	if(dbg) Serial.println("SUCCESS!");

}

void BLINKSTATUSLED(){arp.blinkStatusLED();}
void STATUSLEDOFF(){arp.StatusLEDOFF();}
void ENABLEBLINKCONV(){
	SUCCESS();
	arp.enableblinkStatusLED();}
void DISABLEBLINKCONV(){
	arp.disableblinkStatusLED();
SUCCESS();}

void ALLSOLARTO1(){arp.AllSolarto1();}
void ALLSOLARTO2(){arp.AllSolarto2();}
void RESETSOLAR(){arp.resetSolar();}
