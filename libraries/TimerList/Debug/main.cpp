#include "TimerList.h"
#include <ctime>
#include <iostream>
#include <crtdbg.h>


const static long ntimers=1000;

TimerList t;
TimerList::MyTimer** timer;
TimerList::MyTimer* thistimer;

int nding=0;

int main(int argc, char **argv)
{ 
//timer =new TimerList::MyTimer*[ntimers];
	int i;

//_CrtMemState s1,s2, s3, s4, s5;
//_CrtMemCheckpoint( &s1 ); 

	//for ( i = 0; i < ntimers; i++) timer[i]=t.setTimeout((i+1)*1,ding);

	 //for ( i = 0; i < ntimers; i++) t.deleteTimer(i);

//_CrtMemCheckpoint( &s2 ); 
//if ( _CrtMemDifference( &s3, &s1, &s2) ) _CrtMemDumpStatistics( &s3 );


//delete [] timer; timer=NULL;


	thistimer=t.setInterval(100,ding);
	t.setInterval(3000,dong);




while(1){

t.run();
}


	return 0;
}

void ding() {

std::cout<<"ding\n";

}

void dong() {
	std::cout<<"d0ng\n";

	//t.setTimeout(3000,dong);
thistimer->deleteTimer();
	
}




unsigned long millis(){return clock();}


