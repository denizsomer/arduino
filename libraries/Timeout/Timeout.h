
#ifndef TIMEOUT
#define TIMEOUT

#include "TimerList.h"

extern TimerList t;
extern const bool prnt, dbg;

const unsigned int timeoutInterval=3000;

class Timeout
{
	public:
		Timeout(void);
		virtual ~Timeout(void);

		void exitSerialTimeout(void);
		void startSerialTimeout(void);
		void exitConvTimeout(void);
		void startConvTimeout(void);
		void serialtimeout();
		void convtimeout();

		virtual void resetConversation(void);
		virtual void resetSerial(void);
		virtual void stopTimers(void){return;}
		virtual void startTimers(void){return;}

		bool inConversation;




TimerList::MyTimer *serialTimeout, *convTimeout;

};
#endif

extern void SERIALTIMEOUT();
extern void CONVTIMEOUT();
extern void ENABLEBLINKCONV();
extern void DISABLEBLINKCONV();

