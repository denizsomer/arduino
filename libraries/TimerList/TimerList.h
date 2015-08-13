


#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <stddef.h>
#include <fstream>
#include <iostream>
extern unsigned long millis();
extern void ding();
extern void dong();
#endif

#ifndef incl_TimerList_h
#define incl_TimerList_h

extern const bool prnt, dbg;
extern void checkmem();
typedef void (*timer_callback)(void);

const static int MAX_NUM_TIMERS = 120; //due to Qrduino memory 3000=25*40
    const static int RUN_FOREVER = 0;
    const static int RUN_ONCE = 1;
    const static int DEFCALL_DONTRUN = 0;       // don't call the callback function
    const static int DEFCALL_RUNONLY = 1;       // call the callback function but don't delete the timer
    const static int DEFCALL_RUNANDDEL = 2;      // call the callback function and delete the timer
    const static int DEFCALL_DEL = 3;      // call the callback function and delete the timer

class TimerList {

public:

	class MyTimer {
	public:

		 MyTimer(){

        callback = 0;
        enabled = false;
        delay = 0;
        numRuns = 0;
		maxNumRuns = 0;
		prev_millis=millis();
		id=-1;
		toBeCalled=0;
		toBeDeleted=false;
		prev_ptr=NULL; next_ptr=NULL;

return;
}


 MyTimer(int ID){

        callback = 0;
        enabled = false;
        delay = 0;
        numRuns = 0;
		maxNumRuns = 0;
		prev_millis=millis();
		id=ID;
		toBeCalled=0;
		toBeDeleted=false;
		prev_ptr=NULL; next_ptr=NULL;

return;
}


  MyTimer(long d, timer_callback f, int nRUN, int ID, bool e) {
	callback = f;
	enabled = e;
    delay = d;
	numRuns = 0;
    maxNumRuns = nRUN;
    prev_millis = millis(); 
	id=ID;
	toBeCalled=0;
	toBeDeleted=false;
	prev_ptr=NULL; next_ptr=NULL;
	return;
}

 unsigned long since(){ 
	unsigned long current_millis = millis();
	return current_millis - prev_millis; }

     // restart the specified timer
	void restartTimer(){prev_millis = millis(); return;}

    // returns true if the specified timer is enabled
	bool isEnabled(){return enabled;}

    // enables the specified timer
    void enable(){enabled= true;}

	 // enables the specified timer
    void deleteTimer(){toBeDeleted= true;}

    // disables the specified timer
	void disable(){enabled = false;}

    // enables the specified timer if it's currently disabled, and vice-versa
	void toggle(){ enabled = !enabled;}

	int getid(){return id;}

	// pointers to the callback functions

    timer_callback callback;
	 // delay values
    unsigned long delay;

		// value returned by the millis() function
		// in the previous run() call
		unsigned long prev_millis;

		// number of runs to be executed for each timer
		int maxNumRuns;

		// which timers are enabled
		bool enabled;

		// number of executed runs for each timer
		int numRuns;

		int toBeCalled;

		bool toBeDeleted;
	
	private:
	int id;
	// Pointer to next element
	MyTimer *prev_ptr, *next_ptr;
	friend class TimerList;
};

	MyTimer *last_ptr, *first_ptr;

TimerList(void) { 	ID=0; 
last_ptr = NULL; 
first_ptr = NULL; 
}
// ... Other member functions


public:

	
    // this function must be called inside loop()
	void run();

	void reset();

	int getnumTimers();

    // call function f every d milliseconds
    MyTimer *  setInterval(long, timer_callback f);

    // call function f once after d milliseconds
    MyTimer *  setTimeout(long, timer_callback f);

	// call function f every d milliseconds
    MyTimer *  setInterval(long, timer_callback f, bool);

    // call function f once after d milliseconds
    MyTimer *  setTimeout(long, timer_callback f, bool);



//void searchanddeleteTimer(int);
//void searchanddeleteTimer( MyTimer *);



private:
unsigned int ID;

void deleteTimer(int);
void deleteTimer( MyTimer *);
void deletetimer( MyTimer *);

	    // call function f every d milliseconds for n times
    MyTimer *  setTimer(long, timer_callback f, int, bool);
};

#endif