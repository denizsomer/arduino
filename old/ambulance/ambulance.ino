#include <SimpleTimer.h>
#include <MyPrint.h>
SimpleTimer t;
MyPrint myprint;

//phone variables
const int R520m=1;
const int K700i=2;

const int oriphone =R520m;
int phone=oriphone;
const int  monitor=1;

 //bool prnt=true;
 bool prnt=false;

HardwareSerial* monitorSerial,* phoneSerial;

//voltages
const  int pv_SolPan   = A0; //blue
const  int pv_BattV     = A1; //orange
//const  int pv_spare      = A2; //yellow
const  int pv_BattL     = A3; //green
//const  int pv_spare2     = A4; //yellow

//A1x->A15x

//Block 1:
//const  int p_phone2Tx   = 0; //Rx0 B3  -> UTB3
//const  int p_phone2Rx   = 1; //Tx0 B8  -> //UTB8

//interrupts
const  int p_pirTamp    = 2; //interrupt 0 G2/G7->Volt. Reducer
const  int p_pirAl     = 3; //interrupt 1 G2/G7->Volt. Reducer

//uln2003-2
const  int r_Phone      = 5; //ULN2-3
const  int r_Sink       = 6; //ULN2-2
const  int r_Cont  = 7; //ULN2-1

//Block 2:
//uln2003-3
const  int r_SolL     = 8; //ULN3-4
const  int r_SolV     = 9; //ULN3-3
//rel5v
const  int r_SrnOn   = 13;
const  int r_SrnOff  = 12;
const  int r_DH11      = 11;
//const  int r_spare5v = 13;

// Block 3:
//const  int p_phoneRx     = 14; //G11 ->ROG1
//const  int p_phoneTx     = 15; //G10 ->ROG5
//const  int p_ustapEcho   = 16; //G4 ->ROG2
const  int p_Tap= 17; //G2 ->ROG2
//interrupts
const  int p_IsoSw     = 18; //  interrupt5 white from ROV5
const  int p_Ign     = 19; //  interrupt4 blue A6->Volt. Reducer
const  int p_HbrkSw    = 20; //  interrupt3 purple from ROV5
const  int p_DoorSw  = 21; //  interrupt2 green from RUC5


//Empty
// 22x->41x

//Block 5:
//uln2003-1
const  int r_AlPow = 38; //ULN1-1
//const  int r_Spare    = 39; //ULN1-2
//const  int p_LED5       = 40;
//const  int r_Spare    = 41; //ULN1-3
//const  int p_LED4       = 42;
//const  int p_usrTrigger = 43; // G8->ROG3
const  int p_StatusLED       = 46;
//const  int p_usrEcho    = 45; //G6->ROG7
const  int p_ContLED       = 44; //48;
const  int p_AlLED    = 48;
//49x, spare at RUC

const  int p_ContSw    = 50; //grey from ROV1
const  int p_PerAlSw        = 51;
const  int p_SolSw  = 52; // B2->SW3->A2->GND
const  int p_TotAlSw  = 53; 


boolean s_ContSw,  s_SolSw, s_TotAlSw, s_PerAlSw, s_Ign, s_IsoSw;

//voltage multipliers      SP         VB          LB
//                         12.61       12.73       12.56
const double           f0=0.024771, f1=0.01506, f3=0.01527,
 f2=0.01580,f4=0.01445, v_SolPanMin=4.0, v_CRI=13.5, v_MAX=14.5, v_MIN=11.5;

const unsigned int maxnAls=4, SrnDuration=10000, //SrnDuration=100, 
 AlPowDelay=100, SolDelay=5000, interruptDelay=200, durationAledLED=2000,
 alarmArmDelay=7000, doorAlDelay=7000;

unsigned int nAls;
int timerReset, timerblinkAlLEDSlow, 
timerContactor=-1,intSrc[6],
timerblinkAlLEDMedium,timerblinkAlLEDFast, 
srnStatus, timerblinkContLEDSlow, timerblinkContLEDFast, timercheckMonitor,AlSrc, AlLevel;

const char Source[9][6]={"pirTp","pirAl","Door ","Hbrk ","IgnSw","IsoSw","AlmSw","SolSw" ,"ConSw"  };

const unsigned long IgnDelay=300000,resetInterval=43200000, debounceDelay=10,
					readVoltageInterval=3600000;

volatile int itrptSrc=-1;
 
unsigned long IgnTime,sinceIgn;

double v_SolPan , v_BattL  , v_BattV , v_SolCont;

bool AlArmed, AlAllowed, SolAllowed, ContactorCLOSED;

const int PeriArmed=1;
const int FullArmed=2;

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

int whichBattIsCharged, nPDU,lengthSMS,PDUStatus, counterSerial,jmyPDU, jmyPDU0,nexe, totPDU,
jCIND,  jCMGL,  jCMGR,  jCMGD,  jCPMS,  jRING, jCKEV,idigit=0,
jCIND0, jCMGL0, jCMGR0, jCMGD0, jCPMS0, jRING0, jCKEV0,lphone,
i,j, index, phnStatus, monitorStatus,timercheckPhone, timercheckStatus;

bool isCIND, isCMGD, isCMGL, isCMGR, isCPMS, isRING, isCKEV, ended, ending, started, ismynumber;

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
//Serial2.begin(9600); //initialize serial communication
Serial3.begin(9600); //initialize serial communication

monitorSerial=&Serial;
if(monitor>0) prnt=false;
phoneSerial=&Serial3;


initializePins();
resetAll();

//myprint.dprint("efc");
/*myprint.dprint(3);
myprint.dprint(5.2);
myprint.setphone();
myprint.dprint(3);*/

}


void initializePins(){
//t.setInterval(20000, takeandsendPicture);
//t.setInterval(10000, sendReport);


//initialize outputs
//uln2003-1
pinMode(r_AlPow, OUTPUT); digitalWrite(r_AlPow, LOW);
//uln2003-2
pinMode(r_Phone, OUTPUT); digitalWrite(r_Phone, LOW);
pinMode(r_Sink, OUTPUT); digitalWrite(r_Sink, LOW);
pinMode(r_Cont, OUTPUT); digitalWrite(r_Cont, LOW);
//uln2003-3
pinMode(r_SolL, OUTPUT); digitalWrite(r_SolL, LOW);
pinMode(r_SolV, OUTPUT); digitalWrite(r_SolV, LOW);
//5V relay
for (i = 10; i < 14; i++) {pinMode(i, OUTPUT); digitalWrite(i, HIGH);}
pinMode(p_StatusLED, OUTPUT); digitalWrite(p_StatusLED, LOW);
pinMode(p_ContLED, OUTPUT); digitalWrite(p_ContLED, LOW);
pinMode(p_AlLED, OUTPUT); digitalWrite(p_AlLED, LOW);

//G:
/*pinMode(p_ustapEcho, INPUT);
pinMode(p_usrearEcho, INPUT);
pinMode(p_ustapTrigger, OUTPUT);
pinMode(p_usrearTrigger, OUTPUT);*/

//Listening, pull-ups

//12V INPUTS, FIRST TO VOLTGE REDUCER THEN TO ULN2003 FOR NOT GATE

pinMode(p_pirTamp, INPUT); digitalWrite(p_pirTamp, HIGH); //2
pinMode(p_pirAl, INPUT);   digitalWrite(p_pirAl, HIGH); //3

pinMode(p_Tap, INPUT);   digitalWrite(p_Tap, HIGH);  //18

pinMode(p_IsoSw, INPUT);   digitalWrite(p_IsoSw, HIGH);  //18
pinMode(p_Ign, INPUT);     digitalWrite(p_Ign, HIGH); //19
pinMode(p_HbrkSw, INPUT);  digitalWrite(p_HbrkSw, HIGH); //20
pinMode(p_DoorSw, INPUT);  digitalWrite(p_DoorSw, HIGH); //21

pinMode(p_ContSw, INPUT);  digitalWrite(p_ContSw, HIGH);//50
pinMode(p_SolSw, INPUT);   digitalWrite(p_SolSw, HIGH); //52

pinMode(p_TotAlSw, INPUT); digitalWrite(p_TotAlSw, HIGH); //51 
pinMode(p_PerAlSw, INPUT); digitalWrite(p_PerAlSw, HIGH); //53
}


///////////resets///////////////////////////////////////////////////////////////////
void resetAll(){
if(prnt) Serial.println("resetting...........................");
digitalWrite(p_StatusLED,HIGH);

nexe=-1;

resetTimers();
resetAl();
resetIgn();
resetSerial();
resetPhn();
attachnonAlInterrupts();
resetInterrupts();
checkStatus();
digitalWrite(p_StatusLED,LOW);
}


void resetTimers(){
t.reset();

t.setInterval(readVoltageInterval, readwriteVoltages);
t.setInterval(1000, checkTap);

if(phone>0) timercheckPhone= t.setInterval(phnPDUInterval, prepPhone);
if(monitor>0) timercheckMonitor= t.setInterval(phnPDUInterval+1000, prepMonitor);

timerReset=t.setInterval(resetInterval, resetAll);
timerblinkContLEDSlow= t.setInterval(5000, blinkContLED);
timerblinkContLEDFast= t.setInterval(200, blinkContLED);
timerblinkAlLEDSlow= t.setInterval(5000, blinkAlLED);
timerblinkAlLEDMedium= t.setInterval(1000, blinkAlLED);
timerblinkAlLEDFast= t.setInterval(200, blinkAlLED);

delay(phnPDUInterval+1000);
}
void resetIgn(){
IgnTime=0; 
sinceIgn=0;
}

void resetAl(){
	
disableblinkAlLED();
AlAllowed=true;
nAls=0;
AlSrc=-1;
freesrnStatus();
}
void resetCont(){
disableblinkContLED();
ContactorCLOSED=false;
}
void resetSolar() {
SolAllowed=true;
}



void resetPhn(){
phone=oriphone;
phoneTime=0;
freephnStatus();
freemonitorStatus();
resetSMS();
resetPDU();
initializePhone();
initializeMonitor();
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

AlArmed=false;
	//manually read these
s_TotAlSw=digitalRead(p_TotAlSw);
s_PerAlSw=digitalRead(p_PerAlSw);
s_SolSw=digitalRead(p_SolSw);
s_ContSw=digitalRead(p_ContSw);
s_Ign=digitalRead(p_Ign);
s_IsoSw=digitalRead(p_IsoSw);

	for( i=4; i<9; i++){
manageInterrupts(i);
	delay(200);
	}
}







///////loop////////////////////////////////////////////////////////////////////////////////
void loop()
{
t.run();

if(itrptSrc>-1) 
{
	detachAllInterrupts();

	int nSrc=whichPinInterrupted();
	
	for(int iSrc=0; iSrc < nSrc; iSrc++)  manageInterrupts(intSrc[iSrc]);

	itrptSrc=-1;

	attachnonAlInterrupts();
	if(AlArmed) attachAlInterrupts();
}

if (phone>0) managePhone();

if (monitor>0) manageMonitor();

} //loop





///////interrupt handlers////////////////////////////////////////////////////////////////////////////////
void manageInterrupts(int iSrc){

	if(prnt) {Serial.print(iSrc); Serial.print(Source[iSrc]); Serial.println("...");}
	switch(iSrc){

			case 0:
				if(debounce(p_pirTamp)!=LOW&&AlArmed) {AlSrc=itrptSrc; 
				playSrn();
				}
			break;

			case 1:
				if(debounce(p_pirAl)  !=LOW&&AlArmed) {AlSrc=itrptSrc;
				playSrn();
				}
			break;

			case 2:
				if(debounce(p_DoorSw) ==LOW&&AlArmed){AlSrc=itrptSrc; 
				t.setTimeout(doorAlDelay, playSrn);
				} 
			break;

			case 3:
			if(debounce(p_HbrkSw) ==LOW&&AlArmed){AlSrc=itrptSrc; 
			playSrn();
			} 
			break;

			case 4:
				if(s_Ign==LOW)  
				{
					if(prnt) Serial.println("IgnTurnedON");
					AlAllowed=false;
					SolAllowed=false;
					disarm();
					SolarOff();
					//timerContactor=
					//t.setTimeout(IgnDelay, ContactorOn);
				}
				else
				{
					if(prnt) Serial.println("IgnTurnedOFF");
					ContactorOff();

					AlAllowed=true;
					SolAllowed=true;
					if (s_SolSw==LOW) SolarOn();
				}
			break;

			case 5:
				if(s_IsoSw  ==LOW)
				{
					if(prnt) Serial.println("IsoSwClosed");
					if(AlArmed)
					{
						AlSrc=itrptSrc;
						playSrn();
					} 
				//allow/disallow solar
				} 
			break;

			case 6:

				if (s_TotAlSw==LOW)  {AlLevel=FullArmed; if(prnt) Serial.println("p_TotAlSw");}
				else if (s_PerAlSw==LOW) {AlLevel=PeriArmed; if(prnt) Serial.println("p_PerAlSw");}
				else {AlLevel=0;  if(prnt) Serial.println("disable");}

				if(!AlArmed&&AlLevel>0) {
				t.setTimeout(alarmArmDelay, arm); 
				enableblinkAlLEDFast();
				}
				else  disarm(); 
			break;

			case 7:
				if (s_SolSw==LOW)  {
				if(prnt) Serial.println("SolarClosed");
				if(whichBattIsCharged==0) SolarOn(); 
				}
				else SolarOff(); 
			break;

			case 8:
				if(s_ContSw==LOW)
				{
				if(whichBattIsCharged>0) SolarOff(); 
				if(digitalRead(p_Ign)==LOW)	enableblinkContLEDSlow();
				else	enableblinkContLEDFast();
				if(prnt) Serial.println("ContactorClosed");
				break;
				}
				else {disableblinkContLED();
				if(prnt) Serial.println("ContactorOpen");
				if (s_SolSw==LOW) SolarOn();
				}
			break;

			default:
				if(prnt) Serial.println("what the feck?");
			break;

	}




}

int whichPinInterrupted(){

int nSrc=0;

if(itrptSrc<4) {intSrc[nSrc++]=itrptSrc; return nSrc;}

if(debounce(p_Ign)    !=s_Ign)      {s_Ign=!s_Ign;         intSrc[nSrc++]=itrptSrc;}
if(debounce(p_IsoSw)  !=s_IsoSw)    {s_IsoSw=!s_IsoSw;     intSrc[nSrc++]=itrptSrc;}
if(debounce(p_TotAlSw)!=s_TotAlSw)  {s_TotAlSw=!s_TotAlSw; intSrc[nSrc++]=6;}
if(debounce(p_PerAlSw)!=s_PerAlSw)  {s_PerAlSw=!s_PerAlSw; intSrc[nSrc++]=6;}
if(debounce(p_SolSw)  !=s_SolSw)    {s_SolSw=!s_SolSw;     intSrc[nSrc++]=7;}
if(debounce(p_ContSw) !=s_ContSw)   {s_ContSw=!s_ContSw;   intSrc[nSrc++]=8;}

return nSrc;
}
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




void pirTampInterrupt(){itrptSrc=0;}
void pirAlInterrupt(){itrptSrc=1;}
void DoorSwInterrupt(){itrptSrc=2;}
void HbrkSwInterrupt(){itrptSrc=3;}
void IgnInterrupt(){itrptSrc=4;}
void IsoSwInterrupt(){itrptSrc=5;}
ISR(PCINT0_vect) {itrptSrc=10;}
void attachAlInterrupts(){
	if(AlLevel==FullArmed){
attachInterrupt(0, pirTampInterrupt, CHANGE); //p_pirAl 
attachInterrupt(1, pirAlInterrupt,   CHANGE); //p_pirTamp 
	}
attachInterrupt(2, DoorSwInterrupt,   CHANGE); //p_DoorSw
attachInterrupt(3, HbrkSwInterrupt,  CHANGE); //p_HbrkSw
}

void detachAlInterrupts(){
detachInterrupt(0); //p_pirAl 
detachInterrupt(1); //p_pirTamp
detachInterrupt(2); //p_DrSw
detachInterrupt(3); //p_HbrkSw
}



void attachnonAlInterrupts(){
attachInterrupt(4, IgnInterrupt,     CHANGE); //p_Ign
attachInterrupt(5, IsoSwInterrupt,   CHANGE); //p_IsoSw
}

void detachnonAlInterrupts(){
detachInterrupt(4); //p_pirAl 
detachInterrupt(5); //p_pirTamp
}


void attachAllInterrupts(){
attachAlInterrupts();
attachnonAlInterrupts();
}

void detachAllInterrupts(){
detachAlInterrupts();
detachnonAlInterrupts();
}
//Alarm///////////////////////////////////////////////////////////////////////////////
void arm(){
	if (!AlAllowed) return;

	if(prnt) Serial.println("Alarm Armed!");
	AlArmed=true;
	resetAl();
	digitalWrite(r_AlPow, LOW);
	enableblinkAlLEDSlow();
	attachAlInterrupts();
	}
void disarm(){
	detachAlInterrupts();
	AlArmed=false; 
	digitalWrite(r_AlPow, HIGH);
	if(prnt) Serial.println("Alarm Disarmed");
	disableblinkAlLED();
	}
void enableblinkAlLEDFast(){
	t.disable(timerblinkAlLEDSlow);
	if(AlLevel==FullArmed) t.enable(timerblinkAlLEDFast);
	else if(AlLevel==PeriArmed) t.enable(timerblinkAlLEDMedium);
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

if(!AlArmed||srnStatus==BUSY) return;
nAls++;

if(nAls>=maxnAls) {if(prnt) Serial.print("nAls>=maxnAls,disarming"); disarm(); return;}

srnStatus=BUSY;
srnOn() ; 
t.setTimeout(2*SrnDuration, srnOn);
t.setTimeout(3*SrnDuration, srnOff);
t.setTimeout(4*SrnDuration, freesrnStatus);
//enableblinkAlLEDFast();
if(prnt) Serial.print(Source[AlSrc]);
nexe=93;

}

void playWrn(int WrnDuration) {
srnOn();
t.setTimeout(WrnDuration, srnOff);
}

void freesrnStatus(){
srnStatus=FREE;
AlSrc=-1;
}


void srnOn(){
	if(prnt) Serial.println("SrnOn");
digitalWrite(r_SrnOn, LOW);
t.setTimeout(500, srnOnH);
}

void srnOnH(){
digitalWrite(r_SrnOn, HIGH);
}

void srnOff(){
	if(prnt) Serial.println("SrnOff");
digitalWrite(r_SrnOff, LOW);
t.setTimeout(500, srnOffH);
}

void srnOffH(){
digitalWrite(r_SrnOff, HIGH);
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
//////Contactor//////////////////////////////////
void ContactorOn(){

if(debounce(p_Ign)!=LOW) return;

digitalWrite(r_Cont, HIGH);	

timerContactor=-1;

}


void ContactorOff(){
if(timerContactor!=-1)
{ t.deleteTimer(timerContactor); timerContactor=-1;}

digitalWrite(r_Cont, LOW);
}
void blinkContLED(){
	digitalWrite(p_ContLED,HIGH);
	t.setTimeout(100, ContLEDL);
}

void ContLEDL(){
digitalWrite(p_ContLED,LOW);
}
void enableblinkContLEDFast(){
	t.disable(timerblinkContLEDSlow);
	t.enable(timerblinkContLEDFast);
}
void enableblinkContLEDSlow(){
	t.disable(timerblinkContLEDFast);
	t.enable(timerblinkContLEDSlow);
}
void disableblinkContLED(){
	t.disable(timerblinkContLEDFast);
	t.disable(timerblinkContLEDSlow);
	digitalWrite(p_ContLED,LOW);
}
//////Voltages///////////////////////////////////
void writeVoltages()
{
if(prnt) Serial.print("SP:");
printDouble(v_SolPan, 100);
if(prnt) Serial.print("V,LB:");
printDouble(v_BattL, 100);
if(prnt) Serial.print("V,VB: ");
printDouble(v_BattV, 100);
if(prnt) Serial.println("V");
}


int readVoltages()
{
double sensorValue;
v_SolPan=0.; v_BattV=0.; v_BattL=0.;

for(int counter=0; counter < 4; counter++)
{
delay(5);

sensorValue = double(analogRead(pv_SolPan));
v_SolPan+=sensorValue * f0 * 0.25;

sensorValue = double(analogRead(pv_BattV));
v_BattV+=sensorValue * f1 * 0.25;

sensorValue = double(analogRead(pv_BattL));
v_BattL+=sensorValue * f3 * 0.25;   
}

if(whichBattIsCharged!=0) {v_BattV-=0.12; v_BattL-=0.12;}
if(v_BattL<=v_MIN||v_BattV<=v_MIN) {//shudown all power consuming relays etc...
	nexe=93; return 1;}
else if(v_BattL>=v_MAX||v_BattV>=v_MAX)return -1;
else return 0;
}

void readwriteVoltages() //check often
{
	if(readVoltages()<0){ SolarOff();} 
writeVoltages();

}

void printDouble( double val, double precision){

int digits=int(val);

unsigned int frac;

frac = int((val - double(digits)) * abs(precision));


if(prnt) Serial.print(digits);
if(prnt) Serial.print('.');
if(frac<10) if(prnt) Serial.print("0");
if(prnt) Serial.print(frac);      
}



//Solar////////////////////////////////////////////////////////////////////////////////////
void SolarOff(){
	if(prnt) Serial.println("shut down Sol");
	digitalWrite(r_SolL, LOW);
	digitalWrite(r_SolV, LOW);
	whichBattIsCharged=0;
}
void SolarOn() {
if (!SolAllowed) return;
if(prnt) Serial.println("chargeSolL");
chargeSolL();

return;
}

void chargeSolL(){
//	if(prnt) Serial.println("will charge SolL"); 
	digitalWrite(r_SolV, LOW);
	digitalWrite(r_SolL, HIGH);
	whichBattIsCharged=1;
}
void chargeSolV(){
//	if(prnt) Serial.println("will charge SolV");
	digitalWrite(r_SolL, LOW);
	digitalWrite(r_SolV, HIGH);
	whichBattIsCharged=2;
}


////////////Timers/////////////////////////////////////////////////////////
void disablePhoneTimers(){
	
	if(timercheckPhone>0) t.disable(timercheckPhone);
}
void enablePhoneTimers(){
	
	if(timercheckPhone>0) t.enable(timercheckPhone);
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
void manageMonitor(){
	while(monitorSerial->available()&&!ended)
{

character = monitorSerial->read();
 //if(prnt) Serial.print(character);
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
void managePhone(){
	while(phoneSerial->available()&&!ended)
{

character = phoneSerial->read();
//if(prnt) Serial.print(character);
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

} //SERIAL AVAILABLE

if(ended)
{
	

//for( i=0; i<index; i++) if(prnt) Serial.print(PDU[i]);
//if(prnt) Serial.println("s&&e");

if(isRING) {
	if(prnt) Serial.print("isRING");
	delay(100);
	phoneYes();}
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
		disablePhoneTimers(); t.setTimeout(phnResponseDelay, listPhone); 
		if(prnt) Serial.print(totPDU); 
		if(prnt) Serial.println(" messages to process");}
	//else enablePhoneTimers();
	else{ 
		freephnStatus();
		if(nexe>-1) { execCommand();}
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
	t.setTimeout(phnResponseDelay, clearPhone);
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
		t.setTimeout(phnResponseDelay, clearPhone);

}
else   if(isCIND)
{
  	if(prnt) Serial.println("isCIND");
}

freephnStatus();
resetSerial();
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
}


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
  
  
	char state[5];
	if(prnt) Serial.print("Executing ");if(prnt) Serial.println(nexe);

// toggle digital pins
if(nexe>-1&&nexe<54) { 
digitalWrite(nexe,!digitalRead(nexe)); 
if(prnt) Serial.print("Pin ");if(prnt) Serial.print(nexe); if(prnt) Serial.print(" is ");  if(prnt) Serial.println(digitalRead(nexe));


	startSMS();

	appendtoSMS("Pin ",4);
	itoa(nexe,state,10); appendtoSMS(state,2);
	appendtoSMS(" is ",4);
	itoa(digitalRead(nexe),state,10); appendtoSMS(state,1);

	appendtoSMS("\"",1);

	if (phone>0)    sendSMS();
else if (monitor>0) monitorSMS();
	

} 
else switch(nexe){
case 55: disablePhoneTimers(); break;
case 56: enablePhoneTimers(); break;
case 57: resetIgn(); break;
case 58: resetInterrupts(); break;

//case 59: list
//
case 60: SolarOn(); break;
case 61: chargeSolL(); break;
case 62: chargeSolV(); break;

case 66: SolarOff(); break;
//case 69: list
//
//
case 70: resetAl(); break;
case 71: arm(); break;
case 72: disarm(); break;
case 73: playWrn(1000); break;
case 74: playWrn(4000); break;
case 77: playSrn(); break;
//case 79: list
//
//
case 80: resetPhn(); break;
case 81: takePicture(); break;
case 82: takeandsendPicture(); break;
case 84: checkStatus(); break;


case 90: resetAll(); break;
//case 89: list
//
default: 
	startSMS();
	writeReport(nexe);
	appendtoSMS("\"",1);

	if (phone>0)    sendSMS();
else if (monitor>0) monitorSMS();

	break;
//
  }

nexe=-1;

//t.setTimeout(phnResponseDelay, clearPhone);
}	

void freephnStatus(){
	phoneTime=0;
phnStatus=FREE;
}
void busyphnStatus(){
	phoneTime=millis();
phnStatus=BUSY;
}


void answerCall() {

	if(phnStatus==BUSY) return;
	busyphnStatus();	

phoneSerial->print("ATA");
delay(10);
phoneSerial->println();

if(prnt) Serial.println("ATA");

//busyphnStatus();
} 

void sendSMS() {

	if(phnStatus==BUSY) return;
	busyphnStatus();

	if(prnt) Serial.println("Sending SMS");
	disablePhoneTimers();
	t.setTimeout(30000,enablePhoneTimers);

if(phone==R520m){
t.setTimeout(100, phoneWake);
t.setTimeout(3000,phonepurgeSMS);
t.setTimeout(8000,phoneYes);
t.setTimeout(11000,phoneEnterMyNumber);
t.setTimeout(15000,phoneYes);
}
else if(phone==K700i){
	
t.setTimeout(100, phoneWake);
t.setTimeout(3000,phonepurgeSMS);
t.setTimeout(8000,phoneYes);
t.setTimeout(11000,phoneYes);
t.setTimeout(14000,phoneYes);
t.setTimeout(17000,phoneYes);
t.setTimeout(20000,phoneYes);
}

}




void listPhone() {

	if(phnStatus==BUSY) return;
	busyphnStatus();

if(prnt) Serial.println("listing SMS");
phoneSerial->println("AT+CMGL=4");
}

void clearPhone() {

	if(phnStatus==BUSY) return;
	busyphnStatus();

if(prnt) Serial.println("deleting SMS");
phoneSerial->print("AT+CMGD=");
phoneSerial->print(nPDU);
phoneSerial->println(",0"); 

resetSerial();

}


void readSMS() {

	if(phnStatus==BUSY) return;
	busyphnStatus();
if(prnt) Serial.println("reading SMS");
phoneSerial->print("AT+CMGR=");
phoneSerial->println(nPDU);
}


void takePicture()
{
	
if(phone!=K700i) return;

	if(phnStatus==BUSY) return;
	busyphnStatus();

t.setTimeout(100, phoneWake);
t.setTimeout(2000,phoneKeyboardMode);
t.setTimeout(4000,phoneCameraMode);
t.setTimeout(7000,phoneFocus);
t.setTimeout(10000,phoneCapture);
t.setTimeout(20000,enablePhoneTimers);
t.setTimeout(20000,phonegoHome);
}



void takeandsendPicture()
{
	
if(phone!=K700i) return;

	if(phnStatus==BUSY) return;
	busyphnStatus();

t.setTimeout(100, phoneWake);
t.setTimeout(2000,phoneKeyboardMode);
t.setTimeout(4000,phoneCameraMode);
t.setTimeout(7000,phoneFocus);
t.setTimeout(10000,phoneCapture);
t.setTimeout(13000,phoneYes);
t.setTimeout(16000,phoneYes);
t.setTimeout(19000,phoneDownArrow);
t.setTimeout(22000,phoneYes);
t.setTimeout(25000,phoneYes); 
t.setTimeout(28000,phoneYes);
t.setTimeout(40000,enablePhoneTimers);
t.setTimeout(40000,phonegoHome);
}



void initializePhone(){
//phoneWake();
//delay(2000);
//phoneHello();
//delay(2000);
if (phone==K700i) 
{phoneKeyboardMode();
delay(1000);
}
prepPhone();
}

void phonegoHome() {phoneSerial->println("at+ckpd=\":R\",10");}

void phoneWake(){
if (phone==K700i) {phoneSerial->println("at+ckpd=\"P\"");}
else   {
  phoneSerial->println("at+ckpd=\"E\"");delay(500);
  phoneSerial->println("at+ckpd=\"E\"");
}}

void phoneHello(){phoneSerial->println("AT");}
void phoneKeyboardMode() {phoneSerial->println( "AT+CSCS=\"8859-1\"");}
void phoneWriteNewMessage() {phoneSerial->println( "AT*EAPP=0");}
void phoneStartNewMessage() {phoneSerial->print( "AT*EAPP=0,0,\"");}
void phoneCameraMode() {phoneSerial->println( "AT*EAPP=10");}
void phoneFocus(){phoneSerial->println("at+ckpd=\":F\"");}
void phoneCapture(){phoneSerial->println("at+ckpd=\":C\"");}
void phoneUpArrow(){phoneSerial->println("at+ckpd=\"^\"");}
void phoneDownArrow(){phoneSerial->println("at+ckpd=\"v\"");}
void phoneYes(){
if (phone==K700i)phoneSerial->println("at+ckpd=\"[\"");
else            phoneSerial->println("at+ckpd=\"S\"");}
void phoneReturn(){phoneSerial->println("at+ckpd=\":R\",10");}
void phoneLeftFunction(){phoneSerial->println("at+ckpd=\"[\"");}
void phoneEnterMyNumber(){phoneSerial->println("at+ckpd=\"00447772259300\"");}
void phonepurgeSMS(){

	delay(100);
	int i=0;
	while(i<lengthSMS){
phoneSerial->print(SMS[i]);
if(prnt) Serial.print(SMS[i]);
i++;
delay(10);
	}
	phoneSerial->println();
if(prnt) Serial.println();
}


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

//////////////////////////reports//////////////////////////////////////////////////////
void startSMS() {
resetSMS();
appendtoSMS("AT*EAPP=0,0,\"",13);
return;
}

void writeReport(int nReport){
switch(nReport)
{
//reports
case 91: {reportTimerSince();break;}
case 92: {reportTimerTogo();break;}
case 93: {reportVoltages(); reportAl(); break;} 
case 95: {reportAl();break;} 
case 96: {readwriteVoltages(); reportVoltages(); reportAl();  break;} 
case 97: {reportState(); break;} 
case 98: {reportOutputs();break;}
		 //
case 59: {listOther(); break;}
case 69: {listSol(); break;}
case 79: {listAl(); break;}
case 89: {listPhn(); break;}
case 99: {listReports();break;
	default: break;	 
		 } 

}
}


void appendtoSMS(const char *partSMS, int len){
//len=	strlen(partSMS);
if(lengthSMS+len>=maxSMS) return;
if(lengthSMS>0)lengthSMS--;
strcpy(&SMS[lengthSMS], partSMS);
lengthSMS+=len;
SMS[lengthSMS++]='\0';
}





void listOther(){
	delay(100);
appendtoSMS("disTmrs(55),",12);
appendtoSMS("enTmrs(56),",11);
appendtoSMS("rsetIgn(57),",12);
appendtoSMS("rsetIntrpt(58),",13);
appendtoSMS("rsetAll(90),",12);
}

void listSol(){
	delay(100);
appendtoSMS("rsetSol(60),",12);
appendtoSMS("chgSolL(61),",12);
appendtoSMS("chgSolV(62),",12);
appendtoSMS("SolarOff(66),",13);
}

void listAl(){
	delay(100);
appendtoSMS("rsetAl(70),",11);
appendtoSMS("arm(71),",8);
appendtoSMS("disarm(72),",11);
appendtoSMS("plyWrn1(73),",12);
appendtoSMS("plyWrn4(74),",12);
appendtoSMS("playSrn(77),",12);
}

void listPhn(){
	delay(100);
appendtoSMS("resetPhn(80),",13);
appendtoSMS("takePic(81),",12);
appendtoSMS("takesndPic(82),",15);
appendtoSMS("phnOn(83),",10);
appendtoSMS("chckStatus(84),",15);
appendtoSMS("phnOff(85),",11);

}



void listReports(){//130

appendtoSMS("listOther(59),",13);
appendtoSMS("listSol(69),",12);
appendtoSMS("listAl(79),",11);
appendtoSMS("listPhn(89),",12);

appendtoSMS("TmrSince(91),",13);
appendtoSMS("TmrTogo(92),",12);
appendtoSMS("Voltages(93),",13);
appendtoSMS("PhnStatus(94),",14);
appendtoSMS("Al(95),",7);
appendtoSMS("All(96),",13);
appendtoSMS("Inputs(97),",11);
appendtoSMS("Outputs(98),",12);
}





void reportAl()//30
{
char state;

if(AlArmed){
	if(AlLevel==FullArmed) appendtoSMS("FullArmed",9);
	else if(AlLevel==PeriArmed) appendtoSMS("PeriArmed",9);
}
else appendtoSMS("DisArmed",8);

if(nAls>0)
{
appendtoSMS(" nAls:",6); //+6=15
state=intToChar(nAls);
appendtoSMS(&state,1);//+1=16

for(int i=0; i<5; i++){
state=Source[AlSrc][i];
appendtoSMS(&state,1);//+5=30
}
}

}

void reportVoltages()//33
{
appendtoSMS("SP:",3); //3
reportDouble(v_SolPan, 2);//3+5=8
appendtoSMS("V, LB:",6);//8+6=14
reportDouble(v_BattL, 2);//14+5=19
appendtoSMS("V, VB:",6);//19+6=25
reportDouble(v_BattV, 2);//25+5=30
appendtoSMS("V, ",3);//30+3=33
}


void reportTimerSince(){//44
  appendtoSMS("since. ",7); //7
	if(timerReset>-1) appendtoSMS("tmrRset: ",9); if(prnt) Serial.print("timerReset: "); reportLong(t.since(timerReset)); // 9+8+27=44
}
void reportTimerTogo(){//43
  appendtoSMS("togo. ",6);
	if(timerReset>-1) appendtoSMS("tmrRset: ",9); if(prnt) Serial.print("timerReset: "); reportLong(resetInterval-t.since(timerReset));
}
void reportState(){
char state[2];
appendtoSMS("pirAl",5);
if(digitalRead(p_pirAl)  !=LOW) appendtoSMS("+ ",2); 
else appendtoSMS("- ",2);

appendtoSMS(", pirTmp",8);
if(digitalRead(p_pirTamp)!=LOW) appendtoSMS("+ ",2); 
else appendtoSMS("- ",2);

appendtoSMS("Door",4);
if(digitalRead(p_DoorSw) ==LOW) appendtoSMS("+ ",2); 
else appendtoSMS("- ",2);

appendtoSMS(", Hbrk",6);
if(digitalRead(p_HbrkSw) ==LOW) appendtoSMS("+ ",2); 
else appendtoSMS("- ",2);

appendtoSMS(", Cont",6); 
if(s_ContSw==LOW) appendtoSMS("+ ",2); 
else appendtoSMS("- ",2);

appendtoSMS(", Iso",5); 
if(s_IsoSw  ==LOW) appendtoSMS("+ ",2); 
else appendtoSMS("- ",2);

appendtoSMS(", Ign",5);
if(s_Ign==LOW)  appendtoSMS("+ ",2); 
else appendtoSMS("- ",2);

appendtoSMS(", Sol",5);
if (s_SolSw==LOW)  appendtoSMS("+ ",2); 
else appendtoSMS("- ",2);

if (s_TotAlSw==LOW)   appendtoSMS(", TotAlSw:+",11);
else if (s_PerAlSw==LOW) appendtoSMS(", PerAlSw:+",11);
else appendtoSMS(", AlSw-",7);

return;
}

void reportOutputs(){
char state[2];
appendtoSMS("Phone05: ",9); //ULN2-3
itoa(digitalRead(r_Phone),state,10); appendtoSMS(state,1);
appendtoSMS(",Sink06: ",9); //ULN2-2
itoa(digitalRead(r_Sink),state,10); appendtoSMS(state,1);
appendtoSMS(",Cont07: ",9); //ULN2-1
itoa(digitalRead(r_Cont),state,10); appendtoSMS(state,1);
appendtoSMS(",SolL08: ",9); //ULN3-4
itoa(digitalRead(r_SolL),state,10); appendtoSMS(state,1);
appendtoSMS(",SolV09: ",9); //ULN3-3
//rel5v
itoa(digitalRead(r_SolV),state,10); appendtoSMS(state,1);
appendtoSMS(",SrnOn10: ",10);
itoa(digitalRead(r_SrnOn),state,10); appendtoSMS(state,1);
appendtoSMS(",SrnOff11: ",11);
itoa(digitalRead(r_SrnOff),state,10); appendtoSMS(state,1);
appendtoSMS(",DH1112: ",9);
itoa(digitalRead(r_DH11),state,10); appendtoSMS(state,1);
appendtoSMS(",AlPow40: ",10);
itoa(digitalRead(r_AlPow),state,10); appendtoSMS(state,1);
//appendtoSMS(",AlLED50: ",10);
//itoa(digitalRead(p_AlLED),state,10); appendtoSMS(state,1);
return;
}


//////////////////////////reports//////////////////////////////////////////////////////



void reportDouble( double val, int nfrac){

double dval=val, div=1.;
int ndigits=1, ival;
char state[1];
while(int(abs(dval))>=10) {ndigits++; div*=10; dval=val/div;}

dval=val;

for(int i=ndigits; i>0; i--){
ival=int(dval/div);
state[0]=intToChar(ival);
appendtoSMS(state, 1);
dval=dval-ival*div;
div/=10.;
}

appendtoSMS(".", 1);


div=1.;
i=nfrac; while(i>0) {div*=10;  i--;}
dval=(val-int(val))*div;

for(i=nfrac; i>0; i--){
div/=10.;
ival=int(dval/div);
state[0]=intToChar(ival);
appendtoSMS(state, 1);
dval=dval-ival*div;
}

return;
}



void reportLong( long val){
char state;
int hours, minutes, seconds, ival;
long cval;

val/=1000;

hours=int(val/3600);
cval=60*hours;
cval*=60;
minutes=int((val-cval)/60);
cval+=60*minutes;
seconds=int(val-cval);

if(prnt) Serial.print(hours);if(prnt) Serial.print(" h ");
if(prnt) Serial.print(minutes);if(prnt) Serial.print(" m ");
if(prnt) Serial.print(seconds);if(prnt) Serial.println(" s ");

ival=int(hours/10);
state=intToChar(ival);
appendtoSMS(&state, 1);
state=intToChar(hours-10*ival);
appendtoSMS(&state, 1);
appendtoSMS("h ", 2); //4

ival=int(minutes/10);
state=intToChar(ival);
appendtoSMS(&state, 1);
state=intToChar(minutes-10*ival);
appendtoSMS(&state, 1);
appendtoSMS("m ", 2); //4+4=8

/*ival=int(seconds/10);
state[0]=intToChar(ival);
appendtoSMS(state, 1);
state[0]=intToChar(seconds-10*ival);
appendtoSMS(state, 1);
appendtoSMS("s", 1);*/


return;
}
void checkTap(){
//sink
if(digitalRead(p_Tap)  ==LOW) {
	digitalWrite(r_Sink, HIGH);
	}
else digitalWrite(r_Sink, LOW);
}
/*


void if(prnt) Serial.print(char* txt){
	if(prnt) Serial.print(txt);
}
void if(prnt) Serial.println(char* txt){
	if(prnt) Serial.print(txt);
	if(prnt) Serial.print("\n");
}
*/

//#define define_max(type) void max(type d1,type d2){\
//return;\
//}




