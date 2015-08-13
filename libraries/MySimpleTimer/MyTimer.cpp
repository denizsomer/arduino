

#include "MyTimer.h"

//#if defined(ARDUINO) && ARDUINO >= 100
//#include <Arduino.h>
//#else
//#include <WProgram.h>
//#endif



MyTimer::MyTimer(int ID) {

        callback = 0;
        enabled = false;
        delay = 0;
        numRuns = 0;
		maxNumRuns = 0;
		prev_millis=0;
		id=ID;

return;

}

MyTimer::MyTimer(long d, timer_callback f, int nRUN, int ID) {

	callback = f;
	enabled = true;
    delay = d;
	numRuns = 0;
    maxNumRuns = nRUN;
    prev_millis = millis(); 
	id=ID;

	return;

}

MyTimer::~MyTimer()
{
	
  return;
}




bool MyTimer::operator==(MyTimer &timer2)
{
	
  if (id==timer2.id) return true;

  return false;
}



bool MyTimer::operator!=(MyTimer &timer2)
{
  
  if (id==timer2.id) return false;

  return true;
}


