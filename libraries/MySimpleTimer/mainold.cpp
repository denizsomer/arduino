#include "MyTimerList.h"
#include "MyTimer.h"
#include <crtdbg.h>


const static long ntimers=1000;
MyTimer** timer;


int main(int argc, char **argv)
{ 
	MyTimerList timers;
	int i,j, nsize, *ptr;

_CrtMemState s1,s2, s3, s4, s5;
_CrtMemCheckpoint( &s1 ); 

	//timer=new MyTimer*[ntimers];

	for ( i = 0; i < ntimers; i++) setTim(&timers);
		//timers.setInterval(10, ding);


		//ptr = new int[ntimers];
		//delete [] ptr; ptr=NULL;

		//timer=new MyTimer*[ntimers];
//timer[i]=
		//new MyTimer(i*10, ding, 0, i);

//_CrtMemCheckpoint( &s2 );

//if ( _CrtMemDifference( &s3, &s1, &s2) ) _CrtMemDumpStatistics( &s3 );
	

	


_CrtMemCheckpoint( &s2 ); 

if ( _CrtMemDifference( &s3, &s1, &s2) ) _CrtMemDumpStatistics( &s3 );

// nsize=sizeof(timers);


//for ( i = 0; i < ntimers; i++)
//timers.deleteTimer(timer[i]);
//	delete timer[i];
/*
for ( i = 0; i < ntimers; i++)
timer[i]=timers.setInterval(i*10, ding);*/

//delete [] timer;


	return 0;
}




unsigned long millis(){return 1000;}

void ding() {
//Serial.println("ding");
	return;
}


void setTim(MyTimerList* _timers)
{ 
		_timers->setInterval(10, ding);

}