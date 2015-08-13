

#ifndef incl_AmbulanceC_h
#define incl_AmbulanceC_h

#include "../MyString/MyString.h"
#include "../Phone/Phone.h"
#include "../TimerList/TimerList.h"

extern Phone phone;
extern TimerList t;
extern const bool prnt,dbg;




#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

const byte _ALL   = 2;
const byte _LIMITED   = 1; 
const byte _NONE   = 0; 



class AmbulanceC
{
  public:

    AmbulanceC(void);

    ~AmbulanceC(void);


static const bool isPhone=true;
static const bool isMonitor=true;

static const byte debounceDelay=10;

static const byte i_ARPTx   = 0; //Rx0 
static const byte i_ARPRx   = 1; //Tx0


static const byte i_pirTamp = 2; //interrupt 0
static const byte i_pirAl   = 3; //interrupt 1 

static const byte o_SrnOff  = 9;
static const byte o_SrnOn   = 8;
static const byte o_LHScene    = 10; 
static const byte o_RHScene    = 11; 

static const byte i_phoneRx     = 14; //TX3
static const byte i_phoneTx     = 15; //RX3

static const byte i_monitorRx   = 16; //TX2
static const byte i_monitorTx   = 17; //RX2

//interrupts
static const byte i_TotAlSw  = 18; //  interrupt5
static const byte i_PerAlSw  = 19; //  interrupt4
static const byte i_DoorSw   = 20; //  interrupt3
//21 DOES NOT WORK

static const  byte o_AlPow  = 22; 
static const  byte o_Sink   = 24;
//static const  byte o_sp   = 23;
//static const  byte o_sp   = 25;


static const byte o_LED1       = 33;
static const byte o_LED3       = 35;
static const byte o_StatusLED  = 37;
static const byte o_AlLED      = 45;
static const byte o_LED5       = 47;

static const byte i_IsoSw   = 51; 
static const byte i_EmSw    = 50;
static const byte i_HbrkSw  = 53; 
static const byte i_TapSw   = 52; 

static const byte BUSY=0;
static const byte FREE=1;

void initializePins();


void run();
void arm(void);
void disarm(void);

void playSrn(void);
void playWrn(int);

void reset(void);
void initializeTimers(void);
void resetAl(void);
void resetScene(void);
void resetSrn(void);
void resetInterrupts(void);
void testInterrupts(void);

void reportOutputs(MyString *);
void reportAl(MyString *);
void reportState(MyString *);

void manageInterrupt(int);
void manageAlarm(void);
void setAlLevel(byte);
void setItrptLevel(byte);

void blinkAlLED(void);
void blinkAlLED2(void);
void enableblinkAlLEDSlow(void);
void enableblinkAlLEDFast(void);
void disableblinkAlLED(void);
void AlLEDL(void);
void AlLEDH(void);

void blinkScene(void);
void enableblinkScene(void);
void disableblinkScene(void);
void RHSceneOn(void);
void LHSceneOn(void);

void blinkStatusLED(void);
void enableblinkStatusLED(void);
void disableblinkStatusLED(void);
void StatusLEDON(void);
void StatusLEDOFF(void);


void busysrnStatus(void);
void srnOn(void);
void srnOnL(void);
void srnOff(void);

void pirTampInterrupt(void);
void pirAlInterrupt(void);
void TotAlInterrupt(void);
void PerAlInterrupt(void);
void DoorSwInterrupt(void);

void TapOff();
void TapOn();
bool debounce(int);

void EmergencyON(void);

bool getPin(int pin){ return (digitalRead(pin));}
void setPin(int pin,bool state){ digitalWrite(pin, state); }

 byte AlLevel, lastAlLevel,srnStatus, nAls,AlSrc,ItrptLevel,lastItrptLevel ;


 bool AlArmed;

 static const byte maxnAls=10, AlPowDelay=100, interruptDelay=200;

 static const unsigned int SrnDuration=5000,  durationAledLED=2000,
 alarmArmDelay=7000, doorAlDelay=7000;

 static const bool leftScene=0, rightScene=1;

  bool s_TapSw, s_IsoSw, s_EmSw, s_HbrkSw, scene;

  volatile int itrptSrc;
  
  private:

 TimerList::MyTimer *timerblinkAlLEDSlow, *timerblinkAlLEDMedium, *timerblinkAlLEDFast,
	*timerblinkStatusLED,  *timerblinkScene,  *tapTimeout, *timercheckStatus, *alarmTimeout,
	*srnTimeout1, *srnTimeout2, *doorTimeout;



};




#endif

extern void TAPOFF();
extern void CHECKPHONE();
extern void CHECKMONITOR();
extern void RESETALL();
extern void BLINKSCENE();
extern void BLINKALLED();
extern void BLINKALLED2();
extern void BLINKSTATUSLED();
extern void STATUSLEDOFF();
extern void SUCCESS();

extern void ALLEDL();
extern void ALLEDH();
extern void PLAYSRN();
extern void SRNON();
extern void SRNONL();
extern void SRNOFF();
extern void RESETSRN();
extern void MANAGEALARM();
extern void PIRTAMPINTERRUPT();
extern void PIRALINTERRUPT();
extern void TOTALINTERRUPT();
extern void PERALINTERRUPT();
extern void DOORSWINTERRUPT();

extern void SILENCE();
extern void MINIMIZE();
extern void RESTORE();

