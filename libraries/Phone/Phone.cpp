#include "Phone.h"

extern TimerList::MyTimer* timerReset;
extern const unsigned long resetInterval=43200000;

extern const bool prnt,dbg;


Phone::Phone(void){
	
	phoneSerial=NULL;
	monitorSerial=NULL;

	timercheckMonitor=NULL; 
	timercheckPhone=NULL; 
	timerPhoneTimeout=NULL;
	deleteQueue=NULL;
	ndel=0;
	more=false;


	reset(false, false);
	
	return;}

void Phone::appendTodeleteQueue(byte num){


	if(deleteQueue==NULL) {
	ndel=1;
	deleteQueue= new byte[ndel];
	deleteQueue[0]=num;
		//if(prnt) {Serial.print("appended "); Serial.print(num); Serial.print(" to delete queue, ndel= "); Serial.println(ndel);}
	return;	
	}

	int i=0;

	while (i<ndel&&num!=deleteQueue[i])  i++;

	if (i<ndel) {
		//Serial.print(num); Serial.print(" already in queue, ndel= "); Serial.println(ndel);
		return;}

byte* tmpdel= new byte[ndel];

for (i=0; i<ndel; i++) tmpdel[i]=deleteQueue[i];

delete [] deleteQueue;

deleteQueue= new byte[ndel+1];

for (i=0; i<ndel; i++) deleteQueue[i]=tmpdel[i];

ndel++;

deleteQueue[ndel-1]= num;



delete [] tmpdel;

	if(prnt) {Serial.print("appended "); Serial.print(num); Serial.print(" to delete queue, ndel= "); Serial.println(ndel);}


}

void Phone::deletelastFromdeleteQueue(void){

	if(deleteQueue==NULL) {
	return;	
	}

	byte num=deleteQueue[ndel-1];


byte* tmpdel= new byte[ndel];

for (int i=0; i<ndel; i++) tmpdel[i]=deleteQueue[i];

delete [] deleteQueue;

ndel--;

if (ndel==0){deleteQueue=NULL; delete [] tmpdel; return;}

deleteQueue= new byte[ndel];

for (int i=0; i<ndel; i++) deleteQueue[i]=tmpdel[i];

delete [] tmpdel;

		//if(prnt) {Serial.print("deleted "); Serial.print(num); Serial.print(" from delete queue, ndel= "); Serial.println(ndel-1);}


}



void Phone::setBaud(word baud)
{
	_baud = baud;

	_frameDelay = 0; //24000/_baud;


	if(prnt) Serial.println("setting phone baud.......");
}

word Phone::getBaud(void)
{
	return(_baud);
}






void Phone::reset(bool phn, bool mntr){

	if(deleteQueue!=NULL) delete [] deleteQueue; deleteQueue=NULL;
	ndel=0;

if(prnt) Serial.println("initializing phone.......");

	nPDU=0;
	nexe=-1;

isPhone=phn;
isMonitor=mntr;
if(isPhone||isMonitor) {
enable();
resetSerial();
resetConversation();
}

if(isPhone){
FROM=FROMPHONE;
if(timercheckPhone!=NULL) timercheckPhone->deleteTimer(); timercheckPhone=NULL;
timercheckPhone= t.setInterval(phncheckInterval,CHECKPHONE );

phoneSerial=&Serial3;


startPhone();
delay(1000);
}

if(isMonitor){
FROM=FROMMONITOR;
if(timercheckMonitor!=NULL) timercheckMonitor->deleteTimer(); timercheckMonitor=NULL;
timercheckMonitor= t.setInterval(phncheckInterval,CHECKMONITOR );

monitorSerial=&Serial2;


startMonitor();
delay(1000);
}

if(prnt) {
	Serial.println("Phone initialized.");
}


return;
}





void Phone::resetSerial(){

 ended=false; started=false; 

phnResponse;
counterSerial=0; 
jCIND=0;      jCMGL=0;     jCMGR=0;      jCMGF=0;  
jCMGS=0;  jCMGD=0;     jCPMS=0;     jRING=0; jCMGW=0; jCKEV=0, jCLIP=0;
jCIND0=0;     jCMGL0=0;    jCMGR0=0;     jCMGF0=0;
jCMGS0=0; jCMGD0=0;    jCPMS0=0;    jRING0=0; jCMGW0=0;  jCKEV0=0, jCLIP0=0, jOK=0, jOK0=0;

PDU.free();

Timeout::resetSerial();

}



void Phone::resetConversation(){
if(packedBytes!=NULL) delete [] packedBytes; packedBytes=NULL; packedBytesLength=0;
READY=true;
ATOK=false, CMGFOK=false,  CMGSOK=false,  CMGWOK=false;

SMS.free();

Timeout::resetConversation();
}





void Phone::storeSMS(){


		if(!CMGFOK)
		{
		SUCCESS();
		startConvTimeout();
		stopMonitor();
		delay(2000);  //because of the above. do not remove
			monitorSerial->println("AT+CMGF=0"); 
		}
		else 
		{
			startConvTimeout();
			if(!CMGWOK){
				packedBytes=PackBytes((byte*)SMS.asCharArray(), SMS.length(),  packedBytesLength);

				monitorSerial->print("AT+CMGW=");
				monitorSerial->println((28+2*packedBytesLength)/2);
			}
			else {
				monitorSerial->print("0011000C914477275239000000AA");
				if(SMS.length()<16) monitorSerial->print('0');
				monitorSerial->print(SMS.length(),HEX); 
				for(int i=0; i<packedBytesLength; i++){
				if((int)packedBytes[i]<16) {monitorSerial->print('0');}
				monitorSerial->print(packedBytes[i],HEX); 
				}

				startConvTimeout();
				monitorSerial->write(26);

			}
		}

READY=false;
return;
}


void Phone::sendSMS(){



		if(!CMGFOK)
		{	SUCCESS();
			startConvTimeout();
			phoneSerial->println("AT+CMGF=0"); 

		}
		else 
		{
			startConvTimeout();
			if(!CMGSOK){

				packedBytes=PackBytes((byte*)SMS.asCharArray(), SMS.length(),  packedBytesLength);

				phoneSerial->print("AT+CMGS=");
				phoneSerial->println((28+2*packedBytesLength)/2);
			}
			else {

				phoneSerial->print("0011000C914477275239000000AA");
				if(SMS.length()<16) phoneSerial->print('0');
				phoneSerial->print(SMS.length(),HEX); 
				for(int i=0; i<packedBytesLength; i++){
				if((int)packedBytes[i]<16) {phoneSerial->print('0');}
				phoneSerial->print(packedBytes[i],HEX); 
				}

				startConvTimeout();
				phoneSerial->write(26);

			}
		}

READY=false;
return;
}






int Phone::run(void)
{
	int i;
	MyString tmpSMS;
	if(READY&&SMS.length()>0) {
		if(dbg) {Serial.println("SMS.length="); Serial.println(SMS.length());}
		if (SMS.length()>159) 
		{for(i=0; i<160; i++) tmpSMS.append(SMS[i]);
		SMS.free();
		for(i=0; i<160; i++) SMS.append(tmpSMS[i]);
		tmpSMS.free();
		}

		if(FROM==FROMMONITOR) storeSMS();
		else sendSMS();
	}

	if(isPhone) this->managePhone();	
	

if(isMonitor) this->manageMonitor();

return 0;
	}





void Phone::managePhone(){

	
int i, j, jmyPDU0=0, jmyPDU=0, unpackedBytesLength;
char character;
bool ismynumber=false, isPrompt=false;
byte* msg, len,  *unpackedBytes;
int PDUStatus, totPDU;
MyString tmpstr;


		while(phoneSerial->available()&&!ended)
		{
			delay(_frameDelay);
 
	 character = phoneSerial->read();
	 //if(dbg) 	Serial.print(character);
	counterSerial++;


		if(!started) 
		{
			//if(dbg) {Serial.print(character); Serial.println(jOK);}
		 if(jCIND>5)  {resetSerial(); phnResponse=isCIND;  started=true;}
	else if(jCMGL>5)  {resetSerial(); phnResponse=isCMGL;  started=true;} 
	else if(jCPMS>5)  {resetSerial(); phnResponse=isCPMS;  started=true;}
	else if(jCMGR>5)  {resetSerial(); phnResponse=isCMGR;  started=true;}
	else if(jCMGF>3)  {resetSerial(); phnResponse=isCMGF;  started=true;}
	else if(jCMGS>3)  {resetSerial(); phnResponse=isCMGS;  started=true;}
	else if(jRING>3)  {resetSerial(); phnResponse=isRING; ended=true; break;}
	else if(jOK>1)  {resetSerial(); phnResponse=isOK; ended=true; break;}
	else if(jCMGD>3)  {resetSerial(); phnResponse=isCMGD; started=true; break;}
	else if(jCMGW>3)  {resetSerial(); phnResponse=isCMGW; started=true; }
	else if(jCLIP>5)  {resetSerial(); phnResponse=isCLIP; started=true; }
	else{

	if(character==OK[jOK]){
		if(jOK==0) {jOK0=counterSerial; jOK++;} 
		else	{
			if(jOK0+jOK==counterSerial) jOK++;
			else jOK=0;
		}
	}
	else jOK=0;


	if(character==CIND[jCIND]){
		if(jCIND==0) {jCIND0=counterSerial; jCIND++;} 
		else	{
			if(jCIND0+jCIND==counterSerial) jCIND++;
			else jCIND=0;
		}
	}
	else jCIND=0;

	if(character==CMGL[jCMGL]){
		if(jCMGL==0) {jCMGL0=counterSerial; jCMGL++;} 
		else	{
			if(jCMGL0+jCMGL==counterSerial) jCMGL++;
			else jCMGL=0;
		}
	}
	else jCMGL=0;

	if(character==CPMS[jCPMS]){
		if(jCPMS==0) {jCPMS0=counterSerial; jCPMS++;} 
		else	{
			 if(jCPMS0+jCPMS==counterSerial) jCPMS++;
			else jCPMS=0;
		}
	}
	else jCPMS=0;

	if(character==CMGR[jCMGR]){
		if(jCMGR==0) {jCMGR0=counterSerial; jCMGR++;} 
		else	{
			if(jCMGR0+jCMGR==counterSerial) jCMGR++;
			else jCMGR=0;
		}
	}
	else jCMGR=0;

	if(character==CMGF[jCMGF]){
		if(jCMGF==0) {jCMGF0=counterSerial; jCMGF++;} 
		else	{
			if(jCMGF0+jCMGF==counterSerial) jCMGF++;
			else jCMGF=0;
		}
	}
	else jCMGF=0;

	if(character==CMGS[jCMGS]){
		if(jCMGS==0) {jCMGS0=counterSerial; jCMGS++;} 
		else	{
			if(jCMGS0+jCMGS==counterSerial) jCMGS++;
			else jCMGS=0;
		}
	}
	else jCMGS=0;

	if(character==RING[jRING]){
		if(jRING==0) {jRING0=counterSerial; jRING++;}
		else	{
			if(jRING0+jRING==counterSerial) jRING++;
			else jRING=0;
		}
	}
	else jRING=0;

	if(character==CMGD[jCMGD]){
		if(jCMGD==0) {jCMGD0=counterSerial; jCMGD++;} 
		else	{
			if(jCMGD0+jCMGD==counterSerial) jCMGD++;
			else jCMGD=0;
		}
	}
	else jCMGD=0;

		if(character==CMGW[jCMGW]){
		if(jCMGW==0) {jCMGW0=counterSerial; jCMGW++;} 
		else	{
			if(jCMGW0+jCMGW==counterSerial) jCMGW++;
			else jCMGW=0;
		}
	}
	else jCMGW=0;

	if(character==CLIP[jCLIP]){
		if(jCLIP==0) {jCLIP0=counterSerial; jCLIP++;} 
		else	{
			if(jCLIP0+jCLIP==counterSerial) jCLIP++;
			else jCLIP=0;
					}
	}
	else jCLIP=0;

	}
	} //if !started
			else //started
			{
FROM=FROMPHONE;
startSerialTimeout();
			 PDU.append(character);
		 //if(dbg) {Serial.print("len:"); Serial.println(counterSerial);}
		//}
		  
		     //0791447728008000040C9144772752390xxxxxxx          //33
			 //0000051608001148440xxxxxxxxxxxxxxxxxxxxx         //19
			 //A0												//2
			 //D17B594ECFD7D36F78784E369FD1EA355B8F1FDB      //+280 max    = 334  -->  SERIAL_TX_BUFFER_SIZE=384
			 //C5EE76FC5E96D3F3F17B594ECFD7D36F78784E36
			 //9FD1EA355B8F1FDBC5EE76FC5E96D3F3F17B594E
			 //CFD7D36F78784E369FD1EA355B8F1FDBC5EE76FC
			 //5E96D3F3F17B594ECFD7D36F78784E369FD1EA35
			 //5B8F1FDBC5EE76FC5E96D3F3F17B594ECFD7D36F
			 //78784E369FD1EA355B8F1FDBC5EE76FC5E96D3F3
			 		// if(PDU.length().SERIAL_TX_BUFFER_SIZE)  //process one message at a time

				if((PDU.asCharArray()[PDU.length()-2]=='O'&&
				PDU.asCharArray()[PDU.length()-1]=='K'))
			{
				isPrompt=false; 
				ended=true; break;
			}
			else if((phnResponse==isCMGS||phnResponse==isCMGW)&&
				PDU.asCharArray()[PDU.length()-1]=='>')//
			{ 
				if(dbg) Serial.print(">");
				isPrompt=true;
				ended=true; break;
			}
				else if((phnResponse==isCMGL)&&PDU.length()>10&&
				PDU.asCharArray()[PDU.length()-1]=='\n')//
			{ 
				if(prnt) Serial.print("processing one at a time");
				more=true;
				
				ended=true; break;
			}	
				else if(phnResponse==isCLIP&&
				PDU.asCharArray()[PDU.length()-2]=='"'&&
				PDU.asCharArray()[PDU.length()-1]==',')
			{ 
				ended=true; break;
			}	
				else	if(
				PDU.asCharArray()[PDU.length()-5]=='E'&&
				PDU.asCharArray()[PDU.length()-4]=='R'&&
				PDU.asCharArray()[PDU.length()-3]=='R'&&
				PDU.asCharArray()[PDU.length()-2]=='O'&&
				PDU.asCharArray()[PDU.length()-1]=='R'
				)
			{
				isPrompt=false;
				ended=true; break;
			}

	

	 }
	} //SERIAL AVAILABLE



	if(ended)
	{

		switch(phnResponse){

			case isOK:
if(dbg) Serial.print("isOK");
				more=false;
				if(ndel>0) deleteSMS();
				//else exitConvTimeout();


				break;

			case isCLIP :
				if(dbg) Serial.print("isCLIP");
							i=0;
				while(i<PDU.length())
				{
					if(PDU.asCharArray()[i]==myNUM[jmyPDU])
					{
						if(jmyPDU==0) {jmyPDU0=i; jmyPDU++;} 
						else	
						{
						if(jmyPDU0+jmyPDU==i) jmyPDU++;
						else jmyPDU=0;
						}
					if(dbg) Serial.print(PDU[i]);
					}
					else jmyPDU=0;

					if(jmyPDU==9) {ismynumber=true; break;}

					i++;
				}

				delay(1000);
				answerCall();
				if(!ismynumber)
				{
					if(prnt) Serial.print("Call is not from me : ");
					delay(1000);
					hangUp();
				}
				else if(prnt) Serial.print("Call is from me : ");

				break;



				case isRING :
				if(prnt) Serial.print("RINGING!");
				delay(1000);
				phoneSerial->println("AT+CLIP=1");
				break;

				case isCMGD:
				if(dbg) Serial.println("Deleted SMS.");
			
					if(ndel>0) deleteSMS();
					else if(nexe>-1&&nexe<200) {Serial.println(nexe); execCommand(FROMPHONE); nexe=-1;}
	
				break;

	
				case isCMGL:
					if(dbg) Serial.println("isCMGL");
				i=0; while(i<PDU.length()&&PDU.asCharArray()[i]!=',') i++;
				if(i+1<PDU.length()) {
					//nPDU=charToInt(PDU.asCharArray()[i-1]);
					nPDU=PDU.charToInt(i-1);
				//PDUStatus=charToInt(PDU.asCharArray()[i+1]);
				PDUStatus=PDU.charToInt(i+1);
				}
				if(prnt) Serial.print("nPDU: "); if(prnt) Serial.print(nPDU); if(prnt) Serial.print("status: "); if(prnt) Serial.println(PDUStatus);

				if(PDUStatus==0||PDUStatus==1) 
				{
					if(prnt) Serial.print("Message ");if(prnt) Serial.print(nPDU);if(prnt) Serial.println(" is received");
				   processSMS();
				}
				else {
					if(prnt) Serial.print("Message ");if(prnt) Serial.print(nPDU);if(prnt) Serial.println(" is not received"); 
				}
				appendTodeleteQueue(nPDU);
				
				break;


				case isCMGR:
					processSMS();

				break;


				case isCIND:
				if(dbg) Serial.println("isCIND");
					break;

					

			case isCPMS:
				if(dbg) Serial.println("isCPMS");

				
				i=0; while(i<PDU.length()&&PDU.asCharArray()[i]!=',') i++;
				if(i<PDU.length()) totPDU=PDU.charToInt(i-1);
					//totPDU=charToInt(PDU.asCharArray()[i-1]);

				if( totPDU>0) { 
 
					if(prnt) Serial.print(totPDU); 
					if(prnt) Serial.println(" messages to process");

					listSMS();
				}
				else{ 
					if(dbg) {Serial.print(nexe); Serial.println("totPDU=0");}
	
				}
					break;

					case isCMGF:
						startConvTimeout();
						if(dbg) Serial.println("isCMGF");
						CMGFOK=true;
						READY=true;
					break;

					case isCMGS:
						startConvTimeout();

						CMGSOK=true;
						READY=true;
						if(isPrompt) {if(dbg)  Serial.println(">>>>>>");}
						else {
						if(prnt)  Serial.println("Message sent.");
						exitConvTimeout();
						}
					break;

					case isCMGW:
						startConvTimeout();
						
						CMGWOK=true;
						READY=true;
						if(isPrompt) {if(dbg)  Serial.println(">>>>>>");}
						else {
						if(prnt)  Serial.println("Message stored.");
						exitConvTimeout();
						}
					break;

					

					default:
						if(dbg) Serial.println("WTF");
						if(dbg) Serial.print(phnResponse);
					break;

		} //switch
	exitSerialTimeout();
	} //ended

return;
}


void Phone::manageMonitor(){

	int i, j, jmyPDU0=0, jmyPDU=0, unpackedBytesLength;
char character;
bool ismynumber=false, isPrompt=false;
byte* msg, len,  *unpackedBytes;


	while(monitorSerial->available()&&!ended)
{

character = monitorSerial->read();
 //if(prin) Serial.print(character);
counterSerial++;


	if(!started) 
	{
		//if(dbg) {Serial.print(jCKEV);  Serial.println(counterSerial);}
         if(jCKEV>3)  {resetSerial(); phnResponse=isCKEV;  started=true;}
	else if(jCMGF>3)  {resetSerial(); phnResponse=isCMGF;  started=true;}
	else if(jCMGW>3)  {resetSerial(); phnResponse=isCMGW; started=true; }
else{

		if(character==CKEV[jCKEV])	{
		if(jCKEV==0) {jCKEV0=counterSerial; jCKEV++;} 
		else	{
			if(jCKEV0+jCKEV==counterSerial) jCKEV++;
			else jCKEV=0;
				}
	}
	else jCKEV=0;

		if(character==CMGF[jCMGF]){
		if(jCMGF==0) {jCMGF0=counterSerial; jCMGF++;} 
		else	{
			if(jCMGF0+jCMGF==counterSerial) jCMGF++;
			else jCMGF=0;
		}
	}
	else jCMGF=0;

			if(character==CMGW[jCMGW]){
		if(jCMGW==0) {jCMGW0=counterSerial; jCMGW++;} 
		else	{
			if(jCMGW0+jCMGW==counterSerial) jCMGW++;
			else jCMGW=0;
		}
	}
	else jCMGW=0;


}
	} //if !started
		else //started
		{
FROM=FROMMONITOR;
startSerialTimeout();

//if(prin) {Serial.print(character); Serial.println(inConversation);}


			if(phnResponse==isCKEV&&character=='\n')
			{
				if(!inConversation){ended=true; break;}
				else  exitSerialTimeout();
			}

				PDU.append(character);

				if((phnResponse==isCMGW)&&
				PDU.asCharArray()[PDU.length()-1]=='>')//
			{ 
				if(prnt) Serial.print(">");
				ended=true; isPrompt=true; break;
			}
				if((PDU.asCharArray()[PDU.length()-2]=='O'&&
				PDU.asCharArray()[PDU.length()-1]=='K'))
			{
				ended=true; isPrompt=false; break; 			
			}


	}

} //SERIAL AVAILABLE

if(ended)
{
	
	switch(phnResponse){

						case isCKEV:
						if(dbg) Serial.println("isCKEV");

						if(PDU.asCharArray()[PDU.length()-2]=='0')
						{
								if(PDU.asCharArray()[PDU.length()-4]=='*') //cancel
								{
									LISTENING(false);
									nexe=-1;
								}
								else if(PDU.asCharArray()[PDU.length()-4]=='#') //start/finish recording
								{
									if(nexe>1) //do the job
									{ 
										if(prnt) {Serial.print("nexe:"); Serial.println(nexe);}

										LISTENING(false);

										execCommand(FROMMONITOR);
									}
									else 
									{
										LISTENING(true);
										nexe=0;
									}
								}
								else
								{
									if(nexe>-1) { nexe=nexe*10+PDU.charToInt(PDU.length()-4);
										
										//nexe=nexe*10+charToInt(PDU.asCharArray()[PDU.length()-4]);
									}
									
								}
						} //(PDU.asCharArray()[PDU.length()-2]=='0')


					break;


						case isCMGF:
						startConvTimeout();
						if(dbg) Serial.println("isCMGF");
						CMGFOK=true;
						READY=true;
					break;


					case isCMGW:
						startConvTimeout();
						CMGWOK=true;
						READY=true;
						if(isPrompt) {
						if(dbg) Serial.println(">>>>>>");
						}
						else {
						if(prnt)  Serial.println("Message stored.");
							exitConvTimeout();
						}
					break;

					default:
						if(dbg) Serial.println("WTF");
						if(dbg) Serial.print(phnResponse);
					break;

		}
						exitSerialTimeout();
	}		
}
 


void Phone::startMonitor() {
		if(dbg) Serial.println("startMonitor");
		monitorSerial->println("AT+CMER=3,2,0,0,0");
}
void Phone::stopMonitor() {
	monitorSerial->println("AT+CMER=3,0,0,0,0");
}
void Phone::startPhone() {
		if(dbg) Serial.println("startPhone");
		phoneSerial->println("AT+CPMS=\"ME\"");
}
void Phone::listSMS() {delay(1000); phoneSerial->println("AT+CMGL=4");}
void Phone::readSMS() {delay(1000); phoneSerial->print("AT+CMGR="); phoneSerial->println(nPDU);}
void Phone::processSMS() {

	int i, j, jmyPDU0=0, jmyPDU=0, unpackedBytesLength;
char character;
bool ismynumber=false, isPrompt=false;
byte* msg, len,  *unpackedBytes;
int PDUStatus, totPDU;
MyString tmpstr;



				i=0;
				while(i<PDU.length())
				{
					if(PDU.asCharArray()[i]==myPDU[jmyPDU])
					{
						if(jmyPDU==0) {jmyPDU0=i; jmyPDU++;} 
						else	
						{
						if(jmyPDU0+jmyPDU==i) jmyPDU++;
						else jmyPDU=0;
						}
					//if(prnt) Serial.print(PDU[i]);
					}
					else jmyPDU=0;

					if(jmyPDU==10) {ismynumber=true; break;}

					i++;
				}

				if(ismynumber)
				{
					if(prnt) Serial.print("Message ");if(prnt) Serial.print(nPDU);
					if(prnt) Serial.print(" is from me : "); 
//
//+CMGR: 1,,22  -->12+\n=13   
//[..........................+54 ..][............... +19] [+2(n)] [n*2]
//0791447728008000040C91447727523900000051605032755540     03    311B0C//
//  31 1B 0C -> 160
					

					i+=19;
					Serial.print("len "); Serial.print(PDU.asCharArray()[i]); Serial.println(PDU.asCharArray()[i+1]);
					len=PDU.hexToInt(i);

					if(len>0&&len<4){

					i+=2;

					msg = (byte *) malloc(len);

					j=0;
					while(j<len){
					msg[j]=PDU.hexToInt(i);
					i+=2;
					j++;					
					}

							unpackedBytes=UnpackBytes( msg,len,  unpackedBytesLength);

							nexe=0;
							for(i=0; i<unpackedBytesLength; i++) {
								
								Serial.println((char)unpackedBytes[i]);
							
							tmpstr.free();
							tmpstr.append((char) unpackedBytes[i]);

							if(tmpstr.charToInt(0)>-1&&tmpstr.charToInt(0)<10) nexe=10*nexe+tmpstr.charToInt(0);
							else {nexe=-1; tmpstr.free(); break;}

							if(!(nexe>-1&&nexe<200)) nexe=-1;

							tmpstr.free();
							}

								Serial.print("nexe="); Serial.println(nexe);

							free( msg); msg=NULL;
							if(unpackedBytes!=NULL) delete [] unpackedBytes; unpackedBytes=NULL; 
					} else{if(prnt) Serial.print("too long ");}

				}
				else
				{
					//not from me
					if(prnt) Serial.print("Message ");
					if(prnt) Serial.print(nPDU);
					if(prnt) Serial.println("is not from me");
					
				}


}
void Phone::checkStatus() {delay(1000); phoneSerial->println("AT+CIND?");}

void Phone::deleteSMS(void) {
	if(ndel==0) return;
	delay(1000);
	phoneSerial->print("AT+CMGD="); 
	phoneSerial->print(deleteQueue[ndel-1]);
	phoneSerial->println(",0");
		deletelastFromdeleteQueue();
		
}

void Phone::answerCall() {phoneSerial->println("ATA");} 
void Phone::hangUp() {phoneSerial->println("ATH");} 

//////////////////////////reports//////////////////////////////////////////////////////


void Phone::alarm(){nexe=93; execCommand(FROMPHONE);}
void Phone::execCommand(byte src)		
{
 
	char state[5];
	if(prnt) Serial.print("Executing ");if(prnt) Serial.println(nexe);

// toggle digital pins
if(nexe>-1&&nexe<54) { 
//arc.setPin(nexe,!arc.getPin(nexe)); 
if(prnt) Serial.print("Pin ");if(prnt) Serial.print(nexe); if(prnt) Serial.print(" is ");  if(prnt) Serial.println(digitalRead(nexe));


	SMS.append("Pin ");
	itoa(nexe,state,10); SMS.append(state);
	SMS.append(" is ");
	itoa(digitalRead(nexe),state,10); SMS.append(state);

} 
else if(nexe>99&&nexe<200){
WRITECOMMAND(nexe-100); 
if(nexe==160||nexe==161) {
SUCCESS();
startConvTimeout();
}
}
else switch(nexe){

//case 59: list
case 60: {REPORTMEMORY(TRISTAR14);break;}
case 61: {REPORTMEMORY(TRISTAR12);break;}
case 62: {REPORTALARMS(TRISTAR14); break;}
case 63: {REPORTALARMS(TRISTAR12); break;}
//case 69: list
//case 70: arc.resetAl(); break;
//case 71: arc.arm(); break;
//case 72: arc.disarm(); break;
//case 73: arc.playWrn(1000); break;
//case 74: arc.playWrn(4000); break;
//case 77: arc.playSrn(); break;
//case 79: list
case 84: checkStatus(); break;
//case 89: list
case 90: RESETALL(); break;
//reports
case 91: {REPORTTIMERSINCE(&SMS);break;}
case 92: {REPORTTIMERTOGO(&SMS);break;}
case 95: {REPORTAL(&SMS);break;} 
case 94: {REPORTSTATE(&SMS); break;} 
case 98: {REPORTOUTPUTS(&SMS);break;}
		 //
case 59: {listOther(); break;}
case 69: {listSol(); break;}
case 79: {listAl(); break;}
case 89: {listPhn(); break;}

case 99: {listReports();break;}
	default: break;	 
//
}

nexe=-1;

}	

void Phone::listOther(){
SMS.append("disTmrs(55),");
SMS.append("enTmrs(56),");
SMS.append("rsetIgn(57),");
SMS.append("rsetIntrpt(58),");
SMS.append("rsetAll(90),");
}

void Phone::listSol(){
SMS.append("rsetSol(60),");
SMS.append("chgSolL(61),");
SMS.append("chgSolV(62),");
SMS.append("SolarOff(66),");
}

void Phone::listAl(){
SMS.append("rsetAl(70),");
SMS.append("arm(71),");
SMS.append("disarm(72),");
SMS.append("plyWrn1(73),");
SMS.append("plyWrn4(74),");
SMS.append("playSrn(77),");
}

void Phone::listPhn(){
SMS.append("resetPhn(80),");
SMS.append("phnOn(83),");
SMS.append("chckStatus(84),");
SMS.append("phnOff(85),");

}



void Phone::listReports(){//130

SMS.append("listOther(59),");
SMS.append("listSol(69),");
SMS.append("listAl(79),");
SMS.append("listPhn(89),");

SMS.append("TmrSince(91),");
SMS.append("TmrTogo(92),");
SMS.append("Voltages(93),");
SMS.append("PhnStatus(94),");
SMS.append("Al(95),");
SMS.append("All(96),");
SMS.append("Inputs(97),");
SMS.append("Outputs(98),");
}

void Phone::stopTimers(void){
	 
		 if(dbg) Serial.print("stop timers ");


	 if(timercheckPhone!=NULL) timercheckPhone->disable();
	 if(timercheckMonitor!=NULL) timercheckMonitor->disable();

	 if(FROM==FROMPHONE) disableMonitor();
	 else disablePhone();

	  Timeout::stopTimers();
 }

 void Phone::startTimers(){
	 
	 if(dbg) Serial.print("start timers ");

	 if(timercheckPhone!=NULL) timercheckPhone->enable();
	 if(timercheckMonitor!=NULL) timercheckMonitor->enable();
	enable();
	 
	 Timeout::startTimers();
 }



byte* Phone::PackBytes(byte* unpackedBytes, int unpackedBytesLength, int &shiftedBytesLength)
        {
bool replaceInvalidChars=false;
char defaultChar=' ';

			int i; 
            byte b, defaultByte = (byte)defaultChar;
			shiftedBytesLength=unpackedBytesLength - unpackedBytesLength / 8;
				byte* shiftedBytes = new byte[shiftedBytesLength];

            int shiftOffset = 0;
            int shiftIndex = 0;

            // Shift the unpacked bytes to the right according to the offset (position of the byte)
           for(i=0; i<unpackedBytesLength; i++)
            {
				b=unpackedBytes[i];
                byte tmpByte = b;

                // Handle invalid characters (bytes out of range)
                if (tmpByte > 127)
                {
                    if (!replaceInvalidChars)
                    {
                        // throw exception and exit the method
						return NULL;
                       // throw new Exception("Invalid character detected: " + tmpByte.ToString("X2"));
                    }
                    else
                    {
                        tmpByte = defaultByte;
                    }
                }

                // Perform the byte shifting
                if (shiftOffset == 7)
                {
                    shiftOffset = 0;
                }
                else
                {
                    shiftedBytes[shiftIndex] = (byte)(tmpByte >> shiftOffset);
                    shiftOffset++;
                    shiftIndex++;
                }
            }

            int moveOffset = 1;
            int moveIndex = 1;
            int packIndex = 0;
            byte* packedBytes = new byte[shiftedBytesLength];

            // Move the bits to the appropriate byte (pack the bits)
           for(i=0; i<unpackedBytesLength; i++)
            {
				b=unpackedBytes[i];

                if (moveOffset == 8)
                {
                    moveOffset = 1;
                }
                else
                {
                    if (moveIndex != unpackedBytesLength)
                    {
                        // Extract the bits to be moved
                        int extractedBitsByte = (unpackedBytes[moveIndex] & _encodeMask[moveOffset - 1]);
                        // Shift the extracted bits to the proper offset
                        extractedBitsByte = (extractedBitsByte << (8 - moveOffset));
                        // Move the bits to the appropriate byte (pack the bits)
                        int movedBitsByte = (extractedBitsByte | shiftedBytes[packIndex]);

                        packedBytes[packIndex] = (byte)movedBitsByte;

                        moveOffset++;
                        packIndex++;
                    }
                    else
                    {
                        packedBytes[packIndex] = shiftedBytes[packIndex];
                    }
                }

                moveIndex++;
            }

		   delete [] shiftedBytes;  shiftedBytes=NULL;
            return packedBytes;
        }


        /// <summary>
        ///  Unpacks a packed 8 bit array to a 7 bit unpacked array according to the GSM
        ///  Protocol.
        /// </summary>
        /// <param name="packedBytes">The byte array that should be unpacked.</param>
        /// <returns>The unpacked bytes array.</returns>



byte* Phone::UnpackBytes(byte* packedBytes, int packedBytesLength, int &shiftedBytesLength)
        {
			int i;
			byte b;

			shiftedBytesLength=(packedBytesLength * 8) / 7;
            byte* shiftedBytes = new byte[shiftedBytesLength];

            int shiftOffset = 0;
            int shiftIndex = 0;

            // Shift the packed bytes to the left according to the offset (position of the byte)
           for(i=0; i<packedBytesLength; i++)
            {
				b=packedBytes[i];
                if (shiftOffset == 7)
                {
                    shiftedBytes[shiftIndex] = 0;
                    shiftOffset = 0;
                    shiftIndex++;
                }

                shiftedBytes[shiftIndex] = (byte)((b << shiftOffset) & 127);

                shiftOffset++;
                shiftIndex++;
            }

            int moveOffset = 0;
            int moveIndex = 0;
            int unpackIndex = 1;
            byte* unpackedBytes = new byte[shiftedBytesLength];

            // 
            if (shiftedBytesLength > 0)
            {
                unpackedBytes[unpackIndex - 1] = shiftedBytes[unpackIndex - 1];
            }

            // Move the bits to the appropriate byte (unpack the bits)
           for(i=0; i<packedBytesLength; i++)
            {
				b=packedBytes[i];
                if (unpackIndex != shiftedBytesLength)
                {
                    if (moveOffset == 7)
                    {
                        moveOffset = 0;
                        unpackIndex++;
                        unpackedBytes[unpackIndex - 1] = shiftedBytes[unpackIndex - 1];
                    }

                    if (unpackIndex != shiftedBytesLength)
                    {
                        // Extract the bits to be moved
                        int extractedBitsByte = (packedBytes[moveIndex] & _decodeMask[moveOffset]);
                        // Shift the extracted bits to the proper offset
                        extractedBitsByte = (extractedBitsByte >> (7 - moveOffset));
                        // Move the bits to the appropriate byte (unpack the bits)
                        int movedBitsByte = (extractedBitsByte | shiftedBytes[unpackIndex]);

                        unpackedBytes[unpackIndex] = (byte)movedBitsByte;

                        moveOffset++;
                        unpackIndex++;
                        moveIndex++;
                    }
                }
            }

		    delete [] shiftedBytes;  shiftedBytes=NULL;

            // Remove the padding if exists
            if (unpackedBytes[shiftedBytesLength - 1] == 0)
            {
                byte* finalResultBytes = new byte[shiftedBytesLength - 1];

			 for(i=0; i<shiftedBytesLength - 1; i++)
				finalResultBytes[i]=unpackedBytes[i];

				 delete [] unpackedBytes;  unpackedBytes=NULL;

                return finalResultBytes;
            }

            return unpackedBytes;
        }


