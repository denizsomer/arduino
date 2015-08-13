
#include "TimerList.h"
TimerList::MyTimer *timer1, *timer2;

const bool prnt=true;


TimerList t;
void setup() {     


Serial.begin(9600);


t.reset();
timer1=t.setInterval(1000,ding);
timer2=t.setInterval(3000,dong);


}

void loop() {
t.run();

}

void ding() {
Serial.println("ding");

}

void dong() {
	Serial.println("dong");
	//t.setTimeout(300000,dong);
timer1->deleteTimer();
	
}
