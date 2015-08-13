#include "MyPrint.h"
#include "Arduino.h"

MyPrint::MyPrint()
{
  phone=false;
}

void MyPrint::dprint(const char str[])
{
  Serial.print(str);
}

/*void MyPrint::dprint(char c)
{
//	Serial.print("int3");
//Serial.print(c);
}

void MyPrint::dprint(unsigned char b)
{
//	Serial.print("int3");
//  Serial.print(b);
}

void MyPrint::dprint(int n)
{
	//if(phone) 
//		Serial.print("int");
  //else Serial.print("int2");
}

void MyPrint::dprint(unsigned int n)
{
	//Serial.print("int3");
 // Serial.print(n);
}

void MyPrint::dprint(long n)
{
//		Serial.print("lng1");
 // Serial.print(n);
}

void MyPrint::dprint(unsigned long n)
{
//	Serial.print("long2");
 // Serial.print(n);
}

void MyPrint::dprint(double n)
{
	//	Serial.print("dou");
 // Serial.print(n);
}
*/