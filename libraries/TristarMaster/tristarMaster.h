
#include "Tristar.h"

#ifndef TRISTARMASTER
#define TRISTARMASTER

const unsigned long logTristarInterval=600000; //every ten mins

const byte NORMAL=0;
const byte POOR=1;
const byte RICH=2;

class tristarMaster: public Tristar
{
	public:
		tristarMaster(void);
		~tristarMaster(void){};
		void reset();

		virtual void resetConversation(void);
		virtual void stopTimers(void);
		virtual void startTimers(void);

		void run();

		void connectDevice(void);
		//void readDeviceIdentification(void);
		//void readDeviceSerial(void);
		//void readLog(void);
		void getAnalogStatus(void);
		void issue(void);

		void preprocessMessage(byte);
		void processMessage(void);

		void execCommand(int);

		void sendmemorytoARC(byte);
		void update(byte);
		void logTristar(byte);

TimerList::MyTimer *timerlogTristar;

	private:

		bool deviceconnected, tristarLogStarted, tristarLogReceived;
		 byte batteryCondition[3];
		 int  whoisPOOR;

};
#endif

extern void WRITEMEMORYTOSD(byte);
extern void LOGTRISTAR();
extern void RESETALL();
extern void SUCCESS();
extern void ALLSOLARTO1();
extern void ALLSOLARTO2();
extern void RESETSOLAR();
