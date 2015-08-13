
#include "TimerList.h"

#ifndef incl_AmbulanceP_h
#define incl_AmbulanceP_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class AmbulanceP
{
  public:

    AmbulanceP(void);

    ~AmbulanceP(void);

	void initializePins();
	bool getPin(int pin){ return (digitalRead(pin));}
void setPin(int pin,bool state){ digitalWrite(pin, state); }


	//voltages
static const byte i_ShuntM     = A0; 
static const byte i_ShuntP     = A1; 
static const byte i_BattL1    = A7; 
static const byte i_BattL2    = A6; 
static const byte i_BattV     = A5; 

static const byte  i_ARCTx    = 0; //Rx0 
static const byte  i_ARCRx    = 1; //Tx0
static const byte  i_BatL1Sw  = 2; //  interrupt0
static const byte  i_BatL2Sw  = 3; //  interrupt1

static const byte  o_SP1    = 4;
static const byte  o_SP2    = 5;
static const byte  o_SW80    = 6;
static const byte  o_SW180   = 7;

static const byte  o_ContLED     = 8;
static const byte  o_BatteryLED  = 9;
static const byte  o_StatusLED   = 9; 

static const byte  i_IgnSw   = 19; //  interrupt4 
static const byte  i_ContWatch  = 20; //  interrupt3
static const byte  i_SolSw   = 21; //  interrupt2

static const byte o_chipSelect = 23;

void AllSolarto1();	
void AllSolarto2();	
void BatL1Interrupt();
void BatL2Interrupt();
void BatteryLEDH();	
void BatteryLEDL();	
void blinkBatteryLED();	
void blinkBatteryLED2();	
void blinkContLED();	
void blinkContLED2();	
void ContLEDH();	
void ContLEDL();	
void ContWatchInterrupt();	
bool debounce(int);


void disableblinkBatteryLED();	
void disableblinkContLED();	
void enableblinkBatteryLEDSlow();	
void enableblinkBatteryLEDSlow2();	
void enableblinkContLEDFast();	
void enableblinkContLEDFast2();	
void enableblinkContLEDSlow();	
void enableblinkContLEDSlow2();	
void IgnInterrupt();

void blinkStatusLED(void);
void enableblinkStatusLED(void);
void disableblinkStatusLED(void);
void StatusLEDON(void);
void StatusLEDOFF(void);
	
void run();
void manageInterrupt(int);	
void manageLEDS();	
void readwriteVoltages();	
void reset();	
void resetInterrupts();	
void resetSolar();	
void resetTimers(void);
void SolSwInterrupt();	
void SW180Off();	
void SW180On();	
void SW80Off();	
void SW80On();	
//void writeVoltages();	
//int readVoltages();
void printFloat( float , int);

volatile int itrptSrc;


  
  private:

	  bool enable;
	  //voltage multipliers     
//static const float   f0=5.0/1023., f1=0.01506, f3=0.01527,
// f2=0.01580,f4=0.01445, v_ShuntMin=4.0, v_CRI=13.5, v_MAX=14.5, v_MIN=11.5;

//timers,locations in timer array
 TimerList::MyTimer  *SW180Timeout, *SW80Timeout,  
 *timerblinkContLEDSlow, *timerblinkContLEDSlow2,
*timerblinkContLEDFast, *timerblinkContLEDFast2,*timerblinkStatusLED,
*timerblinkBatteryLEDSlow, *timerblinkBatteryLEDSlow2;

byte whichBattIsCharged;

static const unsigned int SolDelay=5000, readVoltageInterval=5000;

static const byte debounceDelay=10, interruptDelay=200;

static const unsigned long IgnDelay=120000, IgnDelay2NDBAT=300000,resetInterval=43200000;

//float v_ShuntP, v_ShuntM , v_BattL1, v_BattL2 , v_BattV , v_SolCont;

};




#endif

extern const bool prnt, dbg;

extern void SW180ON();
extern void SW80ON();
extern void CONTLEDL();
extern void BATTERYLEDL();
extern void CONTLEDH();
extern void BATTERYLEDH();
extern void BLINKBATTERYLED2();
extern void BLINKBATTERYLED();
extern void BLINKCONTLED2();
extern void BLINKCONTLED();

extern void READWRITEVOLTAGES();


extern void RESETALL();
extern void SRNOFFL();

extern void BATL1INTERRUPT();
extern void BATL2INTERRUPT();
extern void IGNINTERRUPT();
extern void CONTWATCHINTERRUPT();
extern void SOLSWINTERRUPT();

extern void BLINKSTATUSLED();
extern void STATUSLEDOFF();
extern void SUCCESS();



