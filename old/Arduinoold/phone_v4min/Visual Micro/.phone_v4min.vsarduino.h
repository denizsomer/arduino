#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
//Board = Arduino Mega 2560 or Mega ADK
#define __AVR_ATmega2560__
#define 
#define ARDUINO 105
#define ARDUINO_MAIN
#define __AVR__
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

void resetVariables();
void resetIgn();
//
//
void resetIntrpt();
void AlSwInterrupt();
void HbrkSwInterrupt();
void IsoSwInterrupt();
void IgnInterrupt();
void pirAlInterrupt();
void pirTampInterrupt();
void disableTimers();
void enableTimers();
void checkStatus();
void prepareStorage();
void resetAl();
void lightLED(int pin, int times, long millIsoSwn, long millIsoSwff);
void lightLED(int pin, int times);
void playSrn();
void initializeSrn();
void playWrn(int WrnDuration);
void srnOnHigh();
void srnOnLow();
void srnOffHigh();
void srnOffLow();
void writeVoltages();
int readVoltages();
void readwriteVoltages();
void printDouble( double val, double precision);
void resetSol();
void toggleSol();
void chargeSolL();
void chargeSolV();
void shutSol();
void resetPhone();
void resetSMS();
void resetSerial();
void resetphnData();
int charToInt(char c);
char intToChar(int c);
int decodeCommand(char* cmd);
void execCommand(int num);
void freephnStatus();
void busyphnStatus();
void answerCall();
void sendSMS();
void listSMS();
void deleteSMS();
void readSMS();
void takeandsendPicture();
void phoneInitialize();
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
void sendReport(int nReport);
void switchReport(int nReport);
void appendtoSMS(char *partSMS, int len);
void reportReports();
void reportDigital();
void reportPins();
void reportVoltages();
void reportDouble( double val, int nfrac);
void reportCommands();
void reportAl();
void reportVariables();

#include "C:\Program Files (x86)\Arduino\hardware\arduino\variants\mega\pins_arduino.h" 
#include "C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "c:\Users\deniz\Documents\Arduinoold\phone_v4min\phone_v4min.ino"
#endif
