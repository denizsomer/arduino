
#include "Tristar.h"

#ifndef TRISTARSLAVE
#define TRISTARSLAVE

class tristarSlave: public Tristar
{
	public:
		tristarSlave(void);
		~tristarSlave(void){};
		void reset();
		void run();

		void preprocessMessage(byte);
		void processMessage(void);

		void writeCommand(int);

};
#endif

extern void SUCCESS();
extern void REPORTMEMORY(byte);
extern void REPORTALARMS(byte);
extern void RESETSLAVE();