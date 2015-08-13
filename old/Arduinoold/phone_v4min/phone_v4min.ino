#include <SimpleTimer.h>
SimpleTimer t;
//#include <SoftwareSerial.h>
//SoftwareSerial Serial3(10, 11); // RX, TX, 10 Blue, 11 Brown

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
const  int p_pirAl     = 2; //interrupt 0 G2/G7->Volt. Reducer
const  int p_pirTamp    = 3; //interrupt 1 G2/G7->Volt. Reducer
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
f3=0.01555, f4=0.01445, v_SolPanMin=4.0, v_MAX=14.0;

const unsigned int maxnAls=400, maxnTrigs=1, SrnDuration=5000, 
IgnDelay=5000, AlPowDelay=100, SolDelay=5000, interruptDelay=500, durationAledLED=2000;

const unsigned long resetInterval=30000, SolSwInterval=7000;
//resetInterval=43200000, SolSwInterval=7200000;
//resetInterval=1000*60*60*12 SolSwInterval=1000*60*60*2;
 
volatile bool IgnTriggered=true, AlSwTriggered=true,
			  IsoSwTriggered=true, HbrkSwTriggered=true,
			  pirAlTriggered=true, pirTampTriggered=true;

unsigned long IgnTime=0,sinceIgn=0;

double v_SolPan , v_BattL  , v_BattV , v_SolCont;

bool AlArmed=false,
SolSwed=false, SolSwedON=false, SolSwLast=false,
IgnON=false, AlSwedON=false, IsoSwCLOSED=false, HbrkSwDOWN=false, ContactorCLOSED=false;

int nAls=0, nTrigs,
timertoggleSolar=0,  timerReset=0;

//phone variables
const int R520m=0;
const int K700i=1;
const int phone=K700i;

const int BUSY=0;
const int FREE=1;

int cmdQueue[5]={-1, -1, -1, -1, -1}, ncmdQueue=5, smsStatus=-1;

//1000 1sec
//60000 1min
//3600000 1hr
const unsigned int phnStatusInterval=3601117,phnSMSInterval=10000, maxindex=250, 
phoneDiscardInterval=60000, maxSMS=140;

const char CIND[9]="+CIND:/0", CMGL[9]="+CMGL:/0", CMGR[9]="+CMGR:/0", myPDU[13]="7727523900/0",
CPMS[9]="+CPMS:/0", RING[7]="RING/0", CMGD[7]="CMGD/0" ;

char SMS[maxSMS], cmd[4], phnData[maxindex], character;

int whichBattIsCharged=0, counterSerial=0, nSMS=-1,lengthSMS=0,
jCIND=0,  jCMGL=0,  jCMGR=0,  jCMGD=0,  jCPMS=0,  jRING=0,
jCIND0=0, jCMGL0=0, jCMGR0=0, jCMGD0=0, jCPMS0=0, jRING0=0,
i,j, index, phnBattLev, phnSigLev, phnBattWarn, phnCharger, phnService, phnStatus,
phnSounder, newMessage, newCall, starting=-1, ringing=-1, timercheckSMS=0, timercheckStatus=0;

bool prepareStorageAlreadyCalled=false, 
isCIND=false,isCMGD=false,isCMGL=false,isCMGR=false,isCPMS=false,isCOMPLETE=false, isRING=false,
ended=false, ending=false, started=false;

unsigned long isCPMSTime=0, phoneTime=0;


void resetVariables(){
Serial.println("resetting...");
resetAl();
resetIgn();
resetSol();
resetIntrpt();
resetPhone();
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
//t.setInterval(10000, readwriteVoltages);
timercheckSMS= t.setInterval(phnSMSInterval, prepareStorage);
timercheckStatus=t.setInterval(phnStatusInterval, checkStatus);
//timerReset=t.setInterval(resetInterval, resetVariables);
//t.setInterval(20000, takeandsendPicture);
//t.setInterval(10000, sendReport(96));


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
//for (i = 10; i < 14; i++) {pinMode(i, OUTPUT); digitalWrite(i, HIGH);}
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

attachInterrupt(0, pirAlInterrupt,   CHANGE); //p_pirAl 
attachInterrupt(1, pirTampInterrupt, CHANGE); //p_pirTamp //MAY MAKE LOW LATER
attachInterrupt(2, AlSwInterrupt,    CHANGE); //p_AlSw
attachInterrupt(3, HbrkSwInterrupt,  CHANGE); //p_HbrkSw
attachInterrupt(4, IgnInterrupt,     CHANGE); //p_Ign
attachInterrupt(5, IsoSwInterrupt,   CHANGE); //p_IsoSw

pinMode(2, INPUT);    digitalWrite(2, HIGH);
pinMode(3, INPUT);    digitalWrite(3, HIGH);
pinMode(4, INPUT);    digitalWrite(4, HIGH);
pinMode(5, INPUT);    digitalWrite(5, HIGH);
pinMode(6, INPUT);    digitalWrite(6, HIGH);
pinMode(7, INPUT);    digitalWrite(7, HIGH);

}



/////////////////////////////////////////////////////////////////////////////


void loop()
{

		if (!once&&digitalRead(2)==LOW) {prepareStorage(); once=true;}
	else if (!once&&digitalRead(3)==LOW) {listSMS(); once=true;}
	else if (!once&&digitalRead(4)==LOW) {readSMS(); once=true;}
	else if (!once&&digitalRead(5)==LOW) {sendSMS(); once=true;}
	else if (!once&&digitalRead(6)==LOW) {deleteSMS(); once=true;}
        else if (digitalRead(7)==LOW) { once=false;}


while(Serial3.available())
{

character = Serial3.read();
Serial.print(character);
counterSerial++;

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

     if(jRING>3)  {resetSerial(); isRING=true; break;}
else if(jCMGD>3)  {resetSerial(); isCMGD=true; break;}

	if(started)
	{
			
		phnData[index] = character;

		if(isCMGL&&phnData[index-1]==','&&nSMS==-1)
		{
			nSMS=charToInt(phnData[index-2]); smsStatus=charToInt(phnData[index]);
			//Serial.println("isCMGLended");
			ended=true; break;
		}
		else if(isCPMS&&phnData[index]==',')
		{
			//Serial.println("isCPMSended"); 
			ended=true; break;
		}
		else if(character=='K'&&phnData[index-1]=='O'
			||index == maxindex-1) {ended=true; break;}

		phnData[++index] = '\0';
	}
	else 
	{

     if(jCIND>5)  {resetSerial(); isCIND=true; started=true;}
else if(jCMGL>5)  {resetSerial(); isCMGL=true; started=true;} 
else if(jCPMS>5)  {resetSerial(); isCPMS=true; started=true;}
else if(jCMGR>5)  {resetSerial(); isCMGR=true; started=true;}


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


}

} //SERIAL AVAILABLE

/*if(isRING){ 
Serial.print("isRING");
busyphnStatus();
resetSerial();
t.setTimeout(30000, freephnStatus);
answerCall();
}
else if(isCMGD){ 
Serial.print("isCMGD");
resetSerial();
nSMS=-1;
t.setTimeout(2000, freephnStatus);
t.setTimeout(3000, enableTimers);

}





if(phoneTime!=0&&(millis()-phoneTime>phoneDiscardInterval))
{
// Serial.print(starting);   Serial.print(started);   Serial.print(ending);   Serial.print(ended);
//Serial.println(millis()-phoneTime);
	
for( int k=0; k<index; k++)     Serial.print(phnData[k]);

Serial.println("too long wait");

resetSerial();
nSMS=-1;
freephnStatus();
enableTimers();
}





if(started&&ended) {

started=false; ended=false;

for( i=0; i<index; i++) Serial.print(phnData[i]);
Serial.println("s&&e");

if(isCPMS) {
Serial.println("isCPMS"); {Serial.print(phnData[index-2]);Serial.print(phnData[index-1]);Serial.println(phnData[index]);}
t.setTimeout(2000, freephnStatus);
if( charToInt(phnData[index-1])!=0) {disableTimers(); t.setTimeout(5000, listSMS);}

resetSerial();

}
else if(isCMGL) 
{
	Serial.print("nSMS: "); Serial.print(nSMS); Serial.print("status: "); Serial.println(smsStatus);


	if(smsStatus==0||smsStatus==1) 
	{t.setTimeout(5000, readSMS);
	resetSerial();
//freephnStatus();
t.setTimeout(2000, freephnStatus);
	}
	else {
Serial.print("Message ");Serial.print(nSMS);Serial.println("is not received"); 
t.setTimeout(5000, deleteSMS);
	}}


else if(isCMGR) {
		Serial.print("Message ");Serial.print(nSMS);Serial.println("received, checking source");

		
int jmyPDU=0, jmyPDU0;
bool ismynumber=false;

i=0;
while(i<index)
{
  if(phnData[i]==myPDU[jmyPDU])
  {
  	if(jmyPDU==0) {jmyPDU0=i; jmyPDU++;} 
  	else	
        {
  		if(jmyPDU0+jmyPDU==i) jmyPDU++;
  		else jmyPDU=0;
  	}
  Serial.print(phnData[i]);
  }
  else jmyPDU=0;
  
  if(jmyPDU==9) {ismynumber=true; break;}
  
  i++;
}

		if(ismynumber)
		{
			Serial.print("Message ");Serial.print(nSMS);Serial.println("is from me. interpreting");
			while( phnData[i]!='\n'&&i<index) {
			//  Serial.print(i);Serial.print(phnData[i]);Serial.println(index); 
			i++; }
			for( j=i-5; j<i-1; j++) cmd[j-(i-5)]=phnData[j]; 
    		execCommand(decodeCommand(cmd)); 
		}
		else
		{
			//not from me
			Serial.print("Message ");
			Serial.print(nSMS);
			Serial.println("is not from me");
			t.setTimeout(2000, deleteSMS);
		}
resetSerial();
}
else   if(isCIND) {
  	Serial.println("isCIND");
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

if(phnBattLev<3||phnBattWarn>0) digitalWrite(r_Phone, HIGH);
else if(phnBattLev>4) digitalWrite(r_Phone, LOW);


resetSerial();
}
} //started&ended




digitalWrite(r_Cont, LOW);
delay(2000);
digitalWrite(r_Cont, HIGH);

*/
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

////////////Timers/////////////////////////////////////////////////////////
void disableTimers(){
	phoneTime=millis();
	t.disable(timercheckSMS);
	t.disable(timercheckStatus);
}

void enableTimers(){
	phoneTime=0;
	t.enable(timercheckSMS);
	t.enable(timercheckStatus);
}
void checkStatus() {
//if(phnStatus==BUSY) return;
busyphnStatus();

Serial3.println("AT+CIND?");

Serial.println("checking Status");

//delay(2000);
}


void prepareStorage() {

//if(phnStatus==BUSY) return;
busyphnStatus();
Serial.println("preparing Storage");

Serial3.println("AT+CPMS=\"ME\"");

//delay(2000);
}


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

if(v_BattL>=v_MAX||v_BattV>=v_MAX||v_SolPan<v_SolPanMin) return -1;
else return 1;
}

void readwriteVoltages()
{
if(readVoltages()<0&&whichBattIsCharged!=0) shutSol(); //check often
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
void resetSol(){
SolSwLast=digitalRead(p_SolSw);
SolSwedON=(SolSwLast==LOW); 


if(SolSwedON) chargeSolL();
else shutSol();
}

void toggleSol() {

if(!SolSwedON) return;

int whichBatt=whichBattIsCharged;

shutSol();

Serial.println("waiting to read Sol");

delay(SolDelay);

if(readVoltages()<0) { writeVoltages(); return;}
writeVoltages();

Serial.println(whichBatt);
if(whichBatt==0||whichBatt==2) {chargeSolL();}
else if(whichBatt==1)                      {chargeSolV(); }


}   

void chargeSolL(){
Serial.println("will charge SolL"); 
digitalWrite(r_SolL, HIGH);
digitalWrite(r_SolV, LOW);
whichBattIsCharged=1;
}

void chargeSolV(){
Serial.println("will charge SolV"); 
digitalWrite(r_SolV, HIGH);
digitalWrite(r_SolL, LOW);
whichBattIsCharged=2;
}

void shutSol(){
Serial.println("shut down Sol");
digitalWrite(r_SolL, LOW);
digitalWrite(r_SolV, LOW);
whichBattIsCharged=0;
}










//Phone///////////////////////////////////////////////////////////////////////////////////
void resetPhone(){
freephnStatus();
resetSMS();
resetphnData();
}
void resetSMS(){
lengthSMS=0;
SMS[lengthSMS]='\0';
}

void resetSerial(){

started=false; ended=false; 


isCIND=false;isCMGL=false;isCMGR=false; isCMGD=false;isCPMS=false;isCOMPLETE=false; isRING=false;
ended=false; started=false;

counterSerial=0; 
jCIND=0;  jCMGL=0;  jCMGR=0; jCMGD=0;  jCPMS=0;  jRING=0;
jCIND0=0; jCMGL0=0; jCMGR0=0; jCMGD0=0; jCPMS0=0; jRING0=0;

resetphnData();


}
void resetphnData(){

index=0;
phnData[index]='\0';
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


void execCommand(int num)		
{
	Serial.print("Executing ");Serial.println(num);

// toggle digital pins
if(num>-1&&num<54) {
digitalWrite(num,!digitalRead(num)); 
Serial.print("Pin ");Serial.print(num); Serial.print(" is ");  Serial.println(digitalRead(num));
}
else if(num>89) {sendReport(num);  }
// functions
else if(num==88) takeandsendPicture();	
//
else if(num==70)playWrn(0);
else if(num==71)playWrn(4000);
else if(num==77) playSrn();
//
else if(num==60) toggleSol();
else if(num==61) readwriteVoltages();
else if(num==62) {resetSMS(); reportVoltages();}
else if(num==66) shutSol();


t.setTimeout(2000, deleteSMS);
}	



void freephnStatus(){
	
phnStatus=FREE;
}
void busyphnStatus(){
	return;
	
phnStatus=BUSY;
}


void answerCall() {
Serial3.println("ATA");
} 

void sendSMS() {
	if(phnStatus==BUSY) return;
	busyphnStatus();

if(phone==R520m){
phoneWake();
delay(3000);
phonepurgeSMS();
delay(5000);

Serial3.println("AT+CKPD=\"S\"");

delay(3000);

Serial3.println("AT+CKPD=\"07772259300\"");

delay(3000);

Serial3.println("AT+CKPD=\"S\"");

}
else if(phone==K700i){
phoneWake();
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

}
delay(8000);
}



void listSMS() {

	if(phnStatus==BUSY) return;
	busyphnStatus();

Serial.println("listing SMS");
Serial3.println("AT+CMGL=4");
}
void deleteSMS() {

	//if(phnStatus==BUSY) return;
	busyphnStatus();

Serial.println("deleting SMS");
Serial3.print("AT+CMGD=");
Serial3.print(nSMS);
Serial3.println(",0"); 

nSMS=0; smsStatus=-1;

}

void readSMS() {

	//if(phnStatus==BUSY) return;
	busyphnStatus();
Serial.println("reading SMS");
Serial3.print("AT+CMGR=");
Serial3.println(nSMS);
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
delay(1000);
phoneFocus();
delay(1000);
phoneCapture();
delay(3000);
phoneYes();
delay(1000);
 phoneYes();
delay(1000);
phoneDownArrow();
delay(1000);
phoneYes();
delay(1000);
phoneYes(); 
delay(1000);
phoneYes();
delay(4000);
phonegoHome();
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
	Serial3.println(SMS);
Serial.println(SMS);
	/*int i=0;
	while(i<lengthSMS){
Serial3.print(SMS[i]);
Serial.print(SMS[i]);
i++;
delay(5);
	}
	Serial3.println();
Serial.println();*/
}


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

case 93: {reportCommands();break;} 
case 94: {reportPins();break;} 
case 95: {reportAl();break;} 
case 96: {readwriteVoltages(); reportVoltages(); break;} 
case 97: {reportVariables();break;} 
case 98: {reportDigital();break;}
case 99: {reportReports();break;} 

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



void reportReports(){
appendtoSMS("Commands(93),",13);
appendtoSMS("Pins(94),",9);
appendtoSMS("Al(95),",7);
appendtoSMS("Voltages(96),",13);
appendtoSMS("Variables(97,)",14);
appendtoSMS("Digital(98),",12);
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

void reportCommands(){
appendtoSMS("takeandsendPicture(88),",23);	
appendtoSMS("playWrn(0,70),",14);
appendtoSMS("playWrn(4000,71),",17);
appendtoSMS("playSrn(77),",12);
appendtoSMS("chargeSolL(60),",15);
appendtoSMS("chargeSolV(61),",15);
appendtoSMS("shutSol(66),",11);

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
void reportVariables(){};
