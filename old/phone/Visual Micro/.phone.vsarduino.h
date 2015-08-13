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

//
void resetVariables();
void resetPhn();
void resetSMS();
void resetSerial();
void resetPDU();
void phoneInitialize();
void prepareStorage();
void freephnStatus();
void busyphnStatus();
void phoneWake();
void phoneHello();
void phoneKeyboardMode();
int charToInt(char c);
void sendSMS();
void listSMS();
void deleteSMS();
void readSMS();
void phonepurgeSMS();
void phoneYes();
//

#include "C:\Program Files (x86)\Arduino\hardware\arduino\variants\mega\pins_arduino.h" 
#include "C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "c:\Users\deniz\Documents\Arduino\phone\phone.ino"
#endif
