
#include "MyTimer.h"

#ifndef MYTIMERLIST_H
#define MYTIMERLIST_H

//#if defined(ARDUINO) && ARDUINO >= 100
//#include <Arduino.h>
//#else
//#include <WProgram.h>
//#endif



typedef void (*timer_callback)(void);

class MyTimerList {

public:

    // setTimer() constants
    const static int RUN_FOREVER = 0;
    const static int RUN_ONCE = 1;

    // constructor
    MyTimerList(void);

	// destructor
    ~MyTimerList();



    // this function must be called inside loop()
    void run();

    // call function f every d milliseconds
    MyTimer *  setInterval(long d, timer_callback f);

    // call function f once after d milliseconds
    MyTimer *  setTimeout(long d, timer_callback f);

	    // call function f every d milliseconds for n times
    MyTimer *  setTimer(long d, timer_callback f, int n);

    // returns the number of used timers
	int getNumTimers(){return n;}

	    // returns the number of used timers
//	unsigned int getidTimers(){return id;}

	// destroy the specified timer
    void deleteTimer(MyTimer *);

	void free();

private:



	MyTimer **timers, **tmp_timers;



	 // number of timers
   int n;
   unsigned int id;

    // deferred call constants
    const static int DEFCALL_DONTRUN = 0;       // don't call the callback function
    const static int DEFCALL_RUNONLY = 1;       // call the callback function but don't delete the timer
    const static int DEFCALL_RUNANDDEL = 2;      // call the callback function and delete the timer

	void createList();
	void copyListtoTemp();
	void copyTemptoList();



};

#endif

extern void setTim(MyTimerList*);