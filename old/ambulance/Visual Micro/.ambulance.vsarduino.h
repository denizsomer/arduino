#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
//Board = Arduino Mega 2560 or Mega ADK
#define __AVR_ATmega2560__
#define ARDUINO 101
#define ARDUINO_MAIN
#define __AVR__
#define __avr__
#define F_CPU 16000000L
#define __cplusplus
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__

#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
            
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

//
void initializePins();
void resetAll();
void resetTimers();
void resetIgn();
void resetAl();
void resetCont();
void resetSolar();
void resetPhn();
void resetSMS();
void resetSerial();
void resetPDU();
void resetInterrupts();
//
void manageInterrupts(int iSrc);
int whichPinInterrupted();
boolean debounce(int pin);
void pirTampInterrupt();
void pirAlInterrupt();
void DoorSwInterrupt();
void HbrkSwInterrupt();
void IgnInterrupt();
void IsoSwInterrupt();
void attachAlInterrupts();
void detachAlInterrupts();
void attachnonAlInterrupts();
void detachnonAlInterrupts();
void attachAllInterrupts();
void detachAllInterrupts();
void arm();
void disarm();
void enableblinkAlLEDFast();
void enableblinkAlLEDSlow();
void disableblinkAlLED();
void playSrn();
void playWrn(int WrnDuration);
void freesrnStatus();
void srnOn();
void srnOnH();
void srnOff();
void srnOffH();
void blinkAlLED();
void AlLEDL();
void AlLEDH();
void ContactorOn();
void ContactorOff();
void blinkContLED();
void ContLEDL();
void enableblinkContLEDFast();
void enableblinkContLEDSlow();
void disableblinkContLED();
void writeVoltages();
int readVoltages();
void readwriteVoltages();
void printDouble( double val, double precision);
void SolarOff();
void SolarOn();
void chargeSolL();
void chargeSolV();
void disablePhoneTimers();
void enablePhoneTimers();
void checkStatus();
void prepPhone();
void disableMonitorTimer();
void enableMonitorTimer();
void prepMonitor();
void manageMonitor();
void omanageMonitor();
void initializeMonitor();
void listMonitor();
void clearMonitor();
void monitorSMS();
void freemonitorStatus();
void busymonitorStatus();
void managePhone();
int decodeCommand(char* cmd);
void execCommand();
void freephnStatus();
void busyphnStatus();
void answerCall();
void sendSMS();
void listPhone();
void clearPhone();
void readSMS();
void takePicture();
void takeandsendPicture();
void initializePhone();
void phonegoHome();
void phoneWake();
void phoneHello();
void phoneKeyboardMode();
void phoneWriteNewMessage();
void phoneStartNewMessage();
void phoneCameraMode();
void phoneFocus();
void phoneCapture();
void phoneUpArrow();
void phoneDownArrow();
void phoneYes();
void phoneReturn();
void phoneLeftFunction();
void phoneEnterMyNumber();
void phonepurgeSMS();
int charToInt(char c);
char intToChar(int c);
void startSMS();
void writeReport(int nReport);
void appendtoSMS(const char *partSMS, int len);
void listOther();
void listSol();
void listAl();
void listPhn();
void listReports();
void reportAl();
void reportVoltages();
void reportTimerSince();
void reportTimerTogo();
void reportState();
void reportOutputs();
void reportDouble( double val, int nfrac);
void reportLong( long val);
void checkTap();

#include "C:\Programs\arduino-1.0.5-r2\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Programs\arduino-1.0.5-r2\hardware\arduino\variants\mega\pins_arduino.h" 
#include "q:\Documents\Arduino\ambulance\ambulance.ino"
#endif
