#include "TimerList.h"

//#define prnt

TimerList::MyTimer * TimerList::setTimer(long d, timer_callback f, int nRUN, bool e)
{
if(getnumTimers()>=MAX_NUM_TIMERS) return NULL;

MyTimer *new_ptr = new MyTimer(d, f, nRUN, ID++, e);

if (last_ptr!=NULL) new_ptr->prev_ptr = last_ptr;

if (first_ptr==NULL) first_ptr = new_ptr;
if (last_ptr!=NULL) last_ptr->next_ptr = new_ptr;

last_ptr=new_ptr;


return new_ptr;
}


TimerList::MyTimer * TimerList::setTimeout(long d, timer_callback f){
#if defined prnt
		Serial.print(ID); Serial.print("+ ("); Serial.print(getnumTimers()+1); Serial.println(")"); 
# endif
	return setTimer(d, f, RUN_ONCE, true);
}

TimerList::MyTimer * TimerList::setInterval(long d, timer_callback f) {
	#if defined prnt
Serial.print(ID); Serial.print("+ ("); Serial.print(getnumTimers()+1); Serial.println(")"); 	
# endif
	return setTimer(d, f, RUN_FOREVER, true);
}

TimerList::MyTimer * TimerList::setTimeout(long d, timer_callback f, bool e){
#if defined prnt
Serial.print(ID); Serial.print("+ ("); Serial.print(getnumTimers()+1); Serial.println(")");
# endif
	return setTimer(d, f, RUN_ONCE, e);
}

TimerList::MyTimer * TimerList::setInterval(long d, timer_callback f, bool e) {
	#if defined prnt
Serial.print(ID); Serial.print("+ ("); Serial.print(getnumTimers()+1); Serial.println(")"); 	
# endif
	return setTimer(d, f, RUN_FOREVER, e);
}

//void TimerList::searchanddeleteTimer(TimerList::MyTimer * this_ptr)
//{
//
//MyTimer *next_ptr; 
//
//if (this_ptr == last_ptr){ last_ptr=this_ptr->prev_ptr; delete this_ptr;  this_ptr=NULL; return;}
//else if (this_ptr == NULL) return;
//else{
//
//next_ptr = last_ptr;
//	while (1){
//
//		if(next_ptr->prev_ptr==this_ptr)
//		{
//			next_ptr->prev_ptr=this_ptr->prev_ptr;
//			delete this_ptr; this_ptr=NULL;
//			return;
//		}
//
//		next_ptr = next_ptr->prev_ptr;
//		if (next_ptr == NULL) return;
//	}
//
//}
//return;
//}
//
//void TimerList::searchanddeleteTimer(int timerId)
//{
//
//MyTimer *next_ptr, *this_ptr; 
//
//if (timerId == last_ptr->id){ this_ptr=last_ptr; last_ptr=this_ptr->prev_ptr; delete this_ptr;  this_ptr=NULL; return;}
//else{
//
//next_ptr = last_ptr;
//	while (1){
//		this_ptr=next_ptr->prev_ptr;
//
//		if (this_ptr == NULL) return;
//
//		if(timerId ==this_ptr->id)
//		{
//			next_ptr->prev_ptr=this_ptr->prev_ptr;
//			delete this_ptr; this_ptr=NULL;
//			return;
//		}
//
//		next_ptr = next_ptr->prev_ptr;
//		if (next_ptr == NULL) return;
//	}
//
//}
//return;
//}
//
//
//
//


void TimerList::deletetimer(TimerList::MyTimer * this_ptr)
{

	if (this_ptr == NULL) return;
	else if (first_ptr == last_ptr){
		 first_ptr=NULL; last_ptr=NULL;}
	else if (this_ptr == last_ptr){
		last_ptr=this_ptr->prev_ptr;
		(this_ptr->prev_ptr)->next_ptr=NULL;
		}
	else if (this_ptr == first_ptr) {
		first_ptr=this_ptr->next_ptr;
		(this_ptr->next_ptr)->prev_ptr=NULL;
		}
	else{
		(this_ptr->prev_ptr)->next_ptr=this_ptr->next_ptr;
		(this_ptr->next_ptr)->prev_ptr=this_ptr->prev_ptr;
		}

			delete this_ptr;  this_ptr=NULL; return;
}


void TimerList::deleteTimer(TimerList::MyTimer * this_ptr)
{
if(this_ptr==NULL) return;

#if defined prnt
		Serial.print(this_ptr->id); Serial.print("deleted ("); 
# endif

deletetimer(this_ptr);

#if defined prnt
	Serial.print(getnumTimers()); Serial.println(")"); 
# endif

}

void TimerList::deleteTimer(int timerId)
{
	MyTimer *this_ptr;

if (timerId == last_ptr->id) this_ptr=last_ptr;
else if (timerId == first_ptr->id) this_ptr=first_ptr;
else{
	this_ptr = last_ptr;
	while(this_ptr!=NULL&&timerId != this_ptr->id) this_ptr=this_ptr->prev_ptr; 
}

deleteTimer(this_ptr);

}









void TimerList::reset() {

MyTimer *prev_ptr, *this_ptr = last_ptr;

if (this_ptr == NULL) return;

while (1){
	prev_ptr = this_ptr->prev_ptr;


                deleteTimer(this_ptr);

    this_ptr = prev_ptr;
	if (this_ptr == NULL) break;
}

last_ptr=NULL; first_ptr=NULL; ID=0; 
	return;
}




int TimerList::getnumTimers() {

	int i=0;
MyTimer *prev_ptr, *this_ptr;


this_ptr = last_ptr;

if (this_ptr == NULL) return i;

while (1){
	prev_ptr = this_ptr->prev_ptr;


                i++;

    this_ptr = prev_ptr;
	if (this_ptr == NULL) break;
}
 
	return i;
}





void TimerList::run() {

	MyTimer *this_ptr, *next_ptr; 

this_ptr = first_ptr;

if(this_ptr ==NULL) return; 

 unsigned long current_millis = millis();


while (1){

	this_ptr->toBeCalled = DEFCALL_DONTRUN;

	if (this_ptr->toBeDeleted) this_ptr->toBeCalled = DEFCALL_DEL;

        // no callback == no timer, i.e. jump over empty slots
	else if (this_ptr->callback) {

            // is it time to process this timer ?
            // see http://arduino.cc/forum/index.php/topic,124048.msg932592.html#msg932592

            if (current_millis - this_ptr->prev_millis >= this_ptr->delay) {

                // update time
                //prev_millis[i] = current_millis;
                this_ptr->prev_millis += this_ptr->delay;

                // check if the timer callback has to be executed
				if (this_ptr->enabled) {

                    // "run forever" timers must always be executed
					if (this_ptr->maxNumRuns == RUN_FOREVER) {
                        this_ptr->toBeCalled = DEFCALL_RUNONLY;
                    }
                    // other timers get executed the specified number of times
					else if (this_ptr->numRuns < this_ptr->maxNumRuns) {
                        this_ptr->toBeCalled = DEFCALL_RUNONLY;
                        this_ptr->numRuns++;

                        // after the last run, delete the timer
                        if (this_ptr->numRuns >= this_ptr->maxNumRuns) {
                            this_ptr->toBeCalled = DEFCALL_RUNANDDEL;
                        }
                    }
                }

            }
        }


	this_ptr = this_ptr->next_ptr;
	if (this_ptr == NULL) break;
}



this_ptr = first_ptr;
while (1){
	next_ptr = this_ptr->next_ptr;

        switch(this_ptr->toBeCalled) {
            case DEFCALL_DONTRUN:
                break;

            case DEFCALL_RUNONLY:
				//Serial.print(this_ptr->id);Serial.println("calling"); 
                (this_ptr->callback)();
                break;

            case DEFCALL_RUNANDDEL:
				//Serial.print(this_ptr->id);Serial.println("calling"); 
                (this_ptr->callback)();
				deleteTimer(this_ptr);
                break;

				case DEFCALL_DEL:
                deleteTimer(this_ptr);
                break;
        }

    this_ptr = next_ptr;
	if (this_ptr == NULL) break;
}

	return;
}




