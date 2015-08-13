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
//const  int p_LED3       = 46;
//const  int p_usrEcho    = 47; //G6->ROG7
//const  int p_LED2       = 48;
//49x, spare at RUC
const  int p_AlLED    = 50;
//const  int p_C7        = 51; //RUC7
//52x, 53x

//voltage multipliers
const double f0=0.02600,f1=0.01532, f2=0.01580,
f3=0.01555, f4=0.01445, v_SolPanMin=4.0, v_CRI=13.5, v_MAX=14.0;

const unsigned int maxnAls=4, maxnTrigs=1, SrnDuration=5000, 
IgnDelay=5000, AlPowDelay=100, SolDelay=5000, interruptDelay=500, durationAledLED=2000;

int nAls=0, nTrigs=0,
timertoggleSolar=0,  timerVoltage=0,  timerReset=0;

const unsigned long 
//readVoltageInterval=10000, resetInterval=600000, SolSwInterval=60000;
readVoltageInterval=60000, resetInterval=43200000, SolSwInterval=7200000;
//resetInterval=1000*60*60*12 SolSwInterval=1000*60*60*2;
 
volatile bool IgnTriggered=true, AlSwTriggered=true,
			  IsoSwTriggered=true, HbrkSwTriggered=true,
			  pirAlTriggered=true, pirTampTriggered=true;

unsigned long IgnTime=0,sinceIgn=0;

double v_SolPan , v_BattL  , v_BattV , v_SolCont;

bool AlArmed=false,
SolSwed=false, SolSwedON=false, SolSwLast=false,
IgnON=false, AlSwedON=false, IsoSwCLOSED=false, HbrkSwDOWN=false, ContactorCLOSED=false;



//phone variables
const int R520m=0;
const int K700i=1;
const int phone=K700i;

const int BUSY=0;
const int FREE=1;

const unsigned int phoneInterval=10000, maxindex=250, 
phoneDiscardInterval=5000, maxSMS=140;

const char CIND[9]="+CIND:/0", CMGL[9]="CMGL=4/0", myPDU[13]="7727523900/0",
CPMS[9]="+CPMS:/0";

char SMS[maxSMS], cmd[4], phnData[maxindex], character;

int whichBattIsCharged=0, counterSerial=0, nSMS=0,lengthSMS=0,jCIND, jCMGL, jCPMS, jCOMPLETE,
i,j, index, phnBattLev, phnSigLev, phnBattWarn, phnCharger, phnService, phnStatus,
phnSounder, newMessage, newCall, starting=-1, ringing=-1, timercheckSMS=0, timercheckStatus=0;

bool prepareStorageAlreadyCalled=false, isCIND=false,isCMGL=false,isCPMS=false,isCOMPLETE=false, isRINGING=false,
ended=false, ending=false, started=false;

unsigned long isCPMSTime=0, phoneTime=0;


void resetVariables(){
Serial.println("resetting...........................");
delay(10000);
resetAl();
resetIgn();
resetSol();
resetIntrpt();
//resetSMS();
//resetphnData();

}




void resetIgn(){
IgnTime=0; 
sinceIgn=0;
}


void setup()
{
Serial.begin(9600); //initialize serial communication
Serial3.begin(9600); //initialize serial communication

//timertoggleSolar=t.setInterval(SolSwInterval, toggleSol);
timerVoltage=t.setInterval(readVoltageInterval, readwriteVoltages);
//timercheckSMS= t.setInterval(phoneInterval, checkSMS);
//timercheckStatus=t.setInterval(phoneInterval, checkStatus);
timerReset=t.setInterval(resetInterval, resetVariables);
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

attachInterrupt(0, pirTampInterrupt, CHANGE); //p_pirAl 
attachInterrupt(1, pirAlInterrupt,   CHANGE); //p_pirTamp //MAY MAKE LOW LATER
attachInterrupt(2, AlSwInterrupt,    CHANGE); //p_AlSw
attachInterrupt(3, HbrkSwInterrupt,  CHANGE); //p_HbrkSw
attachInterrupt(4, IgnInterrupt,     CHANGE); //p_Ign
attachInterrupt(5, IsoSwInterrupt,   CHANGE); //p_IsoSw

resetVariables();
//phoneInitialize();
//prepareStorage();
//initializeSrn();

}



/////////////////////////////////////////////////////////////////////////////


void loop()
{
t.run();
//doPhoneThings();

/////////////////////////////////////////////////////////////////////////////
if(AlArmed){
if(pirAlTriggered){pirAlTriggered=false; 
	delay(interruptDelay); 
	if((digitalRead(p_pirTamp)==LOW)) nTrigs=maxnTrigs+1;
	if((digitalRead(p_pirAl)==LOW)) nTrigs++;
	Serial.print(" pirAlTriggered nTrigs:"); Serial.println(nTrigs);
}
if(pirTampTriggered){pirTampTriggered=false; 
	delay(interruptDelay); 
	if((digitalRead(p_pirTamp)==LOW)) nTrigs=maxnTrigs+1;
	if((digitalRead(p_pirAl)==LOW)) nTrigs++;
	Serial.print(" pirTampTriggered nTrigs:"); Serial.println(nTrigs);
}
if(HbrkSwTriggered){
    HbrkSwTriggered=false; 
    delay(interruptDelay); 
    HbrkSwDOWN=(digitalRead(p_HbrkSw)==LOW);
	if(HbrkSwDOWN)  {nTrigs=maxnTrigs+1; Serial.print(" HbrkSwDOWN nTrigs:"); Serial.println(nTrigs);}
	  else Serial.println("HbrkSwUP");
}
if(IsoSwTriggered) {IsoSwTriggered=false;  delay(interruptDelay); IsoSwCLOSED=(digitalRead(p_IsoSw)==LOW);
	if(IsoSwCLOSED)  { nTrigs=maxnTrigs+1; Serial.print("IsoSwCLOSED nTrigs:"); Serial.println(nTrigs);}
	  else  Serial.println("IsoSwOPEN");
}
}


if(AlSwTriggered)  {AlSwTriggered=false; delay(interruptDelay); AlSwedON=(digitalRead(p_AlSw)==LOW);
  if(AlSwedON)
  {
    Serial.println("AlSwedON");
    AlArmed=true;
    nAls=0;
    nTrigs=0;
    digitalWrite(r_AlPow, LOW);
  }
  else 
  {
  AlArmed=false; 
  digitalWrite(r_AlPow, HIGH);
  Serial.println("AlSwTriggeredOFF");
  } //AlSwSwedON
} //AlSwTriggered
if(IgnTriggered) {IgnTriggered=false;delay(interruptDelay); IgnON=(IsoSwCLOSED&&digitalRead(p_Ign)==HIGH); 
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
}

//EXECUTE SOLAR
SolSwed=(digitalRead(p_SolSw)!=SolSwLast); 
if(SolSwed) { delay(interruptDelay);  resetSol();}

//EXECUTE Al
if(AlArmed&&nAls<maxnAls&&nTrigs>maxnTrigs) {playSrn(); Serial.print(" nAls:"); Serial.print(nAls);}

//EXECUTE IGN
ContactorCLOSED=(digitalRead(p_ContSw)==LOW);
if(IgnON&&IgnTime>0&&!ContactorCLOSED)
{
	sinceIgn=millis()-IgnTime;
		if(sinceIgn>IgnDelay) {
		  Serial.println("ContactorCLOSED");
		digitalWrite(r_Cont, HIGH);
		}
}


} //loop




///////interrupt handlers////////////////////////////////////////////////////////////////////////////////
void resetIntrpt(){
IgnTriggered=true; AlSwTriggered=true; IsoSwTriggered=true; HbrkSwTriggered=true; pirAlTriggered=true; pirTampTriggered=true;
IgnON=false; AlSwedON=false; IsoSwCLOSED=false; HbrkSwDOWN=false;
}


void AlSwInterrupt(){AlSwTriggered=true;}

void HbrkSwInterrupt(){HbrkSwTriggered=true;}

void IsoSwInterrupt(){IsoSwTriggered=true;}

void IgnInterrupt(){IgnTriggered=true;}



void pirAlInterrupt(){pirAlTriggered=true; }

void pirTampInterrupt(){pirTampTriggered=true;}

//Alarm///////////////////////////////////////////////////////////////////////////////
void resetAl(){
nAls=0;
nTrigs=0;
}
void lightLED(int pin, int times, long millIsoSwn, long millIsoSwff) {

i=0;

while(i<times&&i<10){
digitalWrite(pin, HIGH);   
delay(millIsoSwn);  
digitalWrite(pin, LOW);
delay(millIsoSwff);
i++;
}
}



void lightLED(int pin, int times) {

i=0;

while(i<times&&i<10){
digitalWrite(pin, HIGH);   
delay(100);  
digitalWrite(pin, LOW);
delay(100);
i++;
}
}


void playSrn() {

nAls++;
srnOnLow() ; 
delay(500);
srnOnHigh();
delay(2*SrnDuration);
srnOnLow() ; 
delay(500);
srnOnHigh();
delay(SrnDuration);
srnOffLow() ; 
delay(500);
srnOffHigh();

nTrigs=0;
}

void initializeSrn(){
resetAl();
srnOnLow() ; 
srnOffLow() ;
}

void playWrn(int WrnDuration) {

               srnOnLow();   
t.setTimeout(500,                srnOnHigh);
t.setTimeout(WrnDuration+1000, srnOffLow) ;
t.setTimeout(WrnDuration+1500, srnOffHigh);

}

void srnOnHigh(){
digitalWrite(r_SrnOn, HIGH);
}

void srnOnLow(){
digitalWrite(r_SrnOn, LOW);
}

void srnOffHigh(){
digitalWrite(r_SrnOff, HIGH);
}

void srnOffLow(){
digitalWrite(r_SrnOff, LOW);
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

if(v_BattL>=v_MAX||v_BattV>=v_MAX) {Serial.println("v_BattL>=v_MAX||v_BattV>=v_MAX||v_SolPan<v_SolPanMin"); return -1;}
//else if(v_BattL>=v_CRI||v_BattV>=v_CRI) {Serial.println("v_BattL>=v_CRI||v_BattV>=v_CRI"); return 0;}
else return 1;
}

void readwriteVoltages() //check often
{
	if(readVoltages()<0){ shutSol();} 
	else resetSol();
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

void resetSol() {
SolSwLast=digitalRead(p_SolSw);
SolSwedON=(SolSwLast==LOW); 

if(!SolSwedON){ Serial.println("!SolSwedON"); shutSol(); return;}
else{
	Serial.println("will charge SolL"); 
	digitalWrite(r_SolL, HIGH);
	digitalWrite(r_SolV, LOW);
	whichBattIsCharged=1;
}

}

void toggleSol() {
SolSwLast=digitalRead(p_SolSw);
SolSwedON=(SolSwLast==LOW); 


int whichBatt=whichBattIsCharged;

shutSol();

if(!SolSwedON){ Serial.println("!SolSwedON"); return;}

Serial.println("waiting to read Sol");

delay(SolDelay);

if(readVoltages()<0) { writeVoltages(); return;}
writeVoltages();

Serial.println(whichBatt);
if(whichBatt==0||whichBatt==2) {
	Serial.println("will charge SolL"); 
	digitalWrite(r_SolL, HIGH);
	digitalWrite(r_SolV, LOW);
	whichBattIsCharged=1;
}
else if(whichBatt==1) {
	Serial.println("will charge SolV"); 
	digitalWrite(r_SolV, HIGH);
	digitalWrite(r_SolL, LOW);
	whichBattIsCharged=2;
}
else shutSol();

}   

void shutSol(){
Serial.println("shut down Sol");
digitalWrite(r_SolL, LOW);
digitalWrite(r_SolV, LOW);
whichBattIsCharged=0;
}










//Phone///////////////////////////////////////////////////////////////////////////////////
int charToInt(char c){

if( c =='0') return 0;
else if( c =='1') return 1;
else if( c =='2') return 2;
else if( c =='3') return 3;
else if( c =='4') return 4;
else if( c =='5') return 5;
else if( c =='6') return 6;
else if( c =='7') return 7;
else if( c =='8') return 8;
else if( c =='9') return 9;
else		return -1;


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

int decodexeCommand(char* cmd)		
{
Serial.print(cmd);Serial.println('x');


int num=charToInt(cmd[1]);
Serial.println(num);

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



if(num<5||num==14||num==15||(num>17&&num<22)) {Serial.println("protected pin"); return num;}

// toggle digital pins
if(num<54) {digitalWrite(num,!digitalRead(num)); 
Serial.print("Pin ");Serial.print(num); Serial.print(" is ");  Serial.println(digitalRead(num)); return num;}



if(num>89) {sendReport(num);  }

// functions
else if(num==88) takeandsendPicture();	
//
else if(num==70)playWrn(0);
else if(num==71)playWrn(4000);
else if(num==77) playSrn();
//
else if(num==60) toggleSol();
else if(num==66) shutSol();

return num;

}	

void freephnStatus(){
phnStatus=FREE;
}

void reportCommands(){
appendtoSMS("takeandsendPicture(88),",23);	
appendtoSMS("playWrn(0,70),",14);
appendtoSMS("playWrn(4000,71),",17);
appendtoSMS("playSrn(77),",12);
appendtoSMS("chargeSolL(60),",15);
appendtoSMS("chargeSolV(61),",15);
appendtoSMS("shutSol(66),",11);

}

void reportAll(){
reportAl();
reportVoltages();
reportVariables();
reportDigital();
}
void reportAl(){

char state[2];

appendtoSMS("AlArmed:",13); 
itoa(AlArmed,state,10); appendtoSMS(state,1);

appendtoSMS(" nAls:",6); 
itoa(nAls,state,10); appendtoSMS(state,1);

appendtoSMS(" nTrigs:",11); 
itoa(nTrigs,state,10); appendtoSMS(state,1);

}
void reportVariables(){};
void answerCall() {
Serial3.println("ATA");
} 

void sendSMS() {

t.setTimeout(20000, freephnStatus);
if(phone==R520m){

phonepurgeSMS();
delay(3000);

Serial1.println("AT+CKPD=\"S\"");

delay(3000);

Serial1.println("AT+CKPD=\"07772259300\"");

delay(3000);

Serial1.println("AT+CKPD=\"S\"");

}
else if(phone==K700i){
phoneWake();
t.setTimeout(2000, phonepurgeSMS);
t.setTimeout(6000, phoneYes);
t.setTimeout(8000, phoneYes);
t.setTimeout(10000, phoneYes);
t.setTimeout(12000, phoneYes);
t.setTimeout(14000, phoneYes);

}
}



void deleteSMS() {
Serial.println("deleting SMS");
Serial3.print("AT+CMGD=");
Serial3.print(nSMS);
Serial3.println(",0"); 

}

void readSMS() {
Serial.println("reading SMS");
Serial3.println("AT+CMGL=4");
}


void listentoCall() {
Serial3.println("AT+CPAS");
}

void checkStatus() {

Serial3.println("AT+CIND?");

Serial.println("checking Status");
}


void checkSMS(){

prepareStorage();


}


void prepareStorage() {
//Serial.println("preparing Storage");

Serial3.println("AT+CPMS=\"ME\"");
}

void takeandsendPicture()
{

phnStatus=BUSY;
t.setTimeout(20000, freephnStatus);

if(phone!=K700i) return;
phoneWake();
delay(2000);
phoneKeyboardMode();


t.setTimeout(1000, phoneCameraMode);
t.setTimeout(2000, phoneFocus);
t.setTimeout(3000, phoneCapture);
t.setTimeout(6000, phoneYes);
t.setTimeout(7000, phoneYes);
t.setTimeout(8000, phoneDownArrow);
t.setTimeout(9000, phoneYes);
t.setTimeout(10000, phoneYes);   
t.setTimeout(11000, phoneYes);
t.setTimeout(15000, phonegoHome);

}



void phoneInitialize(){
phoneWake();
delay(2000);
phoneHello();
delay(2000);
if (phone==K700i) phoneKeyboardMode();

resetSMS();

}

void phonegoHome() {Serial3.println("at+ckpd=\":R\",10");}
void phoneWake(){Serial3.println("at+ckpd=\"P\"");}
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
Serial.println(SMS);
Serial3.println(SMS);}


void sendReport(int nReport) {



resetSMS();
appendtoSMS("AT*EAPP=0,0,\"",13);


switchReport(nReport);
appendtoSMS("\"",1);

sendSMS();
return;
}

void switchReport(int nReport){
switch(nReport)
{
//reports
case 90: {reportReports();break;} 
case 93: {reportCommands();break;} 
case 94: {reportPins();break;} 
case 95: {reportAl();break;} 
case 96: {readVoltages(); reportVoltages();break;} 
case 97: {reportVariables();break;} 
case 98: {reportDigital();break;} 
case 99: {reportAll();break;} 

default: Serial3.print("No such report. Txt 90 for a list of reports ");break;
}
}


void appendtoSMS(char *partSMS, int len){
if(lengthSMS+len>=maxSMS) return;
if(lengthSMS>0)lengthSMS--;
strcpy(&SMS[lengthSMS], partSMS);
lengthSMS+=len;
SMS[lengthSMS++]='\0';
}

void resetSMS(){
lengthSMS=0;
SMS[lengthSMS]='\0';
}

void resetphnData(){
index=0;
phnData[index]='\0';
}

void reportReports(){
appendtoSMS("Commands(93),",13);
appendtoSMS("Pins(94),",9);
appendtoSMS("Al(95),",7);
appendtoSMS("Voltages(96),",13);
appendtoSMS("Variables(97,)",14);
appendtoSMS("Digital(98),",12);
appendtoSMS("All(99)",7);
}

void reportDigital(){
char state[2];
//const  int p_phone2Tx   = 0; //Rx0 B3  -> UTB3
//const  int p_phone2Rx   = 1; //Tx0 B8  -> //UTB8
//interrupts
appendtoSMS("02:",3); //interrupt 0 G2/G7->Volt. Reducer
itoa(digitalRead(p_pirAl),state,10); appendtoSMS(state,1);
appendtoSMS(", 03:",5); //interrupt 1 G2/G7->Volt. Reducer
itoa(digitalRead(p_pirTamp),state,10); appendtoSMS(state,1);
appendtoSMS(", 04:",5); //grey from ROV57
itoa(digitalRead(p_ContSw),state,10); appendtoSMS(state,1);

//uln2003-2
appendtoSMS(", 05:",5); //ULN2-3
itoa(digitalRead(r_Phone),state,10); appendtoSMS(state,1);
appendtoSMS(", 06:",5); //ULN2-2
itoa(digitalRead(r_Sink),state,10); appendtoSMS(state,1);
appendtoSMS(", 07:",5); //ULN2-1
itoa(digitalRead(r_Cont),state,10); appendtoSMS(state,1);

//Block 2:
//uln2003-3
appendtoSMS(", 08:",5); //ULN3-4
itoa(digitalRead(r_SolL),state,10); appendtoSMS(state,1);
appendtoSMS(", 09:",5); //ULN3-3
itoa(digitalRead(r_SolV),state,10); appendtoSMS(state,1);
//rel5v
appendtoSMS(", 10:",5);
itoa(digitalRead(r_SrnOn),state,10); appendtoSMS(state,1);
appendtoSMS(", 11:",5);
itoa(digitalRead(r_SrnOff),state,10); appendtoSMS(state,1);
appendtoSMS(", 12:",5);
itoa(digitalRead(r_DH11),state,10); appendtoSMS(state,1);
//const  int r_spare5v = 13;

// Block 3:
//const  int p_phoneRx(14); //G11 ->ROG1
//const  int p_phoneTx     = 15; //G10 ->ROG5
//const  int p_ustapEcho   = 16; //G4 ->ROG2
//const  int p_ustapTrigger= 17; //G3 ->ROG6
//interrupts
appendtoSMS(", 18:",5); //  interrupt5 white from ROV5
itoa(digitalRead(p_IsoSw),state,10); appendtoSMS(state,1);
appendtoSMS(", 19:",5); //  interrupt4 blue A6->Volt. Reducer
itoa(digitalRead(p_Ign),state,10); appendtoSMS(state,1);
appendtoSMS(", 20:",5); //  interrupt3 purple from ROV5
itoa(digitalRead(p_HbrkSw),state,10); appendtoSMS(state,1);
appendtoSMS(", 21:",5); //  interrupt2 green from RUC5
itoa(digitalRead(p_AlSw),state,10); appendtoSMS(state,1);

// Block 4:
//const  int p_B6  = 23; // UTB6
appendtoSMS(", 25:",5);
itoa(digitalRead(p_SolSw),state,10); appendtoSMS(state,1);


//Empty
//22x, 24x, ..., 28x
// 30x->39x

//Block 5:
//uln2003-1
appendtoSMS(", 40:",5);
itoa(digitalRead(r_AlPow),state,10); appendtoSMS(state,1);
//const  int r_Spare    = 41; //ULN1-2
//const  int p_LED5       = 42;
//const  int r_Spare    = 43; //ULN1-3
//const  int p_LED4       = 44;
//const  int p_usrTrigger = 45; // G8->ROG3
//const  int p_LED3       = 46;
//const  int p_usrEcho    = 47; //G6->ROG7
//const  int p_LED2       = 48;
//49x, spare at RUC
appendtoSMS(", 50:",5);
itoa(digitalRead(p_AlLED),state,10); appendtoSMS(state,1);


}


void reportPins(){


//const  int p_phone2Tx   = 0; //Rx0 B3  -> UTB3
//const  int p_phone2Rx   = 1; //Tx0 B8  -> //UTB8
//interrupts
appendtoSMS("p_pirAl02_I",11); //interrupt 0 G2/G7->Volt. Reducer
appendtoSMS(",p_pirTamp03_I",14); //interrupt 1 G2/G7->Volt. Reducer
appendtoSMS(",p_ContSw04_I",13); //grey from ROV57

//uln2003-2
appendtoSMS(",r_Phone05_O",12); //ULN2-3
appendtoSMS(",r_Sink06_O",11); //ULN2-2
appendtoSMS(",r_Cont07_O",11); //ULN2-1

//Block 2:
//uln2003-3
appendtoSMS(",r_SolL08_O",11); //ULN3-4
appendtoSMS(",r_SolV09_O",11); //ULN3-3
//rel5v
appendtoSMS(",r_SrnOn10_O",12);
appendtoSMS(",r_SrnOff11_O",13);
appendtoSMS(",r_DH1112_O",11);

//const  int r_spare5v = 13;

// Block 3:
//const  int p_phoneRx(14); //G11 ->ROG1
//const  int p_phoneTx     = 15; //G10 ->ROG5
//const  int p_ustapEcho   = 16; //G4 ->ROG2
//const  int p_ustapTrigger= 17; //G3 ->ROG6
//interrupts
appendtoSMS(",p_IsoSw18_I",12); //  interrupt5 white from ROV5
appendtoSMS(",p_Ign19_I",10); //  interrupt4 blue A6->Volt. Reducer
appendtoSMS(",p_HbrkSw20_I",13); //  interrupt3 purple from ROV5
appendtoSMS(",p_AlSw21_I",11); //  interrupt2 green from RUC5

//22x, 23x

// Block 4:
//const  int p_B6  = 24; // UTB6
appendtoSMS(",p_SolSw25_I",12);

//Empty

// 26x->39x

//Block 5:
//uln2003-1
appendtoSMS(",r_AlPow40_O",12);
//const  int r_Spare    = 41; //ULN1-2
//const  int p_LED5       = 42;
//const  int r_Spare    = 43; //ULN1-3
//const  int p_LED4       = 44;
//const  int p_usrTrigger = 45; // G8->ROG3
//const  int p_LED3       = 46;
//const  int p_usrEcho    = 47; //G6->ROG7
//const  int p_LED2       = 48;
//49x, spare at RUC
appendtoSMS(",p_AlLED50_O",12);

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
char state[10];
int i=0, ndigits=1, digits=int(val);
double precision=1.;
double dobval=val;

while(int(abs(dobval))>0) {ndigits++; dobval/=10.;}
while(i<nfrac) {i++; precision*=10.;}

unsigned int frac;
if(val >= 0)
frac = int((val - double(digits)) * abs(precision));
else
frac = int((double(digits)- val ) * abs(precision));

if (frac==0) nfrac=1;
/*Serial.print(val);
Serial.print(",");
Serial.print(ndigits);
Serial.print(",");
Serial.println(precision);*/


itoa(digits, state,10); appendtoSMS(state,ndigits);
appendtoSMS(".",1);
utoa(frac, state, 10); appendtoSMS(state,nfrac);
}

void deleteandreadnextSMS(){
resetphnData(); 
prepareStorage();
delay(2000);
deleteSMS();
delay(2000);
readSMS();}





void doPhoneThings()
{
while(Serial3.available())
{
//phoneTime=0;

character = Serial3.read();
counterSerial++;
//Serial.print(character);
//Serial.print(',');
//Serial.print(counterSerial);
//Serial.print(',');
//Serial.println(ringing);



//start of packet// Serial.println(" started");

if(character=='R')  {ringing=counterSerial;}
if(character=='I'&& ringing>0)  ringing++; 
if(character=='N'&& ringing>0)  ringing++;
if(character=='G'&& ringing>0) isRINGING=true;



if(!started){
if(character=='A')  {starting=counterSerial;}
if(character=='T'&& starting>0)  starting++; 
if(character=='+'&& starting>0) 
{
started=true; 
phoneTime=millis();
resetphnData();

//Serial.println(" started");
}
if(starting!=counterSerial) {starting=-1;}
if(ringing!=counterSerial) {ringing=-1;}
}



if(started&&index < maxindex-1)
{
phnData[index++] = character;
phnData[index] = '\0';
}


//end of packet
if(started&&character=='O'&&!ended) {
ending=true;
//Serial.println(" ending");
}
if(started&&character=='K'&&ending||index == maxindex-1) {
ended=true; 
ending=false; 
counterSerial=0;
//Serial.print(index);Serial.println(" ended");
break;
}


}

if(started&&(millis()-phoneTime>phoneDiscardInterval)&&!ended)
{
// Serial.print(starting);   Serial.print(started);   Serial.print(ending);   Serial.print(ended);
//Serial.println(millis()-phoneTime);
for( int k=0; k<index; k++)     Serial.print(phnData[k]);
Serial3.flush();  resetphnData(); started=false; ended=false; starting=false; ending=false; 

}

if(isRINGING){ 
Serial.print("isRiNGING");
phnStatus=BUSY;
t.setTimeout(30000, freephnStatus);
answerCall();
isRINGING=false;

}


if(started&&ended) {
Serial.println("started&&ended");
started=false; ended=false;

for( i=0; i<index; i++) Serial.print(phnData[i]);
Serial.println();

if(index>9) 
{      
i=0;
while(i<index-9&&!isCIND&&!isCMGL&&!isCPMS)
{
jCIND=0; jCMGL=0; jCPMS=0;
for( j=0; j<9; j++)
{
if(phnData[i+j]==CIND[j]) {jCIND++; if(jCIND==6) {isCIND=true; Serial.println("isCIND"); break;}}
if(phnData[i+j]==CMGL[j]) {jCMGL++; if(jCMGL==6) {isCMGL=true; Serial.println("isCMGL"); break;}}
if(phnData[i+j]==CPMS[j]) {jCPMS++; if(jCPMS==6) {isCPMS=true; Serial.println("isCPMS"); break;}}

}
//Serial.print(jCIND);   Serial.print(jCMGL);   Serial.println(jCPMS); 
i++;

}
} //index>10isDe
}//started and ended


if(isCPMS) {resetphnData(); t.setTimeout(2000, readSMS);  isCPMS=false;}
else if(isCMGL) {

i=jCMGL;
while( phnData[i]!=':'&&i<index) i++;

if(i==index) {Serial.print("isCOMPLETE"); isCMGL=false; }
else{
i=i+2;

nSMS=charToInt(phnData[i]);
i=i+2;

Serial.print(nSMS);Serial.print("isCMGL");Serial.println(phnData[i]);

if(phnData[i]=='2'||phnData[i]=='3') {
Serial.print("Message ");Serial.print(nSMS);Serial.println("is not received");

t.setTimeout(2000,deleteandreadnextSMS);   

}

else if(phnData[i]=='0'||phnData[i]=='1'){
while( phnData[i]!='\n'&&i<index) {
// Serial.print(i);Serial.print(phnData[i]);Serial.println(index); 
i++; }
i++;

bool ismynumber=false;
while(i<index-13&&j<9) 
{
j=0;
while(j<10&&phnData[i+j]==myPDU[j]) { 
//Serial.print("myPDU"); Serial.print(j);Serial.println(phnData[i+j]);
if(j==9) {ismynumber=true; break;}
j++;
}
if(ismynumber) break; 
i++;
}


if(ismynumber){
Serial.print("Message ");Serial.print(nSMS);Serial.println("is being interpreted");
while( phnData[i]!='\n'&&i<index) {
//  Serial.print(i);Serial.print(phnData[i]);Serial.println(index); 
i++; }
for( j=i-5; j<i-1; j++) cmd[j-(i-5)]=phnData[j];


Serial.println(decodexeCommand(cmd)); 


}
else{
//not from me,delete
Serial.print("Message ");
Serial.print(nSMS);
Serial.println("is not from me");
t.setTimeout(2000,deleteandreadnextSMS);

}
}
else Serial.println(phnData[i]);
}



isCMGL=false; 
}
else   if(isCIND) {
i=jCIND;
while( phnData[i]!=','&&i<index) i++;
i--;

phnBattLev=atoi(&phnData[i]);
phnSigLev=atoi(&phnData[i+2]);
phnBattWarn=atoi(&phnData[i+4]);
phnCharger=atoi(&phnData[i+6]);
phnService=atoi(&phnData[i+8]);
phnSounder=atoi(&phnData[i+10]);
newMessage=atoi(&phnData[i+12]);
newCall=atoi(&phnData[i+14]);
Serial.print("phnBattLev:"); Serial.print(phnBattLev);
Serial.print(" phnSigLev:"); Serial.print(phnSigLev);
Serial.print(" phnBattWarn:"); Serial.print(phnBattWarn);
Serial.print(" phnCharger:"); Serial.println(phnCharger);
Serial.print(" phnService:"); Serial.print(phnService);
Serial.print(" phnSounder:"); Serial.print(phnSounder);
Serial.print(" newMessage:"); Serial.print(newMessage);
Serial.print(" newCall:"); Serial.println(newCall);
isCIND=false;
}




if(phnBattLev<3||phnBattWarn>0) digitalWrite(r_DH11, LOW);
else if(phnBattLev>4) digitalWrite(r_DH11, HIGH);
}
