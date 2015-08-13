#include <SimpleTimer.h>
SimpleTimer t;

//voltages
const  int pv_SolPan   = A0; //blue
const  int pv_BattV     = A1; //orange
//const  int pv_spare      = A2; //yellow
const  int pv_BattL     = A3; //green
//const  int pv_spare2     = A4; //yellow
//A8x->A15x

//Block 1:
//const  int p_phone2Tx   = 0; //Rx0 B3  -> UTB3
//const  int p_phone2Rx   = 1; //Tx0 B8  -> //UTB8
//interrupts
const  int p_pirAl     = 3; //interrupt 1 G2/G7->Volt. Reducer
const  int p_pirTamp    = 2; //interrupt 0 G2/G7->Volt. Reducer
const  int p_ContSw    = 4; //grey from ROV57

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
//const  int p_ustapTrigger= 17; //G3 ->ROG6
//interrupts
const  int p_IsoSw     = 18; //  interrupt5 white from ROV5
const  int p_Ign     = 19; //  interrupt4 blue A6->Volt. Reducer
const  int p_HbrkSw    = 20; //  interrupt3 purple from ROV5
const  int p_AlSw  = 21; //  interrupt2 green from RUC5

//22x,23x,

// Block 4:
//const  int p_B6  = 24; // UTB6
const  int p_SolSw  = 25; // B2->SW3->A2->GND

//Empty
// 26x->39x

//Block 5:
//uln2003-1
const  int r_AlPow = 40; //ULN1-1
//const  int r_Spare    = 41; //ULN1-2
//const  int p_LED5       = 42;
//const  int r_Spare    = 43; //ULN1-3
//const  int p_LED4       = 44;
//const  int p_usrTrigger = 45; // G8->ROG3
const  int p_StatusLED       = 46;
//const  int p_usrEcho    = 47; //G6->ROG7
const  int p_ContLED       = 13; //48;
//49x, spare at RUC
const  int p_AlLED    = 50;
//const  int p_C7        = 51; //RUC7
//52x, 53x

//voltage multipliers
const double f0=0.02575,f1=0.01516, f2=0.01580,
f3=0.01547, f4=0.01445, v_SolPanMin=4.0, v_CRI=13.5, v_MAX=14.2;

const unsigned int maxnAls=4, maxnTrigs=1, SrnDuration=5000, 
 AlPowDelay=100, SolDelay=5000, interruptDelay=500, durationAledLED=2000;

int nAls, nTrigs, nTrigsold, timerVoltage,  timerReset, timerblinkAlLEDSlow, timerblinkAlLEDFast, 
srnStatus, timerblinkContLEDSlow, timerblinkContLEDFast;

const unsigned long IgnDelay=300000,
resetInterval=43200000,
//resetInterval=120000,
//readVoltageInterval=10000, resetInterval=600000, SolSwInterval=60000;
readVoltageInterval=60000,  SolSwInterval=7200000;
//resetInterval=1000*60*60*12 SolSwInterval=1000*60*60*2;
 
volatile bool IgnTriggered, AlSwTriggered,
			  IsoSwTriggered, HbrkSwTriggered,
			  pirAlTriggered, pirTampTriggered;

unsigned long IgnTime,sinceIgn;

double v_SolPan , v_BattL  , v_BattV , v_SolCont;

bool AlArmed, SolSwTriggered, SolSwedON, SolSwLast,
IgnON, AlSwedON, IsoSwCLOSED, HbrkSwDOWN, ContactorCLOSED;


//phone variables
const int R520m=0;
const int K700i=1;
const int phone=R520m;
//const int phone=K700i;

const int BUSY=0;
const int FREE=1;

//1000 1sec
//60000 1min
//3600000 1hr
const unsigned int phnPDUInterval=10000, maxindex=250, 
maxSMS=140, phnResponseDelay=2000;

const unsigned long phnStatusInterval=7201000, phoneDiscardInterval=30000;

const char CIND[9]="+CIND:/0", CMGL[9]="+CMGL:/0", CMGR[9]="+CMGR:/0", myPDU[13]="7727523900/0",
CPMS[9]="+CPMS:/0", RING[7]="RING/0", CMGD[7]="CMGD/0" ;

char SMS[maxSMS], cmd[4], PDU[maxindex], character;

int whichBattIsCharged, nPDU,lengthSMS,PDUStatus, counterSerial,jmyPDU, jmyPDU0,nexe, totPDU,
jCIND,  jCMGL,  jCMGR,  jCMGD,  jCPMS,  jRING,
jCIND0, jCMGL0, jCMGR0, jCMGD0, jCPMS0, jRING0,
i,j, index, phnBattLev, phnSigLev, phnBattWarn, phnCharger, phnService, phnStatus,
phnSounder, newMessage, newCall, timercheckSMS, timercheckStatus;

bool isCIND, isCMGD, isCMGL, isCMGR, isCPMS, isRING, ended, ending, started, ismynumber;

unsigned long phoneTime;


void setup()
{
Serial.begin(9600); //initialize serial communication
Serial3.begin(9600); //initialize serial communication

//timertoggleSolar=t.setInterval(SolSwInterval, toggleSol);
//timerVoltage=t.setInterval(5000, readwriteVoltages);
timercheckSMS= t.setInterval(phnPDUInterval, prepSMS);
timercheckStatus=t.setInterval(phnStatusInterval, checkStatus);
//timerReset=t.setInterval(resetInterval, resetVariables);
timerblinkContLEDSlow= t.setInterval(2000, blinkContLED);
timerblinkContLEDFast= t.setInterval(200, blinkContLED);
timerblinkAlLEDSlow= t.setInterval(2000, blinkAlLED);
timerblinkAlLEDFast= t.setInterval(200, blinkAlLED);

attachInterrupt(0, pirTampInterrupt, CHANGE); //p_pirAl 
attachInterrupt(1, pirAlInterrupt,   CHANGE); //p_pirTamp //MAY MAKE LOW LATER
attachInterrupt(2, AlSwInterrupt,    CHANGE); //p_AlSw
attachInterrupt(3, HbrkSwInterrupt,  CHANGE); //p_HbrkSw
attachInterrupt(4, IgnInterrupt,     CHANGE); //p_Ign
attachInterrupt(5, IsoSwInterrupt,   CHANGE); //p_IsoSw

resetVariables();

}

///////////resets///////////////////////////////////////////////////////////////////
void resetVariables(){
Serial.println("resetting...........................");
delay(5000);
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

pinMode(p_Ign, INPUT);  
pinMode(p_pirAl, INPUT);
pinMode(p_pirTamp, INPUT);

//Listening, pull-ups
pinMode(p_AlSw, INPUT);    digitalWrite(p_AlSw, HIGH);
pinMode(p_IsoSw, INPUT);    digitalWrite(p_IsoSw, HIGH);
pinMode(p_ContSw, INPUT);    digitalWrite(p_ContSw, HIGH);
pinMode(p_HbrkSw, INPUT);    digitalWrite(p_HbrkSw, HIGH);
pinMode(p_SolSw, INPUT);    digitalWrite(p_SolSw, HIGH);

//pinMode(42, INPUT);    digitalWrite(42, HIGH);
//pinMode(43, INPUT);    digitalWrite(43, HIGH);
//pinMode(44, INPUT);    digitalWrite(44, HIGH);
//pinMode(45, INPUT);    digitalWrite(45, HIGH);
pinMode(52, INPUT);    digitalWrite(52, HIGH);
pinMode(53, INPUT);    digitalWrite(53, HIGH);

nexe=-1;

resetAl();
resetIgn();
resetSol();
resetSerial();
resetPhn();
resetIntrpt();
}





void resetIgn(){
IgnTime=0; 
sinceIgn=0;
}

void resetAl(){
nAls=0;
nTrigs=0;
freesrnStatus();
}
void resetSol() {
SolSwTriggered=false;
SolSwLast=digitalRead(p_SolSw);
SolSwedON=(SolSwLast==LOW); 
if(!SolSwedON){ Serial.println("!SolSwedON"); shutSol(); return;}

chargeSolL();

return;
}

void resetPhn(){
phoneTime=0;
freephnStatus();
resetSMS();
resetPDU();
phoneInitialize();
}
void resetSMS(){
lengthSMS=0;
SMS[lengthSMS]='\0';
}

void resetSerial(){

ending=false; ended=false; started=false;

counterSerial=0; 
isCIND=false;isCMGL=false;isCMGR=false; isCMGD=false;isCPMS=false;isRING=false;
jCIND=0;      jCMGL=0;     jCMGR=0;      jCMGD=0;     jCPMS=0;     jRING=0;
jCIND0=0;     jCMGL0=0;    jCMGR0=0;     jCMGD0=0;    jCPMS0=0;    jRING0=0;

resetPDU();
}
void resetPDU(){
index=0;
PDU[index]='\0';
}

void resetIntrpt(){
IgnTriggered=true; AlSwTriggered=true; IsoSwTriggered=true; 
HbrkSwTriggered=true; pirAlTriggered=true; pirTampTriggered=true;
}





///////loop////////////////////////////////////////////////////////////////////////////////
void loop()
{
t.run();

if (digitalRead(52)==LOW) nexe=99;
 if (digitalRead(53)==LOW) reportTimerSince();



//	else if (digitalRead(43)==LOW) {answerCall(); }
//	else if (digitalRead(44)==LOW) {readSMS(); }
//	else if (digitalRead(45)==LOW) {sendSMS(); }
//	else if (digitalRead(46)==LOW) {deleteSMS(); }
//    else if (digitalRead(47)==LOW) {freephnStatus();}

while(Serial3.available()&&!ended)
{

character = Serial3.read();
//Serial.print(character);
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
            Serial.println();
			Serial.print(PDU[0]);Serial.print(PDU[1]); Serial.println(PDU[2]);

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
	

//for( i=0; i<index; i++) Serial.print(PDU[i]);
//Serial.println("s&&e");

if(isRING) {
	Serial.print("isRING");
	delay(100);
	phoneYes();}
//{ 
//
//t.setTimeout(60000, phnHangup);
//answerCall();
//}
else if(isCMGD){ 
Serial.println("isCMGD");
t.setTimeout(1000, enableTimers);
}
else if(isCPMS) 
{
	if( totPDU!=0) { 
		disableTimers(); t.setTimeout(phnResponseDelay, listSMS); 
		Serial.print(totPDU); 
		Serial.println(" messages to process");}
	//else enableTimers();
	else{ 
		freephnStatus();
		if(nexe>-1) execCommand();
		
		
		enableTimers();}
}
else if(isCMGL) 
{
	Serial.print("nPDU: "); Serial.print(nPDU); Serial.print("status: "); Serial.println(PDUStatus);


	if(PDUStatus==0||PDUStatus==1) 
	{Serial.print("Message ");Serial.print(nPDU);Serial.println(" is received");
	t.setTimeout(phnResponseDelay, readSMS);
	}
	else {
	Serial.print("Message ");Serial.print(nPDU);Serial.println(" is not received"); 
	t.setTimeout(phnResponseDelay, deleteSMS);
	}
}
else if(isCMGR) {
		Serial.print("Message ");Serial.print(nPDU);Serial.println(" received, checking source");

		
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
  //Serial.print(PDU[i]);
  }
  else jmyPDU=0;
  
  if(jmyPDU==9) {ismynumber=true; break;}
  
  i++;
}

		if(ismynumber)
		{

			while( PDU[i]!='\n'&&i<index) {
			//  Serial.print(i);Serial.print(PDU[i]);Serial.println(index); 
			i++; }
			for( j=i-5; j<i-1; j++) cmd[j-(i-5)]=PDU[j]; 
			nexe=decodeCommand(cmd);
			
			Serial.print("Message ");Serial.print(nPDU);
			Serial.print(" is from me : "); Serial.println(nexe);
 
		}
		else
		{
			//not from me
			Serial.print("Message ");
			Serial.print(nPDU);
			Serial.println("is not from me");
					}
		t.setTimeout(phnResponseDelay, deleteSMS);

}
else   if(isCIND)
{
  	Serial.println("isCIND");
i=jCIND;
while( PDU[i]!=','&&i<index) i++;
i--;

phnBattLev=atoi(&PDU[i]);
phnSigLev=atoi(&PDU[i+2]);
phnBattWarn=atoi(&PDU[i+4]);
phnCharger=atoi(&PDU[i+6]);
phnService=atoi(&PDU[i+8]);
phnSounder=atoi(&PDU[i+10]);
newMessage=atoi(&PDU[i+12]);
newCall=atoi(&PDU[i+14]);
Serial.print("phnBattLev:"); Serial.print(phnBattLev);
Serial.print(" phnSigLev:"); Serial.print(phnSigLev);
Serial.print(" phnBattWarn:"); Serial.print(phnBattWarn);
Serial.print(" phnCharger:"); Serial.println(phnCharger);
Serial.print(" phnService:"); Serial.print(phnService);
Serial.print(" phnSounder:"); Serial.print(phnSounder);
Serial.print(" newMessage:"); Serial.print(newMessage);
Serial.print(" newCall:"); Serial.println(newCall);

if(phnBattLev<3||phnBattWarn>0) {chargePhn(); nexe=94;}
else if(phnBattLev>4) nochargePhn();

}


freephnStatus();
resetSerial();
}


//timeout
if(phoneTime!=0&&(millis()-phoneTime>phoneDiscardInterval))
{
// Serial.print(starting);   Serial.print(started);   Serial.print(ending);   Serial.print(ended);
//Serial.println(millis()-phoneTime);
	
for( int k=0; k<index; k++)     Serial.print(PDU[k]);

Serial.println("too long wait");

resetSerial();
freephnStatus();
enableTimers();
}





/////////////////////////////////////////////////////////////////////////////

if((AlArmed&&(pirAlTriggered||pirTampTriggered||HbrkSwTriggered))
   ||IsoSwTriggered||AlSwTriggered||IgnTriggered)
{
delay(interruptDelay);

if(AlArmed)
{
nTrigsold=nTrigs;

	if(pirAlTriggered){pirAlTriggered=false;  
	
		if((digitalRead(p_pirTamp)==LOW)) nTrigs=maxnTrigs+1;
		if((digitalRead(p_pirAl)==LOW)) nTrigs++;
		Serial.print(" pirAlTriggered nTrigs:"); Serial.println(nTrigs);
	}
	else if(pirTampTriggered){pirTampTriggered=false; 

		if((digitalRead(p_pirTamp)==LOW)) nTrigs=maxnTrigs+1;
		if((digitalRead(p_pirAl)==LOW)) nTrigs++;
		Serial.print(" pirTampTriggered nTrigs:"); Serial.println(nTrigs);
	}
	else if(HbrkSwTriggered){HbrkSwTriggered=false; 

		HbrkSwDOWN=(digitalRead(p_HbrkSw)==LOW);
		if(HbrkSwDOWN)  {nTrigs=maxnTrigs+1; Serial.print(" HbrkSwDOWN nTrigs:"); Serial.println(nTrigs);}
		  else Serial.println("HbrkSwUP");
	}

	if(nTrigs>nTrigsold){enableblinkAlLEDFast();
	t.setTimeout(3*SrnDuration+2000, enableblinkAlLEDSlow); }
	 
}


if(IsoSwTriggered) {IsoSwTriggered=false; 
	IsoSwCLOSED=(digitalRead(p_IsoSw)==LOW);
	if(IsoSwCLOSED)  { Serial.print("IsoSwCLOSED"); if(AlArmed) {nTrigs=maxnTrigs+1; Serial.print(" nTrigs:");} Serial.println(nTrigs);}
	else  Serial.println("IsoSwOPEN");
}
if(AlSwTriggered)  {AlSwTriggered=false; 
Serial.println("AlSwTriggered");
	AlSwedON=(digitalRead(p_AlSw)==LOW);
	if(AlSwedON) arm();
	else  disarm();
} //AlSwTriggered
if(IgnTriggered) {IgnTriggered=false; 
	IgnON=(IsoSwCLOSED&&digitalRead(p_Ign)==HIGH); 
	if(IgnON)  
	{
		Serial.println("IgnTurnedON");
		shutSol();
		IgnTime=millis();
	}
	else
	{
		Serial.println("IgnTurnedOFF");
		IgnTime=0;
		Serial.println("ContactorOPEN");
		digitalWrite(r_Cont, LOW);
		resetSol();
	}
} //IgnTriggered
} //if(pirAlTriggered||pirTampTriggered||HbrkSwTriggered||IsoSwTriggered||AlSwTriggered||IgnTriggered)



//EXECUTE SOLAR
SolSwTriggered=(digitalRead(p_SolSw)!=SolSwLast); 
if(SolSwTriggered) { delay(interruptDelay);  resetSol();}

//EXECUTE Al
if(AlArmed&&nAls<maxnAls&&nTrigs>maxnTrigs) {playSrn(); Serial.print(" nAls:"); Serial.print(nAls);}

//EXECUTE IGN
ContactorCLOSED=(digitalRead(p_ContSw)==LOW);

if(ContactorCLOSED) {
	if(IgnON)	enableblinkContLEDSlow();
	else	enableblinkContLEDFast();
}
else disableblinkContLED();

if(IgnON&&IgnTime>0&&!ContactorCLOSED)
{
	
	sinceIgn=millis()-IgnTime;
		if(sinceIgn>IgnDelay) {
  Serial.print(sinceIgn);
  Serial.print(">");
  Serial.print(IgnDelay);
		  Serial.println("ContactorCLOSED");
		digitalWrite(r_Cont, HIGH);

		}
}


} //loop





///////interrupt handlers////////////////////////////////////////////////////////////////////////////////
void AlSwInterrupt(){AlSwTriggered=true;}

void HbrkSwInterrupt(){HbrkSwTriggered=true;}

void IsoSwInterrupt(){IsoSwTriggered=true;}

void IgnInterrupt(){IgnTriggered=true;}



void pirAlInterrupt(){pirAlTriggered=true; }

void pirTampInterrupt(){pirTampTriggered=true;}


//Alarm///////////////////////////////////////////////////////////////////////////////
void arm(){
	Serial.println("Alarm Armed!");
	AlArmed=true;
	resetAl();
	digitalWrite(r_AlPow, LOW);
	enableblinkAlLEDSlow();
	}
void disarm(){
	AlArmed=false; 
	digitalWrite(r_AlPow, HIGH);
	Serial.println("Alarm Disarmed");
	disableblinkAlLED();
	}
void enableblinkAlLEDFast(){
	t.disable(timerblinkAlLEDSlow);
	t.enable(timerblinkAlLEDFast);
}
void enableblinkAlLEDSlow(){
	t.disable(timerblinkAlLEDFast);
	t.enable(timerblinkAlLEDSlow);
}
void disableblinkAlLED(){
	t.disable(timerblinkAlLEDFast);
	t.disable(timerblinkAlLEDSlow);
	digitalWrite(p_AlLED,LOW);
}
void playSrn() {

if(srnStatus==BUSY) return;
srnStatus=BUSY;

nAls++;

srnOn() ; 
t.setTimeout(2*SrnDuration, srnOn);
t.setTimeout(3*SrnDuration, srnOff);
t.setTimeout(3*SrnDuration+2000, freesrnStatus);
//nexe=95;
nTrigs=0;
}

void playWrn(int WrnDuration) {
srnOn();
t.setTimeout(WrnDuration, srnOff);
}

void freesrnStatus(){
srnStatus=FREE;
}


void srnOn(){
	Serial.println("SrnOn");
digitalWrite(r_SrnOn, LOW);
t.setTimeout(500, srnOnH);
}

void srnOnH(){
digitalWrite(r_SrnOn, HIGH);
}

void srnOff(){
	Serial.println("SrnOff");
digitalWrite(r_SrnOff, LOW);
t.setTimeout(500, srnOffH);
}

void srnOffH(){
digitalWrite(r_SrnOff, HIGH);
}




void blinkAlLED(){
	digitalWrite(p_AlLED,HIGH);
	t.setTimeout(100, AlLEDL);
}

void AlLEDL(){
digitalWrite(p_AlLED,LOW);
}
//////Contactor//////////////////////////////////
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
Serial.print("SP:");
printDouble(v_SolPan, 100);
Serial.print("V,LB:");
printDouble(v_BattL, 100);
Serial.print("V,VB: ");
printDouble(v_BattV, 100);
Serial.println("V");
}


int readVoltages()
{

double sensorValue = double(analogRead(pv_SolPan));
v_SolPan=sensorValue * f0;

sensorValue = double(analogRead(pv_BattV));
v_BattV=sensorValue * f1;

sensorValue = double(analogRead(pv_BattL));
v_BattL=sensorValue * f3;   

if(v_BattL>=v_MAX||v_BattV>=v_MAX) {Serial.println("v_BattL>=v_MAX||v_BattV>=v_MAX||v_SolPan<v_SolPanMin"); nexe=96; return -1;}
//else if(v_BattL>=v_CRI||v_BattV>=v_CRI) {Serial.println("v_BattL>=v_CRI||v_BattV>=v_CRI"); return 0;}
else return 1;
}

void readwriteVoltages() //check often
{
	if(readVoltages()<0){ shutSol();} 
//	else resetSol();
writeVoltages();

}

void printDouble( double val, double precision){

int digits=int(val);

unsigned int frac;
if(val >= 0)
frac = int((val - double(digits)) * abs(precision));
else
frac = int((double(digits)- val ) * abs(precision));

Serial.print(digits);
Serial.print('.');
Serial.print(frac);      
}



//Solar////////////////////////////////////////////////////////////////////////////////////
void toggleSol() {
if(whichBattIsCharged==0||whichBattIsCharged==2) chargeSolL();
else if(whichBattIsCharged==1) chargeSolV();
else shutSol();
}   

void shutSol(){
	Serial.println("shut down Sol");
	digitalWrite(r_SolL, LOW);
	digitalWrite(r_SolV, LOW);
	whichBattIsCharged=0;
}
void chargeSolL(){
	Serial.println("will charge SolL"); 
	digitalWrite(r_SolV, LOW);
	digitalWrite(r_SolL, HIGH);
	whichBattIsCharged=1;
}
void chargeSolV(){
	Serial.println("will charge SolV");
	digitalWrite(r_SolL, LOW);
	digitalWrite(r_SolV, HIGH);
	whichBattIsCharged=2;
}


////////////Timers/////////////////////////////////////////////////////////
void disableTimers(){
	
	t.disable(timercheckSMS);
	t.disable(timercheckStatus);
	t.disable(timerReset);
}


void enableTimers(){
	
	t.enable(timercheckSMS);
	t.enable(timercheckStatus);
	t.enable(timerReset);
}
void checkStatus() {

if(phnStatus==BUSY) return;
busyphnStatus();

delay(100);
Serial3.println("AT+CIND?");
Serial.println("checking Status");

}


void prepSMS() {

if(phnStatus==BUSY) return;
busyphnStatus();

delay(100);
Serial.println("preparing Storage");

Serial3.println("AT+CPMS=\"ME\"");

}


//Phone///////////////////////////////////////////////////////////////////////////////////
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

if(num<5||num==14||num==15||(num>17&&num<22)) {Serial.println("protected pin"); return -num;}

return num;
}	



void execCommand()		
{
  
  
	char state[5];
	Serial.print("Executing ");Serial.println(nexe);

// toggle digital pins
if(nexe>-1&&nexe<54) { 
digitalWrite(nexe,!digitalRead(nexe)); 
Serial.print("Pin ");Serial.print(nexe); Serial.print(" is ");  Serial.println(digitalRead(nexe));

resetSMS();
appendtoSMS("AT*EAPP=0,0,\"",13);
appendtoSMS("Pin ",4);


itoa(nexe,state,10); appendtoSMS(state,2);
appendtoSMS(" is ",4);
itoa(digitalRead(nexe),state,10); appendtoSMS(state,1);
appendtoSMS("\"",1);

sendSMS();


} 
else switch(nexe){
case 55: disableTimers(); break;
case 56: enableTimers(); break;
case 57: resetIgn(); break;
case 58: resetIntrpt(); break;
case 59: resetVariables(); break;
//
case 60: resetSol(); break;
case 61: chargeSolL(); break;
case 62: chargeSolV(); break;
case 63: toggleSol(); break;
case 66: shutSol(); break;

//
case 70: resetAl(); break;
case 71: arm(); break;
case 72: disarm(); break;
case 73: playWrn(500); break;
case 74: playWrn(5000); break;
case 77: playSrn(); break;

//
case 80: resetPhn(); break;
case 81: takePicture(); break;
case 82: takeandsendPicture(); break;
case 83: resetSerial(); break;
case 84: checkStatus(); break;
case 85: prepSMS(); break;
case 86: chargePhn(); break;
case 87: nochargePhn(); break;

default: 
	startSMS(); 
	switchReport(nexe);
    appendtoSMS("\"",1);
    sendSMS();
	break;
//
  }

nexe=-1;

//t.setTimeout(phnResponseDelay, deleteSMS);
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

Serial3.print("ATA");
delay(10);
Serial3.println();

Serial.println("ATA");

//busyphnStatus();
} 

void sendSMS() {

	if(phnStatus==BUSY) return;
	busyphnStatus();

	Serial.println("Sending SMS");

if(phone==R520m){
/*phoneWake();
delay(3000);
phonepurgeSMS();
delay(5000);
phoneYes();
delay(3000);
phoneEnterMyNumber();
delay(3000);
phoneYes();
delay(8000);
freephnStatus();*/

t.setTimeout(100, phoneWake);
t.setTimeout(3000,phonepurgeSMS);
t.setTimeout(8000,phoneYes);
t.setTimeout(11000,phoneEnterMyNumber);
t.setTimeout(14000,phoneYes);
t.setTimeout(22000,freephnStatus);

}
else if(phone==K700i){
/*phoneWake();
delay(3000);
phonepurgeSMS();
delay(5000);
 phoneYes();
delay(3000);
 phoneYes();
delay(3000);
phoneYes();
delay(3000);
 phoneYes();
delay(3000);
phoneYes();
delay(8000);
freephnStatus();*/
t.setTimeout(100, phoneWake);
t.setTimeout(3000,phonepurgeSMS();
t.setTimeout(8000,phoneYes();
t.setTimeout(11000,phoneYes();
t.setTimeout(14000,phoneYes();
t.setTimeout(17000,phoneYes();
t.setTimeout(20000,phoneYes();
t.setTimeout(28000,freephnStatus();

}

}



void listSMS() {

	if(phnStatus==BUSY) return;
	busyphnStatus();

Serial.println("listing SMS");
Serial3.println("AT+CMGL=4");
}
void deleteSMS() {

	if(phnStatus==BUSY) return;
	busyphnStatus();

Serial.println("deleting SMS");
Serial3.print("AT+CMGD=");
Serial3.print(nPDU);
Serial3.println(",0"); 

resetSerial();

}

void readSMS() {

	if(phnStatus==BUSY) return;
	busyphnStatus();
Serial.println("reading SMS");
Serial3.print("AT+CMGR=");
Serial3.println(nPDU);
}


void takePicture()
{
	
if(phone!=K700i) return;

	if(phnStatus==BUSY) return;
	busyphnStatus();

phoneWake();
delay(2000);
phoneKeyboardMode();
delay(2000);
phoneCameraMode();
delay(3000);
phoneFocus();
delay(3000);
phoneCapture();
delay(10000);
phonegoHome();
}



void takeandsendPicture()
{
	
if(phone!=K700i) return;

	if(phnStatus==BUSY) return;
	busyphnStatus();

phoneWake();
delay(2000);
phoneKeyboardMode();
delay(2000);
phoneCameraMode();
delay(3000);
phoneFocus();
delay(3000);
phoneCapture();
delay(3000);
phoneYes();
delay(3000);
 phoneYes();
delay(3000);
phoneDownArrow();
delay(3000);
phoneYes();
delay(3000);
phoneYes(); 
delay(3000);
phoneYes();
delay(10000);
phonegoHome();
}



void phoneInitialize(){
phoneWake();
delay(2000);
phoneHello();
delay(2000);
if (phone==K700i) phoneKeyboardMode();
}

void phonegoHome() {Serial3.println("at+ckpd=\":R\",10");}
void phoneWake(){
if (phone==K700i) {Serial3.println("at+ckpd=\"P\"");}
else   {
  Serial3.println("at+ckpd=\"E\"");delay(500);
  Serial3.println("at+ckpd=\"E\"");
}}

void phoneHello(){Serial3.println("AT");}
void phoneKeyboardMode() {Serial3.println( "AT+CSCS=\"8859-1\"");}
void phoneWriteNewMessage() {Serial3.println( "AT*EAPP=0");}
void phoneStartNewMessage() {Serial3.print( "AT*EAPP=0,0,\"");}
void phoneCameraMode() {Serial3.println( "AT*EAPP=10");}
void phoneFocus(){Serial3.println("at+ckpd=\":F\"");}
void phoneCapture(){Serial3.println("at+ckpd=\":C\"");}
void phoneUpArrow(){Serial3.println("at+ckpd=\"^\"");}
void phoneDownArrow(){Serial3.println("at+ckpd=\"v\"");}
void phoneYes(){
if (phone==K700i)Serial3.println("at+ckpd=\"[\"");
else            Serial3.println("at+ckpd=\"S\"");}
void phoneReturn(){Serial3.println("at+ckpd=\":R\",10");}
void phoneLeftFunction(){Serial3.println("at+ckpd=\"[\"");}
void phoneEnterMyNumber(){Serial3.println("at+ckpd=\"00447772259300\"");}
void phonepurgeSMS(){

	delay(100);
	int i=0;
	while(i<lengthSMS){
Serial3.print(SMS[i]);
Serial.print(SMS[i]);
i++;
delay(10);
	}
	Serial3.println();
Serial.println();
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

void chargePhn(){
	Serial.println("will charge Phone"); 
	digitalWrite(r_Phone, HIGH);
}
void nochargePhn(){
	Serial.println("will not charge Phone"); 
	digitalWrite(r_Phone, LOW);
}
//////////////////////////reports//////////////////////////////////////////////////////
void startSMS() {
resetSMS();
appendtoSMS("AT*EAPP=0,0,\"",13);
return;
}

void switchReport(int nReport){
switch(nReport)
{
//reports
case 88: {reportTimerSince();break;}
case 89: {reportTimerTogo();break;}
case 90: {listAl(); break;}
case 91: {listSol(); break;}
case 92: {listPhn(); break;}
case 93: {reportCommands();break;} 
case 94: {reportphnStatus(); break;}
case 95: {reportAl();break;} 
case 96: {readwriteVoltages(); reportVoltages(); break;} 
case 97: {reportInputs();break;} 
case 98: {reportOutputs();break;}
default: {reportReports();break;} 
}
}


void appendtoSMS(char *partSMS, int len){
if(lengthSMS+len>=maxSMS) return;
if(lengthSMS>0)lengthSMS--;
strcpy(&SMS[lengthSMS], partSMS);
lengthSMS+=len;
SMS[lengthSMS++]='\0';
}



void reportReports(){
appendtoSMS("TmrSince(88),",13);
appendtoSMS("TmrTogo(89),",12);
appendtoSMS("listAl(90),",11);
appendtoSMS("listAl(90),",11);
appendtoSMS("listSol(91),",12);
appendtoSMS("listPhn(92),",12);
appendtoSMS("Commands(93),",13);
appendtoSMS("PhnStatus(94),",14);
appendtoSMS("Al(95),",7);
appendtoSMS("Voltages(96),",13);
appendtoSMS("Inputs(97),",11);
appendtoSMS("Outputs(98),",12);
}
void listAl(){
appendtoSMS("rsetAl(70),",11);
appendtoSMS("arm(71),",8);
appendtoSMS("disarm(72),",11);
appendtoSMS("plyWrn1(73),",12);
appendtoSMS("plyWrn4(74),",12);
appendtoSMS("playSrn(77),",12);
}

void listSol(){
appendtoSMS("rsetSol(60),",12);
appendtoSMS("chgSolL(61),",12);
appendtoSMS("chgSolV(62),",12);
appendtoSMS("toggSol(63),",12);
appendtoSMS("shutSol(66),",12);
}

void listPhn(){
appendtoSMS("resetPhn(80),",13);
appendtoSMS("takePic(81),",12);
appendtoSMS("takesndPic(82),",15);
appendtoSMS("rsetSerial(83),",15);
appendtoSMS("chckStatus(84),",15);
appendtoSMS("prepStorge(85),",15);
appendtoSMS("chrgPhn(86),",12);
appendtoSMS("nochrgPhn(87),",14);
}




void reportCommands(){
appendtoSMS("disTmrs(55),",12);
appendtoSMS("enTmrs(56),",11);
appendtoSMS("rsetIgn(57),",12);
appendtoSMS("rsetIntrpt(58),",13);
appendtoSMS("rsetVar(59),",12);

}


void reportphnStatus(){
char state[2];
appendtoSMS("BattLev:",8); itoa(phnBattLev,state,10); appendtoSMS(state,1);
appendtoSMS(",SigLev:",8); itoa(phnSigLev,state,10); appendtoSMS(state,1);
appendtoSMS(",BattWarn:",10); itoa(phnBattWarn,state,10); appendtoSMS(state,1);
appendtoSMS(",Charger:",9); itoa(phnCharger,state,10); appendtoSMS(state,1);
appendtoSMS(",Service:",9); itoa(phnService,state,10); appendtoSMS(state,1);
return;
}
void reportAl(){

char state[1];

appendtoSMS("AlArmed:",8); 
if(AlArmed) appendtoSMS("1",1);
else appendtoSMS("0",1);

appendtoSMS(" nAls:",6); 
state[0]=intToChar(nAls);
appendtoSMS(state,1);

appendtoSMS(" nTrigs:",8); 
state[0]=intToChar(nTrigs);
appendtoSMS(state,1);

}

void reportVoltages()
{
appendtoSMS("Solar Panel:",12);
reportDouble(v_SolPan, 2);
appendtoSMS("V, Leisure Battery:",19);
reportDouble(v_BattL, 2);
appendtoSMS("V, Vehicle Battery:",19);
reportDouble(v_BattV, 2);
appendtoSMS("V",1);
}

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



void reportTimerSince(){
  appendtoSMS("since. ",7);
	appendtoSMS("tmrchkSMS: ",11); Serial.print("timercheckSMS: "); reportLong(t.since(timercheckSMS));
	appendtoSMS("tmrchkStat: ",12); Serial.print("timercheckStatus: "); reportLong(t.since(timercheckStatus));
	appendtoSMS("tmrRset: ",9); Serial.print("timerReset: "); reportLong(t.since(timerReset));
}
void reportTimerTogo(){
  appendtoSMS("togo. ",6);
	appendtoSMS("tmrchkSMS: ",11); Serial.print("timercheckSMS: "); reportLong(phnPDUInterval-t.since(timercheckSMS));
	appendtoSMS("tmrchkStat: ",12); Serial.print("timercheckStatus: "); reportLong(phnStatusInterval-t.since(timercheckStatus));
	appendtoSMS("tmrRset: ",9); Serial.print("timerReset: "); reportLong(resetInterval-t.since(timerReset));
}
void reportInputs(){
char state[2];
appendtoSMS("pirAl02: ",9); //interrupt 0 G2/G7->Volt. Reducer
itoa(digitalRead(p_pirAl),state,10); appendtoSMS(state,1);
appendtoSMS(",pirTamp03: ",12); //interrupt 1 G2/G7->Volt. Reducer
itoa(digitalRead(p_pirTamp),state,10); appendtoSMS(state,1);
appendtoSMS(",ContSw04: ",11); //grey from ROV57
itoa(digitalRead(p_ContSw),state,10); appendtoSMS(state,1);
appendtoSMS(",IsoSw18: ",10); //  interrupt5 white from ROV5
itoa(digitalRead(p_IsoSw),state,10); appendtoSMS(state,1);
appendtoSMS(",Ign19: ",8); //  interrupt4 blue A6->Volt. Reducer
itoa(digitalRead(p_Ign),state,10); appendtoSMS(state,1);
appendtoSMS(",HbrkSw20: ",11); //  interrupt3 purple from ROV5
itoa(digitalRead(p_HbrkSw),state,10); appendtoSMS(state,1);
appendtoSMS(",AlSw21: ",9); //  interrupt2 green from RUC5
itoa(digitalRead(p_AlSw),state,10); appendtoSMS(state,1);
appendtoSMS(",SolSw25: ",10);
itoa(digitalRead(p_SolSw),state,10); appendtoSMS(state,1);
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
appendtoSMS(",AlLED50: ",10);
itoa(digitalRead(p_AlLED),state,10); appendtoSMS(state,1);
return;
}




void reportLong( long val){
char state[1];
int hours, minutes, seconds, ival;
long cval;

val/=1000;

hours=int(val/3600);
cval=60*hours;
cval*=60;
minutes=int((val-cval)/60);
cval+=60*minutes;
seconds=int(val-cval);

Serial.print(hours);Serial.print(" h ");
Serial.print(minutes);Serial.print(" m ");
Serial.print(seconds);Serial.println(" s ");

ival=int(hours/10);
state[0]=intToChar(ival);
appendtoSMS(state, 1);
state[0]=intToChar(hours-10*ival);
appendtoSMS(state, 1);
appendtoSMS("h ", 2);

ival=int(minutes/10);
state[0]=intToChar(ival);
appendtoSMS(state, 1);
state[0]=intToChar(minutes-10*ival);
appendtoSMS(state, 1);
appendtoSMS("m ", 2);

/*ival=int(seconds/10);
state[0]=intToChar(ival);
appendtoSMS(state, 1);
state[0]=intToChar(seconds-10*ival);
appendtoSMS(state, 1);
appendtoSMS("s", 1);*/


return;
}


