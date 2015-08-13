

#ifndef Phone_h
#define Phone_h

#include "Timeout.h"
#include "MyString.h"
#include "TimerList.h"

extern TimerList t;
extern const bool prnt, dbg, prout, prin;
extern const byte TRISTAR14, TRISTAR12,ARD;


extern void RESETALL();

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

const unsigned int phoneDiscardInterval=10000,phncheckInterval=10000;

const byte _encodeMask []= { 1, 3, 7, 15, 31, 63, 127 };
const byte _decodeMask []=  { 128, 192, 224, 240, 248, 252, 254 };

const byte isCIND=0, isCMGL=1, isCPMS=2,
          isCMGR=3, isCMGF=4, isCMGS=5, 
		  isRING=6, isCMGD=7, isCMGW=8, 
		  isCKEV=9, isCLIP=10, isOK=11,  FROMPHONE=0, FROMMONITOR=1;

const char CIND[9]="+CIND:/0", CMGL[9]="+CMGL:/0", CMGR[9]="+CMGR:/0",
CMGF[7]="CMGF/0", CMGS[7]="CMGS/0", CKEV[9]="+CKEV:/0", CLIP[9]="+CLIP:/0",
myPDU[13]="7727523900/0", myNUM[13]="7772259300/0",
CPMS[9]="+CPMS:/0", RING[7]="RING/0", CMGD[7]="CMGD/0" ,  CMGW[7]="CMGW/0", OK[5]="OK/0";

class Phone: public Timeout
{
	public:

		Phone(void);
		byte* PackBytes(byte* , int, int&);
		byte* UnpackBytes(byte* , int, int&);
		void reset(bool, bool);
		void setBaud(word);
		word getBaud(void);
		void startPhone(void);
		void startMonitor(void);
		void stopMonitor(void);
		void listSMS(void);
		void readSMS(void);
		void processSMS(void);
		void checkStatus(void);
		void deleteSMS(void);
		void appendTodeleteQueue(byte);
		void deletelastFromdeleteQueue(void);

		void sendSMS(void);
		void storeSMS(void);
		void answerCall(void);
		void hangUp(void);
		void alarm(void);





		bool enabled(){return isEnabled;}
		void enable(){isEnabled=true; PhoneEnabled=isPhone;
		MonitorEnabled=isMonitor;
		if(dbg) {Serial.print("PHONES enabled:");Serial.print(PhoneEnabled);Serial.println(MonitorEnabled);}
		}
		void disable(){isEnabled=false; PhoneEnabled=false; MonitorEnabled=false;
		if(dbg) {Serial.print("PHONES disabled:");Serial.print(PhoneEnabled);Serial.println(MonitorEnabled);}
		}
		void disablePhone(){PhoneEnabled=false; if(dbg) Serial.println("Phone enabled:");}
		void disableMonitor(){MonitorEnabled=false; if(dbg) Serial.println("Monitor enabled:"); }
		

// serial engine
		void managePhone(void);
		void manageMonitor(void);

		virtual void resetSerial(void);
		virtual void resetConversation(void);
		virtual void stopTimers(void);
		virtual void startTimers(void);


		int run(void);

		//reports
		void execCommand(byte);
		void listOther(void);
		void listSol(void);
		void listAl(void);
		void listPhn(void);
		void listReports(void);

		
		void reportDouble(float val, int nfrac);
		void reportLong(long val);
		void timeout();



//resetConversation
MyString SMS;

private:

	//reset
HardwareSerial* phoneSerial, *monitorSerial;

word _baud, _frameDelay;
bool isPhone, isMonitor, isEnabled, PhoneEnabled, MonitorEnabled;

int nPDU, idigit, nexe;

//resetConversation
		byte *packedBytes, *deleteQueue, ndel;
		int  packedBytesLength;
bool ATOK, CMGFOK, CMGSOK, CMGWOK, READY;

//resetSerial()

MyString PDU;

bool  ended, started, more;

byte phnResponse, counterSerial, FROM,
jCIND,  jCMGL,  jCMGR,  jCMGF,  jCMGS,  jCMGD,  jCPMS,  jRING,  jCMGW,  jCKEV,  jCLIP, jOK, 
jCIND0, jCMGL0, jCMGR0, jCMGF0, jCMGS0, jCMGD0, jCPMS0, jRING0, jCMGW0, jCKEV0, jCLIP0, jOK0;


//t.reset
TimerList::MyTimer *timercheckMonitor,*timercheckPhone, *timerPhoneTimeout;

};

#endif

extern void REPORTMEMORY(byte);
extern void REPORTALARMS(byte);
extern void DISABLESLAVE();
extern void ENABLESLAVE();
extern void WRITECOMMAND(int);

extern void REPORTAL(MyString *);
extern void REPORTSTATE(MyString *);
extern void REPORTOUTPUTS(MyString *);
extern void REPORTTIMERSINCE(MyString *);
extern void REPORTTIMERTOGO(MyString *);

extern void SUCCESS();		
extern void LISTENING(bool);

extern void CHECKPHONE();
extern void CHECKMONITOR();





