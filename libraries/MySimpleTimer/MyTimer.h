

#ifndef MYTIMER_H
#define MYTIMER_H

//#if defined(ARDUINO) && ARDUINO >= 100
//#include <Arduino.h>
//#else
//#include <WProgram.h>
//#endif
extern unsigned long millis();
extern void ding();


typedef void (*timer_callback)(void);

class MyTimer {

public:

    // constructor
    MyTimer(int);

	// call function f every d milliseconds for n times
    MyTimer(long, timer_callback, int, int);

	// destructor
    ~MyTimer();

	bool operator==(MyTimer &);
    bool operator!=(MyTimer &);

    // restart the specified timer
	void restartTimer(){prev_millis = millis();}

    // returns true if the specified timer is enabled
	bool isEnabled(){return enabled;}

    // enables the specified timer
    void enable(){enabled= true;}

    // disables the specified timer
	void disable(){enabled = false;}

    // enables the specified timer if it's currently disabled,
    // and vice-versa
	void toggle(){ enabled = !enabled;}


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

private:


	MyTimer *next_ptr;
	friend class LinkedListlinked_list;

	int id;

};

#endif

