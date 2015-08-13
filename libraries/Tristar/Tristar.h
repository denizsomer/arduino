
#ifndef TRISTAR
#define TRISTAR


#include <modbus.h>
#include <modbusDevice.h>
#include "MyString.h"
#include "TimerList.h"
#include "Timeout.h"

extern TimerList t;
extern const bool prnt, dbg, prout, prin;
const int byteOffset[]={0,10000,0};




extern const byte TRISTAR14, TRISTAR12,ARD;


class Tristar: public Timeout
{
	public:
		Tristar(void);
		virtual ~Tristar(void);

		virtual void reset(void);
		virtual void run(void);

		virtual	void preprocessMessage(byte){};
		virtual	void processMessage(void){exitSerialTimeout();}



		void setBaud(word);
		word getBaud(void);

		void setSerial(byte);
		void checkSerial(void);
		void serialRx(void);

		virtual void resetSerial(void);

		virtual void resetConversation(void){Timeout::resetConversation();};
		virtual void stopTimers(void){};
		virtual void startTimers(void){};


		void calcCrc(void);

		void setStatus(word, word);
		void sendStatus(word, word);
		void getStatus(word, word);

		bool writeMemorytoString(byte, MyString *);
		void writeAlarmstoString(byte, MyString *);


		void printMemory(byte);


		bool enabled(){return isEnabled;}
		void enable(){isEnabled=true;
		//if(prnt) Serial.println("TRISTAR enabled");
		}
		void disable(){isEnabled=false;
		//if(prnt) Serial.println("TRISTAR disabled");
		}

		void sendMsg();

		void State(byte , MyString*);
		void Alarms(byte , MyString*);
		void Faults(byte , MyString*);


		modbusDevice *_device;

				byte *_msg,
			  _len, _sid, _ndata, _funcType;

	//private:

 		HardwareSerial* tristarSerial;

//TimerList::MyTimer *serialTimeout, *convTimeout;

bool  _ended, _started;



		byte auchCRCHi(byte);
		byte auchCRCLo(byte);
		byte COMMAND(byte);
		void hextoChar(byte);

		bool isEnabled;


		word _baud,
			 _crc,
			 _frameDelay;
};
#endif

extern void TRISTARSERIALTIMEOUT();
extern void TRISTARCONVTIMEOUT();

