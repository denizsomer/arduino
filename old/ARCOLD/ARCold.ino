#include <SimpleTimer.h>
SimpleTimer t;

//#include <modbus.h>


//phone variables
const int R520m=1;
const int K700i=2;

const int oriphone =R520m;
int phone=oriphone;
const int  monitor=0;


 bool prnt=true;
 //bool prnt=false;

HardwareSerial* monitorSerial,* phoneSerial;

const  int p_ARPTx   = 0; //Rx0 
const  int p_ARPRx   = 1; //Tx0
const  int p_pirTamp = 2; //interrupt 0
const  int p_pirAl   = 3; //interrupt 1 

const  int r_SrnOff  = 9;
const  int r_SrnOn   = 8;
const  int r_LHScene    = 10; 
const  int r_RHScene    = 11; 

const  int p_phoneRx     = 14; //TX3
const  int p_phoneTx     = 15; //RX3
const  int p_monitorRx   = 16; //TX2
const  int p_monitorTx   = 17; //RX2

//interrupts
const  int p_TotAlSw  = 18; //  interrupt5
const  int p_PerAlSw  = 19; //  interrupt4
const  int p_DoorSw   = 20; //  interrupt3
//21 DOES NOT WORK

const  int r_AlPow  = 22; 
const  int r_Sink   = 24;
//const  int r_sp   = 23;
//const  int r_sp   = 25;


const  int p_LED1       = 33;
const  int p_LED3       = 35;
const  int p_StatusLED  = 37;
const  int p_AlLED      = 45;
const  int p_LED5       = 47;

const  int p_IsoSw   = 51; 
const  int p_EmSw   = 50;
const  int p_HbrkSw  = 53; 
const  int p_TapSw     = 52; 

bool s_TapSw=LOW, s_IsoSw=LOW, s_EmSw=HIGH, s_HbrkSw=LOW;

const unsigned int maxnAls=10, //SrnDuration=10000,
SrnDuration=5000, 
 AlPowDelay=100, interruptDelay=200, durationAledLED=2000,
 alarmArmDelay=7000, doorAlDelay=7000;

int timerReset, timerblinkAlLEDSlow, timerTap=-1,
timerblinkAlLEDMedium,timerblinkAlLEDFast, timerblinkScene,
srnStatus, timercheckMonitor,AlSrc, AlLevel, nAls;


const unsigned long resetInterval=43200000, debounceDelay=20;

volatile int itrptSrc=-1;
 
const bool leftScene=0, rightScene=1;
bool AlArmed, scene;

const int NoAlarm=0;
const int Periphery=1;
const int InsideOut=2;
const int Emergency=3;
int lastAlLevel;

const int BUSY=0;
const int FREE=1;

//1000 1sec
//60000 1min
//3600000 1hr
const unsigned int phnPDUInterval=2000, maxindex=250, 
maxSMS=140, phnResponseDelay=2000;

const unsigned long phnStatusInterval=7201000, phoneDiscardInterval=30000;

const char CIND[9]="+CIND:/0", CMGL[9]="+CMGL:/0", CMGR[9]="+CMGR:/0", myPDU[13]="7727523900/0",
CPMS[9]="+CPMS:/0", RING[7]="RING/0", CMGD[7]="CMGD/0" ,  CKEV[9]="+CKEV:/0";

char SMS[maxSMS], cmd[4], PDU[maxindex], character, charmon;

int nPDU,lengthSMS,PDUStatus, counterSerial,jmyPDU, jmyPDU0,nexe, totPDU,
jCIND,  jCMGL,  jCMGR,  jCMGD,  jCPMS,  jRING, jCKEV,idigit=0,
jCIND0, jCMGL0, jCMGR0, jCMGD0, jCPMS0, jRING0, jCKEV0,lphone,
i,j, index, phnStatus, monitorStatus,timercheckPhone, timercheckStatus;

bool detachAll,isCIND, isCMGD, isCMGL, isCMGR, isCPMS, isRING, isCKEV, ended, ending, started, ismynumber;

unsigned long phoneTime, monitorTime;


void setup()
{
	//new interrupts
/* Pin to interrupt map:67

PCINT 00 01 02 03 = PCIE0 = pcmsk0
D     53 52 51 50 = PCIE0 = pcmsk0
*/
	//new interrupts
MCUCR = (1<<ISC00) | (1<<ISC01);
PCICR |= (1<<PCIE0);
PCMSK0 |= (1<<PCINT0);
PCMSK0 |= (1<<PCINT1);
PCMSK0 |= (1<<PCINT2);
PCMSK0 |= (1<<PCINT3);
Serial.begin(9600); //initialize serial communication
Serial2.begin(9600); //initialize serial communication with monitor
Serial3.begin(9600); //initialize serial communication with phone

monitorSerial=&Serial3;
phoneSerial=&Serial3;

	Serial2.flush();
	Serial3.flush();


initializePins();
resetAll();

}


void initializePins(){
//t.setInterval(10000, sendReport);

//initialize outputs
pinMode(r_SrnOff, OUTPUT); digitalWrite(r_SrnOff, LOW);
pinMode(r_SrnOn, OUTPUT); digitalWrite(r_SrnOn, LOW);
pinMode(r_AlPow, OUTPUT); digitalWrite(r_AlPow, LOW);
pinMode(r_Sink, OUTPUT); digitalWrite(r_Sink, LOW);
pinMode(23, OUTPUT); digitalWrite(23, LOW);
pinMode(25, OUTPUT); digitalWrite(25, LOW);
pinMode(r_LHScene, OUTPUT); digitalWrite(r_LHScene, LOW);
pinMode(r_RHScene, OUTPUT); digitalWrite(r_RHScene, LOW);
pinMode(p_LED1, OUTPUT); digitalWrite(p_LED1, LOW);
pinMode(p_StatusLED, OUTPUT); digitalWrite(p_StatusLED, LOW);
pinMode(p_LED3, OUTPUT); digitalWrite(p_LED3, LOW);
pinMode(p_AlLED, OUTPUT); digitalWrite(p_AlLED, LOW);
pinMode(p_LED5, OUTPUT); digitalWrite(p_LED5, LOW);

//Listening, pull-ups

pinMode(p_pirTamp, INPUT); digitalWrite(p_pirTamp, HIGH); 
pinMode(p_pirAl, INPUT);   digitalWrite(p_pirAl, HIGH); 
pinMode(p_IsoSw, INPUT);   digitalWrite(p_IsoSw, HIGH);  
pinMode(p_EmSw, INPUT);    digitalWrite(p_EmSw, HIGH); 
pinMode(p_TapSw, INPUT);   digitalWrite(p_TapSw, HIGH);  
pinMode(p_TapSw, INPUT);   digitalWrite(p_TapSw, HIGH); 
pinMode(p_HbrkSw, INPUT);  digitalWrite(p_HbrkSw, HIGH); 
pinMode(p_DoorSw, INPUT);  digitalWrite(p_DoorSw, HIGH); 
pinMode(p_TotAlSw, INPUT); digitalWrite(p_TotAlSw, HIGH); 
pinMode(p_PerAlSw, INPUT); digitalWrite(p_PerAlSw, HIGH); 
}


///////////resets///////////////////////////////////////////////////////////////////
void resetAll(){
if(prnt) Serial.println("resetting...........................");
digitalWrite(p_StatusLED,HIGH);

nexe=-1;

resetTimers();
resetAl();
resetScene();
resetSerial();
resetPhn();
attachnonAlInterrupts();
resetInterrupts();
checkStatus();
digitalWrite(p_StatusLED,LOW);
}


void resetTimers(){
t.reset();

//t.setInterval(1000, checkTap);

if(phone>0) timercheckPhone= t.setInterval(phnPDUInterval, prepPhone);
if(monitor>0) timercheckMonitor= t.setInterval(phnPDUInterval+1000, prepMonitor);


timerReset=t.setInterval(resetInterval, resetAll);
timerblinkScene= t.setInterval(500, blinkScene);
timerblinkAlLEDSlow= t.setInterval(5000, blinkAlLED);
timerblinkAlLEDMedium= t.setInterval(1000, blinkAlLED);
timerblinkAlLEDFast= t.setInterval(200, blinkAlLED);

delay(phnPDUInterval+1000);
}


void resetAl(){
disableblinkAlLED();
nAls=0;
AlSrc=-1;
freesrnStatus();
}
void resetScene(){
disableblinkScene();
scene=leftScene	;
}

void resetPhn(){
phone=oriphone;
phoneTime=0;
freephnStatus();
freemonitorStatus();
resetSMS();
resetPDU();
}
void resetSMS(){
lengthSMS=0;
SMS[lengthSMS]='\0';
}

void resetSerial(){

ending=false; ended=false; started=false;

counterSerial=0; 
isCIND=false;isCMGL=false;isCMGR=false; isCMGD=false;isCPMS=false;isRING=false;isCKEV=false; 
jCIND=0;      jCMGL=0;     jCMGR=0;      jCMGD=0;     jCPMS=0;     jRING=0; jCKEV=0;
jCIND0=0;     jCMGL0=0;    jCMGR0=0;     jCMGD0=0;    jCPMS0=0;    jRING0=0; jCKEV0=0;

resetPDU();
}
void resetPDU(){
index=0;
PDU[index]='\0';
}

void resetInterrupts(){
detachAll=false;
AlArmed=false;

//then do the hard interrupts
for(i=2; i<4; i++){
manageInterrupt(i);
delay(200);
}

for(i=18; i<21; i++){
manageInterrupt(i);
delay(200);
}
manageInterrupt(99);

}



///////loop////////////////////////////////////////////////////////////////////////////////
void loop()
{
t.run();

if(!detachAll){
//master.run();
	if(itrptSrc>-1) { manageInterrupt(itrptSrc);}

	if (phone>0) managePhone();

	if (monitor>0) manageMonitor();
}

} //loop







///////managers////////////////////////////////////////////////////////////////////////////////
void manageInterrupt(int iSrc){

if(prnt) {Serial.print("Interrupt: "); Serial.print(iSrc); Serial.println(" ...");}
	
switch(iSrc){

			case p_pirTamp:
				if(debounce(p_pirTamp)==LOW){
if(prnt) Serial.println("p_pirTamp");

						if(AlArmed) {
							AlSrc=itrptSrc; 
							playSrn();
						}}
				break;

			case p_pirAl:
				if(debounce(p_pirAl)  ==LOW){
if(prnt) Serial.println("p_pirAl");

						if(AlArmed) {AlSrc=itrptSrc;
				playSrn();
						}}
			break;

			case p_DoorSw:
				if(debounce(p_DoorSw) ==LOW){
if(prnt) Serial.println("p_DoorSw");

						if(AlArmed){AlSrc=itrptSrc; 
				t.setTimeout(doorAlDelay, playSrn);
						} }
			break;


			case p_TotAlSw:
			case p_PerAlSw:
				if (debounce(p_PerAlSw)==LOW) 
				{
					AlLevel=Periphery; 
						if(!AlArmed) {
						t.setTimeout(alarmArmDelay, manageAlarm); 
						enableblinkAlLEDFast();
						}
				if(prnt) Serial.println("p_PerAlSw enable");
				}
				else if (debounce(p_TotAlSw)==LOW) 
				{
					AlLevel=InsideOut;
						if(!AlArmed) {
						t.setTimeout(alarmArmDelay, manageAlarm); 
						enableblinkAlLEDFast();
						}
					if(prnt) Serial.println("p_TotAlSw enable");
				}
				else if(AlLevel!=Emergency){AlLevel=NoAlarm; manageAlarm();  if(prnt) Serial.println("AlSw disable");}


			break;


			default: //check the additional suspects

			if(s_EmSw!=debounce(p_EmSw))
			{
				s_EmSw=debounce(p_EmSw);
				if(s_EmSw==LOW)
				{
				}
				else
				{
					if(prnt) Serial.println("EmergencyON");
					EmergencyON();

				}
			}

			if(s_IsoSw!=debounce(p_IsoSw))
			{
				s_IsoSw=debounce(p_IsoSw);
				if(s_IsoSw==LOW){
					if(prnt) Serial.println("IsoSwClosed");
					if(AlArmed)
					{
					AlSrc=itrptSrc;
						playSrn();
					}
				}
				else{
					if(prnt) Serial.println("IsoSwOpened");
				}
			} 

			if(s_HbrkSw!=debounce(p_HbrkSw))
			{
				s_HbrkSw=debounce(p_HbrkSw);
				if(s_HbrkSw==LOW)
				{
					if(prnt) Serial.println("HBrkReleased");
					if(AlArmed){
					AlSrc=itrptSrc; 
					playSrn();
					}
				}
				else
				{
					if(prnt) Serial.println("HBrkPulled");
				}
			}
								
			if(s_TapSw!=debounce(p_TapSw))
			{
				s_TapSw=debounce(p_TapSw);
				if(s_TapSw==LOW) TapOn();
				else	TapOff();
				
			}


	

			break;

}





	itrptSrc=-1;


}


void managePhone(){
	//if(phoneSerial->available()&&!ended) busyphnStatus();

	while(phoneSerial->available()&&!ended)
	{

		//if(prnt) {Serial.print(started);Serial.println(ended);}

character = phoneSerial->read();
if(prnt) Serial.print(character);
counterSerial++;


	if(!started) 
	{

     if(jCIND>5)  {resetSerial(); isCIND=true;  started=true;}
else if(jCMGL>5)  {resetSerial(); isCMGL=true;  started=true;} 
else if(jCPMS>5)  {resetSerial(); isCPMS=true;  started=true;}
else if(jCMGR>5)  {resetSerial(); isCMGR=true;  started=true;}
else if(jRING>3)  {resetSerial(); isRING=true; ended=true; break;}
else if(jCMGD>3)  {resetSerial(); isCMGD=true; ended=true; break;}
else{
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

if(character==CMGD[jCMGD]){
	if(jCMGD==0) {jCMGD0=counterSerial; jCMGD++;} 
	else	{
		if(jCMGD0+jCMGD==counterSerial) jCMGD++;
		else jCMGD=0;
	}
}
else jCMGD=0;

if(character==RING[jRING]){
	if(jRING==0) {jRING0=counterSerial; jRING++;}
	else	{
		if(jRING0+jRING==counterSerial) jRING++;
		else jRING=0;
	}
}
else jRING=0;
}
} //if !started
		else //started
	{
			
		PDU[index] = character;
		
		if(isCMGL&&index==3&&PDU[1]==',')
		{
			nPDU=charToInt(PDU[0]); 
			PDUStatus=charToInt(PDU[2]);

		}
		else if(isCPMS&&PDU[1]==',')
		{ 
			totPDU=charToInt(PDU[0]);
		}
		PDU[++index] = '\0';
		
		if((PDU[index-1]=='K'&&PDU[index-2]=='O')||index == maxindex-1)
		{
			ended=true; break;
		}
		

		
	}

		//timeout
	if(phoneTime!=0&&(millis()-phoneTime>phoneDiscardInterval))
	{
	// if(prnt) Serial.print(starting);   if(prnt) Serial.print(started);   if(prnt) Serial.print(ending);   if(prnt) Serial.print(ended);
	//if(prnt) Serial.println(millis()-phoneTime);
		
	for( int k=0; k<index; k++)     if(prnt) Serial.print(PDU[k]);

	if(prnt) Serial.println("too long wait phone");

	resetSerial();
	freephnStatus();
	enablePhoneTimers();
	}

} //SERIAL AVAILABLE

if(ended)
{
for( i=0; i<index; i++) if(prnt) Serial.print(PDU[i]);
if(prnt) Serial.println("s&&e");

if(isRING) {
	if(prnt) Serial.print("isRING");
	delay(100);
	answerCall();}
//{ 
//
//t.setTimeout(60000, phnHangup);
//answerCall();
//}
else if(isCMGD){ 
if(prnt) Serial.println("isCMGD");
t.setTimeout(1000, enablePhoneTimers);
}
else if(isCPMS) 
{
	if( totPDU!=0) { 
		disablePhoneTimers(); t.setTimeout(phnResponseDelay, listSMS); 
		if(prnt) Serial.print(totPDU); 
		if(prnt) Serial.println(" messages to process");}
	//else enablePhoneTimers();
	else{ 
		if(prnt) {Serial.print(nexe); Serial.println("totPDU=0");}
		freephnStatus();
		if(nexe>-1) execCommand();
		else enablePhoneTimers();
	}
}
else if(isCMGL) 
{
	if(prnt) Serial.print("nPDU: "); if(prnt) Serial.print(nPDU); if(prnt) Serial.print("status: "); if(prnt) Serial.println(PDUStatus);


	if(PDUStatus==0||PDUStatus==1) 
	{if(prnt) Serial.print("Message ");if(prnt) Serial.print(nPDU);if(prnt) Serial.println(" is received");
	t.setTimeout(phnResponseDelay, readSMS);
	}
	else {
	if(prnt) Serial.print("Message ");if(prnt) Serial.print(nPDU);if(prnt) Serial.println(" is not received"); 
	t.setTimeout(phnResponseDelay, deleteSMS);
	}
}
else if(isCMGR) {
		if(prnt) Serial.print("Message ");if(prnt) Serial.print(nPDU);if(prnt) Serial.println(" received, checking source");

		
jmyPDU=0;
ismynumber=false;

i=0;
while(i<index)
{
  if(PDU[i]==myPDU[jmyPDU])
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
  
  if(jmyPDU==9) {ismynumber=true; break;}
  
  i++;
}

		if(ismynumber)
		{

			while( PDU[i]!='\n'&&i<index) {
			//  if(prnt) Serial.print(i);if(prnt) Serial.print(PDU[i]);if(prnt) Serial.println(index); 
			i++; }
			for( j=i-5; j<i-1; j++) cmd[j-(i-5)]=PDU[j]; 
			nexe=decodeCommand(cmd);
			
			if(prnt) Serial.print("Message ");if(prnt) Serial.print(nPDU);
			if(prnt) Serial.print(" is from me : "); if(prnt) Serial.println(nexe);
 
		}
		else
		{
			//not from me
			if(prnt) Serial.print("Message ");
			if(prnt) Serial.print(nPDU);
			if(prnt) Serial.println("is not from me");
					}
		t.setTimeout(phnResponseDelay, deleteSMS);

}
else   if(isCIND)
{
  	if(prnt) Serial.println("isCIND");
}

freephnStatus();
resetSerial();
}



}


void manageMonitor(){
	while(monitorSerial->available()&&!ended)
{

character = monitorSerial->read();
 if(prnt) Serial.print(character);
counterSerial++;


	if(!started) 
	{

     if(jCKEV>5)  {resetSerial(); isCKEV=true;  started=true;}
else{
	if(character==CKEV[jCKEV]){
	if(jCKEV==0) {jCKEV0=counterSerial; jCKEV++;} 
	else	{
		if(jCKEV0+jCKEV==counterSerial) jCKEV++;
		else jCKEV=0;
	}
}
else jCKEV=0;
}
	} //if !started
		else //started
	{
		PDU[index] = character;

		PDU[++index] = '\0';
		
		if((PDU[index-1]=='\n')||index == maxindex-1)
		{
			ended=true; break;
		}

		
	}

} //SERIAL AVAILABLE

if(ended)
{
	if(prnt){
//for( i=0; i<index; i++)  Serial.print(PDU[i]);
//Serial.println("monitors&&e");
	}

	if(isCKEV&&PDU[2]=='0'){
	 if(PDU[0]=='#')
	 {
		 
		 disableMonitorTimer();
		 disablePhoneTimers();
		 phone=-1;
			
			digitalWrite(p_StatusLED,HIGH);
	
				if(nexe>1 ) 
				{ 
					if(prnt) monitorSerial->println(nexe);
					execCommand();
				}

			idigit=0;
			resetSerial();
		}
	 else if(PDU[0]=='*')
	 {
		 nexe=-1;
		 digitalWrite(p_StatusLED,LOW);
		resetSerial();

		enableMonitorTimer();
		phone=oriphone;
		freephnStatus();
		enablePhoneTimers();
		
	}
		else 
		{
			idigit++;
			if(idigit==1) nexe=10*charToInt(PDU[0]);
			else if(idigit==2) nexe+=charToInt(PDU[0]);
		}
	}

	resetSerial();
}


//timeout
if(monitorTime!=0&&(millis()-monitorTime>phoneDiscardInterval))
{
	
for( int k=0; k<index; k++)     if(prnt) Serial.print(PDU[k]);

if(prnt) Serial.println("too long wait monitor");

resetSerial();

enableMonitorTimer();
phone=oriphone;
freephnStatus();
enablePhoneTimers();
}
}


//Interrupt///////////////////////////////////////////////////////////////////////////////////
boolean debounce(int pin)
{
boolean state;
boolean previousState;
previousState = digitalRead(pin); // store switch state
for(int counter=0; counter < debounceDelay; counter++)
{
delay(1); // wait for 1 millisecond
state = digitalRead(pin); // read the pin
if( state != previousState)
{
counter = 0; // reset the counter if the state changes
previousState = state; // and save the current state
}
}
// here when the switch state has been stable longer than the debounce period
return state;
}




void pirTampInterrupt(){itrptSrc=p_pirTamp;}
void pirAlInterrupt(){itrptSrc=p_pirAl;}
void TotAlInterrupt(){itrptSrc=p_TotAlSw;}
void PerAlInterrupt(){itrptSrc=p_PerAlSw;}
void DoorSwInterrupt(){itrptSrc=p_DoorSw;}
ISR(PCINT0_vect) {itrptSrc=99;}

void attachAlInterrupts(){
detachAll=false;
	if(prnt) Serial.println("attachAlInterrupts");
	if(AlLevel==InsideOut){
attachInterrupt(1, pirAlInterrupt,   CHANGE); 
	}

attachInterrupt(0, pirTampInterrupt, CHANGE); 
attachInterrupt(3, DoorSwInterrupt,   CHANGE); 
}

void detachAlInterrupts(){

	//if(prnt) Serial.println("detachAlInterrupts");
detachInterrupt(0); //p_pirAl 
detachInterrupt(1); //p_pirTamp
detachInterrupt(3); //p_Door
}



void attachnonAlInterrupts(){
	if(prnt)  Serial.println("attachnonAlInterrupts");
attachInterrupt(4, PerAlInterrupt,   CHANGE); 
attachInterrupt(5, TotAlInterrupt,   CHANGE); 
}

void detachnonAlInterrupts(){

	//if(prnt) {Serial.println();  Serial.println("detachnonAlInterrupts");}
detachInterrupt(4); 
detachInterrupt(5); 
}



//Alarm///////////////////////////////////////////////////////////////////////////////
void manageAlarm(){
if(AlLevel==NoAlarm) disarm();
else arm();
}
void arm(){
	if(prnt) {Serial.print("Alarm Armed!");Serial.println(AlLevel);}
	AlArmed=true;
	resetAl();
	digitalWrite(r_AlPow, LOW);
	enableblinkAlLEDSlow();
	attachAlInterrupts();
	}
void disarm(){
	detachAlInterrupts();
	srnOff() ;
	AlArmed=false; 
	digitalWrite(r_AlPow, HIGH);
	if(prnt) Serial.println("Alarm Disarmed");
	disableblinkAlLED();
	}

void enableblinkAlLEDFast(){
	t.disable(timerblinkAlLEDSlow);
	if(AlLevel==InsideOut) t.enable(timerblinkAlLEDFast);
	else if(AlLevel==Periphery) t.enable(timerblinkAlLEDMedium);
}
void enableblinkAlLEDSlow(){
	t.disable(timerblinkAlLEDFast);
	t.disable(timerblinkAlLEDMedium);
	t.enable(timerblinkAlLEDSlow);
}
void disableblinkAlLED(){
	t.disable(timerblinkAlLEDFast);
	t.disable(timerblinkAlLEDMedium);
	t.disable(timerblinkAlLEDSlow);
	digitalWrite(p_AlLED,LOW);
}
void playSrn() {

if((!AlArmed||srnStatus==BUSY)) return;
nAls++;

if(AlArmed!=Emergency&&nAls>=maxnAls) {if(prnt) Serial.print("nAls>=maxnAls,disarming"); disarm(); return;}

	detachAll=true;

srnStatus=BUSY;
srnOn() ; 
enableblinkScene();


t.setTimeout(2*SrnDuration, srnOn);
t.setTimeout(3*SrnDuration, srnOff);
t.setTimeout(3*SrnDuration+1000, freesrnStatus);



//enableblinkAlLEDFast();
if(prnt) {Serial.print("Pin");Serial.print(AlSrc);}
nexe=93;

}




void playWrn(int WrnDuration) {
srnOn();
t.setTimeout(WrnDuration, srnOff);
}

void EmergencyON(){
	detachAll=true;
					lastAlLevel=AlLevel;
                       AlLevel=Emergency;
					   AlArmed=true;
						digitalWrite(r_AlPow, LOW);
						t.setTimeout(4*SrnDuration, EmergencyOFF);
					playSrn();


}

void EmergencyOFF(){
	detachAll=false;
	AlLevel=lastAlLevel;
manageAlarm();


}
void freesrnStatus(){
srnStatus=FREE;
AlSrc=-1;
disableblinkScene();
srnOff();
detachAll=false;
}


void srnOn(){
	if(prnt) Serial.println("SrnOn");
digitalWrite(r_SrnOn, HIGH);
t.setTimeout(500, srnOnL);
}

void srnOnL(){
digitalWrite(r_SrnOn, LOW);
}

void srnOff(){
	if(prnt) Serial.println("SrnOff");
digitalWrite(r_SrnOff, HIGH);
t.setTimeout(500, srnOffL);
}

void srnOffL(){
digitalWrite(r_SrnOff, LOW);
}




void blinkAlLED(){
	digitalWrite(p_AlLED,HIGH);
	t.setTimeout(30, AlLEDL);
}

void AlLEDL(){
digitalWrite(p_AlLED,LOW);
}
void AlLEDH(){
digitalWrite(p_AlLED,HIGH);
}

/*void printDouble( double val, double precision){

int digits=int(val);

unsigned int frac;

frac = int((val - double(digits)) * abs(precision));


if(prnt) Serial.print(digits);
if(prnt) Serial.print('.');
if(frac<10) if(prnt) Serial.print("0");
if(prnt) Serial.print(frac);      
}*/



//Solar////////////////////////////////////////////////////////////////////////////////////
void blinkScene(){
	if(!AlArmed) return;

	if(scene==leftScene) RHSceneOn();
	else LHSceneOn();
	scene=!scene;
}
void enableblinkScene(){t.enable(timerblinkScene);}
void disableblinkScene(){t.disable(timerblinkScene);

	digitalWrite(r_RHScene, LOW);
	digitalWrite(r_LHScene, LOW);
}

void LHSceneOn(){
	if(prnt) Serial.println("LHSceneOn()"); 
	digitalWrite(r_RHScene, LOW);
	digitalWrite(r_LHScene, HIGH);
}
void RHSceneOn(){
	if(prnt) Serial.println("RHSceneOn()");
	digitalWrite(r_LHScene, LOW);
	digitalWrite(r_RHScene, HIGH);
}


////////////Timers/////////////////////////////////////////////////////////
void disablePhoneTimers(){
	
	if(timercheckPhone>0) t.disable(timercheckPhone);
		if(prnt) Serial.println("Phone timer disabled");
}
void enablePhoneTimers(){
	
	if(timercheckPhone>0) t.enable(timercheckPhone);
	if(prnt) Serial.println("Phone timer enabled");
}

void checkStatus() {

if(phnStatus==BUSY) return;
busyphnStatus();

delay(100);
phoneSerial->println("AT+CIND?");
if(prnt) Serial.println("checking Status");

}


void prepPhone() {

if(phnStatus==BUSY) return;
busyphnStatus();

phoneSerial->println("AT+CPMS=\"ME\"");

if(prnt) Serial.println("preparing Phone");

}


void disableMonitorTimer(){
	if(timercheckMonitor>0) t.disable(timercheckMonitor);
}
void enableMonitorTimer(){
	if(timercheckMonitor>0) t.enable(timercheckMonitor);
}
void prepMonitor() {

	if(monitor<1) return;


initializeMonitor();

if(prnt) Serial.println("preparing Monitor");
}





//Monitor///////////////////////////////////////////////////////////////////////////////////
void omanageMonitor(){
	while(monitorSerial->available()&&!ended)
{

character = monitorSerial->read();
if(prnt) Serial.print(character);
counterSerial++;


	if(!started) 
	{

     if(jCKEV>5)  {resetSerial(); isCKEV=true;  started=true;}
else{
if(character==CKEV[jCKEV]){
	if(jCKEV==0) {jCKEV0=counterSerial; jCKEV++;} 
	else	{
		if(jCKEV0+jCKEV==counterSerial) jCKEV++;
		else jCKEV=0;
	}
}
else jCKEV=0;

}
	} //if !started
		else //started
	{
		PDU[index] = character;

		PDU[++index] = '\0';
		
		if((PDU[index-1]=='\n')||index == maxindex-1)
		{
			ended=true; break;
		}

		
	}

} //SERIAL AVAILABLE

if(started&&ended)
{

		     if(PDU[0]=='*'||PDU[0]=='#')
		{
				if(nexe>0 ) 
				{ 
					if(prnt) Serial.println(nexe);
					execCommand();
				}
			nexe=-1;
			idigit=0;
			resetSerial();
		}
		else if(PDU[2]=='0')
		{
			idigit++;
			if(idigit==1) nexe=10*charToInt(PDU[0]);
			else if(idigit==2) nexe+=charToInt(PDU[0]);
		}


	resetSerial();
}


//timeout
}



void initializeMonitor() {monitorSerial->println("AT+CMER=3,2,0,0,0");}

void listMonitor() {

monitorSerial->println("AT+CMGL=4");
}
void clearMonitor() {

monitorSerial->print("AT+CMGD=");
monitorSerial->print(nPDU);
monitorSerial->println(",0"); 

resetSerial();

}



void monitorSMS() {

	if(prnt) Serial.println("monitoring SMS");


	delay(100);
	int i=0;
	while(i<lengthSMS){
monitorSerial->print(SMS[i]);
if(prnt) Serial.print(SMS[i]);
i++;
delay(10);
	}
	monitorSerial->println();
if(prnt) Serial.println();
}



void freemonitorStatus(){
	monitorTime=0;
monitorStatus=FREE;
}
void busymonitorStatus(){
	monitorTime=millis();
monitorStatus=BUSY;
}


//Phone///////////////////////////////////////////////////////////////////////////////////

void freephnStatus(){
	phoneTime=0;
phnStatus=FREE;
if(prnt) Serial.println("phone FREE");
}
void busyphnStatus(){
phoneTime=millis();
phnStatus=BUSY;

if(prnt) Serial.println("phone BUSY");
}





//////////////////////////reports//////////////////////////////////////////////////////




void TapOn(){digitalWrite(r_Sink, HIGH);
		if(prnt) Serial.println("Tap ON");

timerTap=t.setTimeout(10000, TapOff);
}
void TapOff(){
		if(timerTap!=-1)
{ t.deleteTimer(timerTap); timerTap=-1;}
			if(prnt) Serial.println("Tap OFF");

		digitalWrite(r_Sink, LOW);}

int charToInt(char c){

if( c =='1') return 1;
else if( c =='2') return 2;
else if( c =='3') return 3;
else if( c =='4') return 4;
else if( c =='5') return 5;
else if( c =='6') return 6;
else if( c =='7') return 7;
else if( c =='8') return 8;
else if( c =='9') return 9;
else if( c =='A') return 10;
else if( c =='a') return 10;
else if( c =='B') return 11;
else if( c =='b') return 11;
else if( c =='C') return 12;
else if( c =='c') return 12;
else if( c =='D') return 13;
else if( c =='d') return 13;
else if( c =='E') return 14;
else if( c =='e') return 14;
else if( c =='F') return 15;
else if( c =='f') return 15;
else		return 0;


}

char intToChar(int c){

if( c ==0) return '0';
else if( c ==1) return '1';
else if( c ==2) return '2';
else if( c ==3) return '3';
else if( c ==4) return '4';
else if( c ==5) return '5';
else if( c ==6) return '6';
else if( c ==7) return '7';
else if( c ==8) return '8';
else if( c ==9) return '9';
else		return '!';


}


void  listSMS() {phoneSerial->println("AT+CMGL=4");}
void readSMS() {phoneSerial->print("AT+CMGR="); phoneSerial->println(nPDU);}
void answerCall() {phoneSerial->print("ATA");delay(10); phoneSerial->println();}
void deleteSMS() {phoneSerial->print("AT+CMGD="); phoneSerial->print(nPDU); phoneSerial->println(",0"); }
int decodeCommand(char* cmd)		
{

int num=charToInt(cmd[1]);

if (num>-1) num*=10;
else	return -1;


if( cmd[0] =='3')	num+=0;
else if( cmd[0] =='B')	num+=1;
else return-2;

if( cmd[3] =='8')		num+=0;
else if( cmd[3] =='9')		num+=2;
else if( cmd[3] =='A')		num+=4;
else if( cmd[3] =='B')		num+=6;
else if( cmd[3] =='C')		num+=8;
else		return-3;

if(num<5||num==14||num==15||(num>17&&num<22)) {if(prnt) Serial.println("protected pin"); return -num;}

return num;
}	



void execCommand()		
{
  
  
//	char state[5];
//	if(prnt) Serial.print("Executing ");if(prnt) Serial.println(nexe);
//
//// toggle digital pins
//if(nexe>-1&&nexe<54) { 
//digitalWrite(nexe,!digitalRead(nexe)); 
//if(prnt) Serial.print("Pin ");if(prnt) Serial.print(nexe); if(prnt) Serial.print(" is ");  if(prnt) Serial.println(digitalRead(nexe));
//
//
//	startSMS();
//
//	appendtoSMS("Pin ",4);
//	itoa(nexe,state,10); appendtoSMS(state,2);
//	appendtoSMS(" is ",4);
//	itoa(digitalRead(nexe),state,10); appendtoSMS(state,1);
//
//	appendtoSMS("\"",1);
//
//	if (phone>0)    sendSMS();
//else if (monitor>0) monitorSMS();
//	
//
//} 
//else switch(nexe){
//case 55: disablePhoneTimers(); break;
//case 56: enablePhoneTimers(); break;
//case 57: resetIgn(); break;
//case 58: resetInterrupts(); break;
//
////case 59: list
////
//case 60: SolarOn(); break;
//case 61: chargeSolL(); break;
//case 62: chargeSolV(); break;
//
//case 66: SolarOff(); break;
////case 69: list
////
////
//case 70: resetAl(); break;
//case 71: arm(); break;
//case 72: disarm(); break;
//case 73: playWrn(1000); break;
//case 74: playWrn(4000); break;
//case 77: playSrn(); break;
////case 79: list
////
////
//case 80: resetPhn(); break;
//case 81: takePicture(); break;
//case 82: takeandsendPicture(); break;
//case 84: checkStatus(); break;
//
//
//case 90: resetAll(); break;
////case 89: list
////
//default: 
//	startSMS();
//	writeReport(nexe);
//	appendtoSMS("\"",1);
//
//	if (phone>0)    sendSMS();
//else if (monitor>0) monitorSMS();
//
//	break;
////
//  }
//
//nexe=-1;

//t.setTimeout(phnResponseDelay, clearPhone);
}	

void startSMS() {
resetSMS();
appendtoSMS("AT*EAPP=0,0,\"",13);
return;
}

void appendtoSMS(const char *partSMS, int len){
//len=	strlen(partSMS);
if(lengthSMS+len>=maxSMS) return;
if(lengthSMS>0)lengthSMS--;
strcpy(&SMS[lengthSMS], partSMS);
lengthSMS+=len;
SMS[lengthSMS++]='\0';
}





void sendSMS() {

//	if(phnStatus==BUSY) return;
//	busyphnStatus();
//
//	if(prnt) Serial.println("Sending SMS");
//	disablePhoneTimers();
//	t.setTimeout(30000,enablePhoneTimers);
//
//if(phone==R520m){
//t.setTimeout(100, phoneWake);
//t.setTimeout(3000,phonepurgeSMS);
//t.setTimeout(8000,phoneYes);
//t.setTimeout(11000,phoneEnterMyNumber);
//t.setTimeout(15000,phoneYes);
//}
//else if(phone==K700i){
//	
//t.setTimeout(100, phoneWake);
//t.setTimeout(3000,phonepurgeSMS);
//t.setTimeout(8000,phoneYes);
//t.setTimeout(11000,phoneYes);
//t.setTimeout(14000,phoneYes);
//t.setTimeout(17000,phoneYes);
//t.setTimeout(20000,phoneYes);
//}

}




