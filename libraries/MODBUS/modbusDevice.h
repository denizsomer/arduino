

#if defined(ARDUINO) && ARDUINO >= 100
#include <stdint.h>
#include <Arduino.h>
#else
#endif

#ifndef _MODBUSDEVICE
#define _MODBUSDEVICE

#include <modbusRegBank.h>
#include <modbus.h>
//#include <Wprogram.h>

class modbusDevice:public modbusRegBank
{
	public:
		modbusDevice(void);
		void setId(byte id);
		byte getId(void);
		
	private:
		byte _id;
};
#endif
