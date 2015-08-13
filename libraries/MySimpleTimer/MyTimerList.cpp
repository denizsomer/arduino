

#include <stddef.h>
#include "MyTimerList.h"

MyTimerList::MyTimerList() {

n=0;id=0;
timers=NULL;
tmp_timers=NULL;

return;

}

MyTimerList::~MyTimerList()
{
free();
  return;
}

void MyTimerList::free()
{
		if(timers!=NULL){
	for (int i = 0; i < n; i++){if(timers[i]!=NULL) delete  timers[i];  timers[i]=NULL;}

	 delete [] timers; timers=NULL;}

		if(tmp_timers!=NULL) delete [] tmp_timers; tmp_timers=NULL;

  return;
}
void MyTimerList::run() {
    int i;
    unsigned long current_millis;
   
	int *toBeCalled=new int[n];

    // get current time
    current_millis = millis();

    for (i = 0; i < n; i++) {

        toBeCalled[i] = DEFCALL_DONTRUN;

        // no callback == no timer, i.e. jump over empty slots
        if (timers[i]->callback) {

            // is it time to process this timer ?
            // see http://arduino.cc/forum/index.php/topic,124048.msg932592.html#msg932592

            if (current_millis - timers[i]->prev_millis >= timers[i]->delay) {

                // update time
                //prev_millis[i] = current_millis;
                timers[i]->prev_millis += timers[i]->delay;

                // check if the timer callback has to be executed
                if (timers[i]->enabled) {

                    // "run forever" timers must always be executed
                    if (timers[i]->maxNumRuns == RUN_FOREVER) {
                        toBeCalled[i] = DEFCALL_RUNONLY;
                    }
                    // other timers get executed the specified number of times
                    else if (timers[i]->numRuns < timers[i]->maxNumRuns) {
                        toBeCalled[i] = DEFCALL_RUNONLY;
                        timers[i]->numRuns++;

                        // after the last run, delete the timer
                        if (timers[i]->numRuns >= timers[i]->maxNumRuns) {
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
                (*timers[i]->callback)();
                break;

            case DEFCALL_RUNANDDEL:
                (*timers[i]->callback)();
                deleteTimer(timers[i]);
                break;
        }
    }

	if(toBeCalled!=NULL) delete [] toBeCalled; toBeCalled=NULL;
	return;
}



MyTimer * MyTimerList::setTimer(long d, timer_callback f, int nRUN) {

	if (f == NULL) return NULL;

	//copyListtoTemp();

	n=1;

	createList();

	//copyTemptoList();

	//timers[n-1]= new MyTimer(d, f, nRUN, id++);

    //return timers[n-1];

	return NULL;

}


void MyTimerList::deleteTimer(MyTimer *timer) {
   
	copyListtoTemp();

	n--;

	createList();

	int j=0;
	for (int i = 0; i < n+1; i++) {
		if (tmp_timers[i]!=timer) timers[j++]=tmp_timers[i]; 
		tmp_timers[i]=NULL;
	}

	delete [] tmp_timers; tmp_timers=NULL;

	delete  timer; timer=NULL;

	return;
}



MyTimer *  MyTimerList::setInterval(long d, timer_callback f) {
    return setTimer(d, f, RUN_FOREVER);
}


MyTimer *  MyTimerList::setTimeout(long d, timer_callback f) {
    return setTimer(d, f, RUN_ONCE);
}





void MyTimerList::copyListtoTemp(){

	if(n==0) return;

    tmp_timers=new MyTimer *[n];

	for (int i = 0; i < n; i++) {tmp_timers[i]=timers[i]; timers[i]=NULL;}

	return;
}

void MyTimerList::createList(){

	//if(timers!=NULL) delete [] timers; timers=NULL;

		if(timers!=NULL){
	for (int i = 0; i < n; i++){if(timers[i]!=NULL) delete  timers[i];  timers[i]=NULL;}
}
	 delete [] timers; timers=NULL;


	timers=new MyTimer *[n]; for (int i = 0; i < n; i++) timers[i]=NULL;



	return;
}



void MyTimerList::copyTemptoList(){

	if(n==1) return;

	for (int i = 0; i < n-1; i++) {timers[i]=tmp_timers[i]; tmp_timers[i]=NULL;}

	delete [] tmp_timers; tmp_timers=NULL;

	return;
}



