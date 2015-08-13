#include "Timeout.h"

Timeout::Timeout()
{

	serialTimeout=NULL;
	convTimeout=NULL;
	inConversation=false;

	return;
}

Timeout::~Timeout()
{
	return;
}



void Timeout::resetSerial(void){
	if(serialTimeout!=NULL) serialTimeout->deleteTimer(); serialTimeout=NULL;
}
void Timeout::resetConversation(void){
	inConversation=false;
	if(convTimeout!=NULL) convTimeout->deleteTimer();  convTimeout=NULL;
}
void Timeout::startSerialTimeout(void){

	if(serialTimeout!=NULL) serialTimeout->deleteTimer(); 
	serialTimeout=t.setTimeout(timeoutInterval, SERIALTIMEOUT);

	//if(dbg) Serial.println("start Serial Timeout");
}

void Timeout::exitSerialTimeout(void){
	if(serialTimeout!=NULL) serialTimeout->deleteTimer(); 
	serialTimeout=NULL;

	resetSerial();

	//if(dbg) Serial.println("exit serial timeout");
}


void Timeout::startConvTimeout(void){

	inConversation=true;

	stopTimers();

	ENABLEBLINKCONV();

	delay(750);

	if(convTimeout!=NULL) convTimeout->deleteTimer(); 
	convTimeout=t.setTimeout(timeoutInterval, CONVTIMEOUT);


		if(dbg) Serial.println("start Conversation Timeout");
}


void Timeout::exitConvTimeout(void){

	inConversation=false;

	DISABLEBLINKCONV();

	if(convTimeout!=NULL) convTimeout->deleteTimer(); 
	convTimeout=NULL;

	resetConversation();

	startTimers();

	if(dbg) Serial.println("exit conversation timeout");
}




void Timeout::serialtimeout()
{

	if(serialTimeout==NULL) return;
	else{
			
//if(prnt) Serial.println("serial timed out");

exitSerialTimeout();}

}

void Timeout::convtimeout()
{

	if(convTimeout==NULL) return;
	else{
			
//if(prnt) Serial.println("conversation timed out");

exitConvTimeout();}

}







