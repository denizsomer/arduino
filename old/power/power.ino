#include <SimpleTimer.h>
SimpleTimer t;

 bool prnt=true;
 //bool prnt=false;

//voltages
const  int pv_ShuntM     = A0; 
const  int pv_ShuntP     = A1; 
const  int pv_BattL1    = A7; 
const  int pv_BattL2    = A6; 
const  int pv_BattV     = A5; 


//uln2003-1
const  int r_Sol1        = 4;
const  int r_Sol2        = 5;
const  int r_SW80  = 6;
const  int r_SW180    = 7; 

//uln2003-2
const  int r_LED1        = 8;
const  int r_LED2        = 9;
//const  int r_SW80  = 10; //SPARE
//const  int r_SW180    = 11; //SPARE
 
//interrupts

const  int p_BatL1Sw  = 2; //  interrupt0
const  int p_BatL2Sw  = 3; //  interrupt1
//const  int p_IgnSw     = 18; //  interrupt5 //SPARE
const  int p_IgnSw     = 19; //  interrupt4 
const  int p_ContSw  = 20; //  interrupt3
const  int p_SolSw   = 21; //  interrupt2


//voltage multipliers     
const double           f0=5.0/1023., f1=0.01506, f3=0.01527,
 f2=0.01580,f4=0.01445, v_ShuntMin=4.0, v_CRI=13.5, v_MAX=14.5, v_MIN=11.5;

const unsigned int SolDelay=5000, interruptDelay=200;

const char Source[5][6]={"batL1","batL2","SolSw" ,"ConSw","IgnSw"};


boolean s_ContSw,  s_SolSw, s_BatL1Sw, s_BatL2Sw, s_IgnSw;

int timerReset, timerContactor=-1,intSrc[5];

const unsigned long IgnDelay=300000,resetInterval=43200000, 
debounceDelay=10, readVoltageInterval=5000;

volatile int itrptSrc=-1;
 
unsigned long IgnTime,sinceIgn;

double v_ShuntP, v_ShuntM , v_BattL1, v_BattL2  , v_BattV , v_SolCont;

bool ContactorCLOSED;

//1000 1sec
//60000 1min
//3600000 1hr

int whichBattIsCharged;



void setup()
{
Serial.begin(9600); //initialize serial communication

initializePins();
resetAll();

}


void initializePins(){
//initialize outputs
//uln2003-1
pinMode(r_SW180, OUTPUT);   digitalWrite(r_SW180, LOW);
pinMode(r_SW80, OUTPUT); digitalWrite(r_SW80, LOW);
pinMode(r_Sol1, OUTPUT);       digitalWrite(r_Sol1, LOW);
pinMode(r_Sol2, OUTPUT);       digitalWrite(r_Sol2, LOW);
//uln2003-2
pinMode(r_LED1, OUTPUT);       digitalWrite(r_LED1, LOW);
pinMode(r_LED2, OUTPUT);       digitalWrite(r_LED2, LOW);

pinMode(p_IgnSw, INPUT);         digitalWrite(p_IgnSw, HIGH); 
pinMode(p_ContSw, INPUT);      digitalWrite(p_ContSw, HIGH);
pinMode(p_BatL1Sw, INPUT);      digitalWrite(p_BatL1Sw, HIGH); 
pinMode(p_BatL2Sw, INPUT);      digitalWrite(p_BatL2Sw, HIGH); 
pinMode(p_SolSw, INPUT);       digitalWrite(p_SolSw, HIGH); 

pinMode(pv_ShuntM, INPUT); 
pinMode(pv_ShuntP, INPUT);
pinMode(pv_BattL1, INPUT);
pinMode(pv_BattL2, INPUT);
pinMode(pv_BattV, INPUT);
}


///////////resets///////////////////////////////////////////////////////////////////
void resetAll(){
if(prnt) Serial.println("resetting...........................");
resetTimers();
resetIgn();
attachInterrupts();
resetInterrupts();
}


void resetTimers(){
t.reset();

t.setInterval(readVoltageInterval, readwriteVoltages);

timerReset=t.setInterval(resetInterval, resetAll);

}
void resetIgn(){
IgnTime=0; 
sinceIgn=0;
}

void resetCont(){
ContactorCLOSED=false;
}




void resetInterrupts(){
s_SolSw=digitalRead(p_SolSw);
s_ContSw=digitalRead(p_ContSw);
s_BatL1Sw=digitalRead(p_BatL1Sw);
s_BatL2Sw=digitalRead(p_BatL2Sw);
s_IgnSw=digitalRead(p_IgnSw);
}







///////loop////////////////////////////////////////////////////////////////////////////////
void loop()
{
t.run();

if(itrptSrc>-1) 
{

	detachInterrupts();

	int nSrc=whichPinInterrupted();
	
	for(int iSrc=0; iSrc < nSrc; iSrc++)  manageInterrupts(intSrc[iSrc]);

	itrptSrc=-1;

	attachInterrupts();
}


} //loop





///////interrupt handlers////////////////////////////////////////////////////////////////////////////////
void manageInterrupts(int iSrc){

	if(prnt) {Serial.print(iSrc); Serial.print(Source[iSrc]); Serial.println("...");}
	switch(iSrc){
			case 0:
				if(s_BatL1Sw==LOW)  
				{
					if(prnt) Serial.println("BatL1TurnedON");
                                        digitalWrite(r_Sol1, HIGH);
				}
				else
				{
					if(prnt) Serial.println("BatL1TurnedOFF");
                                        digitalWrite(r_Sol1, LOW);
				}
			break;

			case 1:
				if(s_BatL2Sw==LOW)  
				{
					if(prnt) Serial.println("BatL2TurnedON");
                                        digitalWrite(r_Sol2, HIGH);
				}
				else
				{
					if(prnt) Serial.println("BatL2TurnedOFF");
                                        digitalWrite(r_Sol2, LOW);
				}
			break;

			case 4:
				if(s_IgnSw==LOW)  
				{
					if(prnt) Serial.println("IgnTurnedON");
					//timerContactor=
					//t.setTimeout(IgnDelay, ContactorOn);
				}
				else
				{
					if(prnt) Serial.println("IgnTurnedOFF");
					ContactorOff();
				}
			break;

			case 3:
				if(s_ContSw==LOW)
				{
  digitalWrite(r_SW180, HIGH);
				if(prnt) Serial.println("ContactorClosed");
				break;
				}
				else {
  digitalWrite(r_SW180, LOW);
				if(prnt) Serial.println("ContactorOpen");
				}
			break;


			case 2:
				if (s_SolSw==LOW)  {
  digitalWrite(r_SW80, HIGH);
				if(prnt) Serial.println("SolarClosed");
				}
else
digitalWrite(r_SW80, LOW);
			break;



			default:
				if(prnt) Serial.println("what the feck?");
			break;

	}




}

int whichPinInterrupted(){

int nSrc=0;

if(debounce(p_BatL1Sw)!=s_BatL1Sw)  {s_BatL1Sw=!s_BatL1Sw; intSrc[nSrc++]=0;}
if(debounce(p_BatL2Sw)!=s_BatL2Sw)  {s_BatL2Sw=!s_BatL2Sw; intSrc[nSrc++]=1;}
if(debounce(p_IgnSw)  !=s_IgnSw)    {s_IgnSw=!s_IgnSw;     intSrc[nSrc++]=4;}
if(debounce(p_ContSw) !=s_ContSw)   {s_ContSw=!s_ContSw;   intSrc[nSrc++]=3;}
if(debounce(p_SolSw)  !=s_SolSw)    {s_SolSw=!s_SolSw;     intSrc[nSrc++]=2;}


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




void BatL1Interrupt(){itrptSrc=0;}
void BatL2Interrupt(){itrptSrc=1;}
void IgnInterrupt(){itrptSrc=4;}
void ContSwInterrupt(){itrptSrc=3;}
void SolSwInterrupt(){itrptSrc=2;}
void attachInterrupts(){
attachInterrupt(0, BatL1Interrupt,   CHANGE); 
attachInterrupt(1, BatL2Interrupt,   CHANGE); 
attachInterrupt(4, IgnInterrupt,     CHANGE); 
attachInterrupt(3, ContSwInterrupt,  CHANGE);
attachInterrupt(2, SolSwInterrupt,   CHANGE); 
}

void detachInterrupts(){
detachInterrupt(0); 
detachInterrupt(1); 
detachInterrupt(2); 
detachInterrupt(3); 
detachInterrupt(4); 
}
//////Voltages///////////////////////////////////
void writeVoltages()
{
if(prnt) Serial.print("ShuntP:");
printDouble(v_ShuntP, 100);
if(prnt) Serial.print("ShuntM:");
printDouble(v_ShuntM, 100);
if(prnt) Serial.print("diff:");
double amps=1000*(v_ShuntP-v_ShuntM);
printDouble(amps, 100);
if(prnt) Serial.print("mV,LB1:");
printDouble(v_BattL1, 100);
if(prnt) Serial.print("V,LB2:");
printDouble(v_BattL2, 100);
if(prnt) Serial.print("V,VB: ");
printDouble(v_BattV, 100);
if(prnt) Serial.println("V");
}


int readVoltages()
{

int nsamp=100;
double sensorValue, dsamp=double(nsamp);
v_ShuntP=0.; v_ShuntM=0.; v_BattV=0.; v_BattL1=0.; v_BattL2=0.;

for(int counter=0; counter < nsamp; counter++)
{
delay(10/nsamp);

sensorValue = double(analogRead(pv_ShuntP));
v_ShuntP+=sensorValue * f0 /dsamp;

sensorValue = double(analogRead(pv_ShuntM));
v_ShuntM+=sensorValue * f0 /dsamp;

sensorValue = double(analogRead(pv_BattV));
v_BattV+=sensorValue * f1 /dsamp;

sensorValue = double(analogRead(pv_BattL1));
v_BattL1+=sensorValue * f3 /dsamp;   

sensorValue = double(analogRead(pv_BattL2));
v_BattL2+=sensorValue * f3 /dsamp; 
}

/*if(whichBattIsCharged!=0) {v_BattV-=0.12; v_BattL-=0.12;}
if(v_BattL<=v_MIN||v_BattV<=v_MIN) {//shudown all power consuming relays etc...
	nexe=93; return 1;}
else if(v_BattL>=v_MAX||v_BattV>=v_MAX)return -1;
else*/ 
return 0;
}

void readwriteVoltages(){
readVoltages();
writeVoltages();
}
//Relays////////////////////////////////////////////////////////////////////////////////////
void ContactorOn(){

if(debounce(p_IgnSw)!=LOW) return;

digitalWrite(r_SW180, HIGH);	

timerContactor=-1;

}


void ContactorOff(){
if(timerContactor!=-1)
{ t.deleteTimer(timerContactor); timerContactor=-1;}

digitalWrite(r_SW180, LOW);
}
void SolarOverrideOff(){
	if(prnt) Serial.println("stop overriding solar");
	digitalWrite(r_Sol1, LOW);
	digitalWrite(r_Sol2, LOW);
	whichBattIsCharged=0;
}
void AllSolarto2(){
	if(prnt) Serial.println("All Panels to Controller 2"); 
	digitalWrite(r_Sol2, LOW);
	digitalWrite(r_Sol1, HIGH);
	whichBattIsCharged=2;
}
void AllSolarto1(){
	if(prnt) Serial.println("All Panels to Controller 1");
	digitalWrite(r_Sol1, LOW);
	digitalWrite(r_Sol2, HIGH);
	whichBattIsCharged=1;
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

void printDouble( double val, double precision){

int digits=int(val);

unsigned int frac;

frac = int((val - double(digits)) * abs(precision));


if(prnt) Serial.print(digits);
if(prnt) Serial.print('.');
if(frac<10) if(prnt) Serial.print("0");
if(prnt) Serial.print(frac);      
}




