/*
 
http://www.bajdi.com
 
  SRF06 module detecting distance in cm. The closer we get to the object
  the more leds start to burn.
  The circuit:
 * leds on pin 2,3,4,5
 * SRF06 module: trig to pin 12, echo to pin 13
*/
 
#include "Ultrasonic.h"
 
const int  TRIG = 9;
const int  ECHO = 10;
const int led1 = 2;
const int led2 = 3;
const int led3 = 4;
const int led4 = 13;
Ultrasonic SRF06(TRIG, ECHO);
 
void setup()
{
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  Serial.begin(9600); 
}
 
void loop()
{
  Serial.println(SRF06.Ranging(CM));
  if (SRF06.Ranging(CM) >= 100)
  {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
  }
  if (SRF06.Ranging(CM) >= 80 && SRF06.Ranging(CM) < 100)   {     digitalWrite(led1, HIGH);     digitalWrite(led2, LOW);     digitalWrite(led3, LOW);     digitalWrite(led4, LOW);   }   if (SRF06.Ranging(CM) >= 60 && SRF06.Ranging(CM) < 80)   {     digitalWrite(led1, HIGH);     digitalWrite(led2, HIGH);     digitalWrite(led3, LOW);     digitalWrite(led4, LOW);   }   if (SRF06.Ranging(CM) >= 40 && SRF06.Ranging(CM) < 60)
  {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, LOW);
  }
  if (SRF06.Ranging(CM) < 40)
  {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
  }
}
