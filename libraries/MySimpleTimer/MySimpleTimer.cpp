/*
 * MySimpleTimer.cpp
 *
 * MySimpleTimer - A timer library for Arduino.
 * Author: mromani@ottotecnica.com
 * Copyright (c) 2010 OTTOTECNICA Italy
 *
 * This library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser
 * General Public License along with this library; if not,
 * write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include "MySimpleTimer.h"


// Select time function:
//static inline unsigned long elapsed() { return micros(); }
static inline unsigned long elapsed() { return millis(); }


MySimpleTimer::MySimpleTimer() {

	n=0;

deleteArrays();
deleteTempArrays();

return;

}

MySimpleTimer::~MySimpleTimer()
{
deleteArrays();
deleteTempArrays();
	
  return;
}
void MySimpleTimer::run() {
    int i;
    unsigned long current_millis;
	    // deferred function call (sort of) - N.B.: this array is only used in run()
    int *toBeCalled=new int[n];

    // get current time
    current_millis = millis();

    for (i = 0; i < n; i++) {

        toBeCalled[i] = DEFCALL_DONTRUN;

        // no callback == no timer, i.e. jump over empty slots
        if (callbacks[i]) {

            // is it time to process this timer ?
            // see http://arduino.cc/forum/index.php/topic,124048.msg932592.html#msg932592

            if (current_millis - prev_millis[i] >= delays[i]) {

                // update time
                //prev_millis[i] = current_millis;
                prev_millis[i] += delays[i];

                // check if the timer callback has to be executed
                if (enabled[i]) {

                    // "run forever" timers must always be executed
                    if (maxNumRuns[i] == RUN_FOREVER) {
                        toBeCalled[i] = DEFCALL_RUNONLY;
                    }
                    // other timers get executed the specified number of times
                    else if (numRuns[i] < maxNumRuns[i]) {
                        toBeCalled[i] = DEFCALL_RUNONLY;
                        numRuns[i]++;

                        // after the last run, delete the timer
                        if (numRuns[i] >= maxNumRuns[i]) {
                            toBeCalled[i] = DEFCALL_RUNANDDEL;
                        }
                    }
                }
            }
        }
    }

    for (i = 0; i < n; i++) {
        switch(toBeCalled[i]) {
            case DEFCALL_DONTRUN:
                break;

            case DEFCALL_RUNONLY:
                (*callbacks[i])();
                break;

            case DEFCALL_RUNANDDEL:
                (*callbacks[i])();
                deleteTimer(i);
                break;
        }
    }

	if(toBeCalled!=NULL) delete [] toBeCalled; toBeCalled=NULL;
	return;
}


// find the first available slot
// return -1 if none found
int MySimpleTimer::setTimer(long d, timer_callback f, int nRUN) {

	if (f == NULL) {
        return -1;
    }

if(n>0) copyArraystoTemp();
n++;
createArrays();
if(n>1) copyTemptoArrays();

	callbacks[n-1] = f;
	enabled[n-1] = true;
    delays[n-1] = d;
    maxNumRuns[n-1] = nRUN;
    prev_millis[n-1] = elapsed();

    return n;
}


int MySimpleTimer::setInterval(long d, timer_callback f) {
    return setTimer(d, f, RUN_FOREVER);
}


int MySimpleTimer::setTimeout(long d, timer_callback f) {
    return setTimer(d, f, RUN_ONCE);
}


void MySimpleTimer::deleteTimer(int timerId) {
    if (timerId >= n) {
        return;
    }

    // nothing to delete if no timers are in use
    if (n == 0) {
        return;
    }

    // don't decrease the number of timers if the
    // specified slot is already empty
    if (callbacks[timerId] != NULL) {
        callbacks[timerId] = 0;
        enabled[timerId] = false;
        delays[timerId] = 0;
        numRuns[timerId] = 0;

        // update number of timers
        n--;
    }
}


// function contributed by code@rowansimms.com
void MySimpleTimer::restartTimer(int numTimer) {
    if (numTimer >= n) {
        return;
    }

    prev_millis[numTimer] = elapsed();
}


bool MySimpleTimer::isEnabled(int numTimer) {
    if (numTimer >= n) {
        return false;
    }

    return enabled[numTimer];
}


void MySimpleTimer::enable(int numTimer) {
    if (numTimer >= n) {
        return;
    }

    enabled[numTimer] = true;
}


void MySimpleTimer::disable(int numTimer) {
    if (numTimer >= n) {
        return;
    }

    enabled[numTimer] = false;
}


void MySimpleTimer::toggle(int numTimer) {
    if (numTimer >= n) {
        return;
    }

    enabled[numTimer] = !enabled[numTimer];
}



void MySimpleTimer::deleteArrays(){

	if(callbacks!=NULL) delete [] callbacks; callbacks=NULL;
	if(enabled!=NULL) delete [] enabled; enabled=NULL;
	if(delays!=NULL) delete [] delays; delays=NULL;
	if(numRuns!=NULL) delete [] numRuns; numRuns=NULL;
	if(maxNumRuns!=NULL) delete [] maxNumRuns; maxNumRuns=NULL;
	if(prev_millis!=NULL) delete [] prev_millis; prev_millis=NULL;

	return;
}

void MySimpleTimer::deleteTempArrays(){

	if(tmp_callbacks!=NULL) delete [] tmp_callbacks; tmp_callbacks=NULL;
	if(tmp_enabled!=NULL) delete [] tmp_enabled; tmp_enabled=NULL;
	if(tmp_delays!=NULL) delete [] tmp_delays; tmp_delays=NULL;
	if(tmp_numRuns!=NULL) delete [] tmp_numRuns; tmp_numRuns=NULL;
	if(tmp_maxNumRuns!=NULL) delete [] tmp_maxNumRuns; tmp_maxNumRuns=NULL;
	if(tmp_prev_millis!=NULL) delete [] tmp_prev_millis; tmp_prev_millis=NULL;

	return;

}

void MySimpleTimer::createArrays(){

	deleteArrays();

	callbacks=new timer_callback[n];
	enabled=new bool[n];
	delays=new long[n];
	numRuns=new int[n];
	maxNumRuns=new int[n];
	prev_millis=new unsigned long[n];

	return;
}

void MySimpleTimer::createTempArrays(){

	deleteTempArrays();

    tmp_callbacks=new timer_callback[n];
    tmp_enabled=new bool[n];
    tmp_delays=new long[n];
    tmp_numRuns=new int[n];
    tmp_maxNumRuns=new int[n];
	tmp_prev_millis=new unsigned long[n];

	return;
}

void MySimpleTimer::copyArraystoTemp(){

	int i;

	createTempArrays();

	for (int i = 0; i < n; i++) {
    tmp_callbacks[i]=callbacks[i];
    tmp_enabled[i]=enabled[i];
    tmp_delays[i]=delays[i];
    tmp_numRuns[i]=numRuns[i];
    tmp_maxNumRuns[i]=maxNumRuns[i];
	tmp_prev_millis[i]=prev_millis[i];
	}

	return;
}

void MySimpleTimer::copyTemptoArrays(){

	int i;

	createArrays();

	for (int i = 0; i < n-1; i++) {
    callbacks[i]=tmp_callbacks[i];
    enabled[i]=tmp_enabled[i];
    delays[i]=tmp_delays[i];
    numRuns[i]=tmp_numRuns[i];
    maxNumRuns[i]=tmp_maxNumRuns[i];
	prev_millis[i]=tmp_prev_millis[i];
	}

	return;
}

