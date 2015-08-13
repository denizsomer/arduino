#include "Timer.h"

Timer t;
int pin = 13,lastTime, elapsed;



void setup()
{
  pinMode(pin, OUTPUT);
  //t.pulse(pin, 500, HIGH); // 10 seconds
  // t.pulse(pin, 10 * 60 * 1000, HIGH); // 10 minutes  
}

void loop()
{
  
  elapsed=millis()-lastTime;
  if(elapsed>5000) doAfter(); 
}

void doAfter(){
  lastTime=millis();
digitalWrite(pin, HIGH); 
delay(500);
digitalWrite(pin,LOW);
}


