

#if defined(ARDUINO) && ARDUINO >= 100
#else
typedef unsigned char byte;
typedef unsigned int word;
#define _CRT_SECURE_NO_DEPRECATE
#include <stddef.h>
#include <iostream>
#include "HardwareSerial.h"



#endif

#include "tristarSlave.h"
#include "tristarMaster.h"



const bool prnt=false;


int main(int argc, char **argv)
{ 

	tristarSlave slave;
	tristarMaster master;


	return 0;
}

int digitalRead(int i){return i;};
void digitalWrite(int, bool){};
long millis(){return 0;}
void delay(int){};



