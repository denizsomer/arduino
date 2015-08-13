#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
//All of the data accumulated will be stored here
modbusDevice regBank;

#include <tristarMaster.h>
tristarMaster master;

#include <SD.h>



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

const  int p_ARCTx    = 0; //Rx0 
const  int p_ARCRx    = 1; //Tx0
const  int p_BatL1Sw  = 2; //  interrupt0
const  int p_BatL2Sw  = 3; //  interrupt1

const  int r_SP1    = 4;
const  int r_SP2    = 5;
const  int r_SW80    = 6;
const  int r_SW180   = 7;

const  int r_ContLED        = 8;
const  int r_BatteryLED        = 9;

const  int p_IgnSw   = 19; //  interrupt4 
const  int p_ContWatch  = 20; //  interrupt3
const  int p_SolSw   = 21; //  interrupt2

const int chipSelect = 23;

//voltage multipliers     
const double   f0=5.0/1023., f1=0.01506, f3=0.01527,
 f2=0.01580,f4=0.01445, v_ShuntMin=4.0, v_CRI=13.5, v_MAX=14.5, v_MIN=11.5;

//timers,locations in timer array
int timerReset, timerSW180=-1, timerSW80=-1,  
timerlogTristar, timerblinkContLEDSlow, timerblinkContLEDSlow2,
timerblinkContLEDFast, timerblinkContLEDFast2,
timerblinkBatteryLEDSlow, timerblinkBatteryLEDSlow2;

const char Source[2][11]={"ondort.txt","onikii.txt"};

int stateTristar[2], whichBattIsCharged;

const unsigned long IgnDelay=120000, IgnDelay2NDBAT=300000,
logTristarInterval=600000, SolDelay=5000, interruptDelay=200,
debounceDelay=10, readVoltageInterval=5000,resetInterval=43200000;

volatile int itrptSrc=-1;

unsigned long sinceTristarStateChange[2];

double v_ShuntP, v_ShuntM , v_BattL1, v_BattL2 , v_BattV , v_SolCont, Ahdaily[2];

bool SDInitialized=false;



void setup()
{

	//new interrupt
MCUCR = (1<<ISC00) | (1<<ISC01);
PCICR |= (1<<PCIE0);
PCMSK0 |= (1<<PCINT0); // 53
//PCMSK0 |= (1<<PCINT1); // 52
//PCMSK0 |= (1<<PCINT2); // 51
//PCMSK0 |= (1<<PCINT3); // 50


Serial.begin(9600); //initialize serial communication

master.setBaud(9600); 
regBank.setId(1);
for(i = 30008 ; i < 30029 ; i++) regBank.add(i);
master._device = &regBank;  

initializePins();
resetAll();

	Ahdaily[0]=0.0; Ahdaily[1]=0.0;

}


void initializePins(){
//initialize outputs
//uln2003-1
pinMode(r_SW180, OUTPUT);   digitalWrite(r_SW180, LOW);
pinMode(r_SW80, OUTPUT); digitalWrite(r_SW80, LOW);
pinMode(r_SP1, OUTPUT);       digitalWrite(r_SP1, LOW);
pinMode(r_SP2, OUTPUT);       digitalWrite(r_SP2, LOW);
//uln2003-2
pinMode(r_ContLED, OUTPUT);       digitalWrite(r_ContLED, LOW);
pinMode(r_BatteryLED, OUTPUT);       digitalWrite(r_BatteryLED, LOW);

pinMode(p_IgnSw, INPUT);         digitalWrite(p_IgnSw, HIGH); 
pinMode(p_ContWatch, INPUT);      digitalWrite(p_ContWatch, HIGH);
pinMode(p_BatL1Sw, INPUT);      digitalWrite(p_BatL1Sw, HIGH); 
pinMode(p_BatL2Sw, INPUT);      digitalWrite(p_BatL2Sw, HIGH); 
pinMode(p_SolSw, INPUT);       digitalWrite(p_SolSw, HIGH); 
pinMode(p_ContWatch, INPUT);      digitalWrite(p_ContWatch, HIGH);


pinMode(pv_ShuntM, INPUT); 
pinMode(pv_ShuntP, INPUT);
pinMode(pv_BattL1, INPUT);
pinMode(pv_BattL2, INPUT);
pinMode(pv_BattV, INPUT);

  pinMode(chipSelect, OUTPUT);   digitalWrite(chipSelect, HIGH);
}


///////////resets///////////////////////////////////////////////////////////////////
void resetAll(){
if(prnt) Serial.println("resetting...........................");
resetTimers();
resetSolar();
resetSD();
attachInterrupts();
resetInterrupts();

}

void logTristar(){

unsigned long  timer1;

for(int id=0; id<2; id++){
	
	timer1=millis();

	master.setSlave(id);

	while(1)
	{
	if(millis()-timer1>5000) break;
	if(master.run()){
		
		  //reset timer if state changed 
		if(stateTristar[id]!=master._device->get(0x754B)){
		stateTristar[id]=master._device->get(0x754B);
		sinceTristarStateChange[id]=millis();
		}
		//reset Ah counter if night
		if(stateTristar[id]==3) 	Ahdaily[id]=0.0; 		
		
		logtoSD(id);
		break;}
	}
}

return;
};


void resetSD()
{



  Serial.print("Initializing SD card...");
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    
    return;
  }
  Serial.println("card initialized.");
  SDInitialized=true;
}
void logtoSD(int id)
{
	if(!SDInitialized){
	
	Serial.println("SD card not initialized.");
	return;
	
	}


	long num;
	File dataFile;

  
    // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.


	dataFile = SD.open(Source[id], FILE_WRITE);


  // if the file is available, write to it:
  if (dataFile) {



num=((long)master._device->get(0x7545)) <<16;
num|=master._device->get(0x7546);
dataFile.print(num); dataFile.print(" h ");

dataFile.print(dfact[0]*master._device->get(0x7538)); dataFile.print(" V ");

dataFile.print(dfact[2]*master._device->get(0x753a)); dataFile.print(" V ");

double achrg=dfact[3]*master._device->get(0x753b);
dataFile.print(achrg); dataFile.print(" A ");

Ahdaily[id]+=achrg*logTristarInterval/(3600000.);
dataFile.print(Ahdaily[id]); dataFile.print(" Ah ");

dataFile.print(dfact[6]*master._device->get(0x753e)); dataFile.print(" C ");

dataFile.print(dfact[7]*master._device->get(0x753f)); dataFile.print(" C ");

dataFile.print(dfact[8]*master._device->get(0x7540)); dataFile.print(" V ");


dataFile.print(State[stateTristar[id]]); dataFile.print(" ");
num=(millis()-sinceTristarStateChange[id])/60000;
dataFile.print(num); dataFile.print(" m ");

num=master._device->get(0x754C);

if(num>=230) dataFile.print("100");
else dataFile.print(num/2.3); 

dataFile.println(" % ");

    dataFile.close();
    
    Serial.println("finished writing ");
  }  
  // if the file isn't open, pop up an error:
  //else 
  {
   // Serial.println("error opening file");

	num=((long)master._device->get(0x7545)) <<16;
num|=master._device->get(0x7546);
Serial.print(num); Serial.print("h ");

Serial.print(dfact[0]*master._device->get(0x7538)); Serial.print("V ");

Serial.print(dfact[2]*master._device->get(0x753a)); Serial.print("V ");

double achrg=dfact[3]*master._device->get(0x753b);
Serial.print(achrg); Serial.print("A ");

Ahdaily[id]+=achrg*logTristarInterval/(3600000.);
Serial.print(Ahdaily[id]); Serial.print("Ah ");

Serial.print(dfact[6]*master._device->get(0x753e)); Serial.print("C ");

Serial.print(dfact[7]*master._device->get(0x753f)); Serial.print("C ");

Serial.print(dfact[8]*master._device->get(0x7540)); Serial.print("V ");

Serial.print(State[stateTristar[id]]); Serial.print(" ");
num=(millis()-sinceTristarStateChange[id])/60000;
Serial.print(num); Serial.print(" m ");

num=master._device->get(0x754C);

if(num>=230) Serial.print("100");
else Serial.print(num/2.3); 

Serial.println("% ");


  } 
  
  
}


void resetTimers(){
t.reset();

//t.setInterval(readVoltageInterval, readwriteVoltages);
timerlogTristar= t.setInterval(logTristarInterval, logTristar);


timerReset=t.setInterval(resetInterval, resetAll);
timerblinkContLEDSlow= t.setInterval(2000, blinkContLED);
timerblinkContLEDSlow2= t.setInterval(2000, blinkContLED2);
timerblinkContLEDFast= t.setInterval(500, blinkContLED);
timerblinkContLEDFast2= t.setInterval(500, blinkContLED2);

timerblinkBatteryLEDSlow= t.setInterval(2000, blinkBatteryLED);
timerblinkBatteryLEDSlow2= t.setInterval(2000, blinkBatteryLED2);

}


void resetSolar(){
	if(prnt) Serial.println("reset solar");
	digitalWrite(r_SP1, LOW);
	digitalWrite(r_SP2, LOW);

	whichBattIsCharged=0;

	manageLEDS();
}
void resetInterrupts(){
int i;

for(i=2; i<4; i++){
manageInterrupt(i);
delay(200);
}

for(i=19; i<22; i++){
manageInterrupt(i);
delay(200);
};

}







///////loop////////////////////////////////////////////////////////////////////////////////
void loop()
{
t.run();

master.run();

if(itrptSrc>-1) manageInterrupt(itrptSrc);

} //loop





///////interrupt handlers////////////////////////////////////////////////////////////////////////////////
void manageInterrupt(int iSrc){

	detachInterrupts();

if(prnt) {Serial.print("Interrupt: "); Serial.print(iSrc); Serial.println(" ...");}
	
switch(iSrc){
			case p_BatL1Sw:
				if(debounce(p_BatL1Sw)==LOW)  
				{
					if(prnt) Serial.println("BatL1 SW ON");

					if(digitalRead(p_SolSw)==LOW) AllSolarto1();
				}
				else {if(prnt) Serial.println("BatL1 SW OFF");}
			break;

			case p_BatL2Sw:
				if(debounce(p_BatL2Sw)==LOW)  
				{
					if(prnt) Serial.println("BatL2 SW ON");

					if(digitalRead(p_SolSw)==LOW) AllSolarto2();
					else  SW80ON();
				}
				else
				{
					if(prnt) Serial.println("BatL2 SW OFF");

                    SW80OFF();
				}
			break;

			case p_IgnSw:
				if(debounce(p_IgnSw)==LOW)  
				{
					if(prnt) Serial.println("IgnON");

					timerSW180 = t.setTimeout(IgnDelay, SW180ON);

					timerSW80 = t.setTimeout(IgnDelay2NDBAT, SW80ON);
				}
				else
				{
					SW180OFF();
						SW80OFF();

					if(prnt) Serial.println("IgnOFF");
				}
			break;

			case p_ContWatch:
				 if(debounce(p_ContWatch) ==LOW )
				{
				 if(prnt) Serial.println("Contactor WATCH ON");

				 if(digitalRead(p_BatL2Sw)==LOW) SW80ON();

				}
			 else  {
                if(prnt) Serial.println("Contactor WATCH OFF");
			 }
			break;


			case p_SolSw:
				if (debounce(p_SolSw)==LOW)  {

					if(prnt) Serial.println("Solar SW ON");

						 if(digitalRead(p_BatL1Sw)==LOW)  AllSolarto1();
					     else   AllSolarto2();

				}
				else{
					if(prnt) Serial.println("Solar SW OFF");
					resetSolar();
				}
			break;

			default: //check the additional suspect



			break;

	}

manageLEDS();

	itrptSrc=-1;

	attachInterrupts();

}

void manageLEDS(){


	if(digitalRead(p_ContWatch)==LOW)	
	{
		if(digitalRead(p_IgnSw)==LOW)
		{ 
			if(digitalRead(p_BatL2Sw)==LOW) enableblinkContLEDSlow2();
				else enableblinkContLEDSlow();
		}
		else  
                {    	if(digitalRead(p_BatL2Sw)==LOW) enableblinkContLEDFast2();
                                else enableblinkContLEDFast();
                
                }
	}
	else disableblinkContLED();



	if(digitalRead(p_SolSw)==LOW)	
	{
			if(digitalRead(p_BatL2Sw)==LOW) enableblinkBatteryLEDSlow2();
				else enableblinkBatteryLEDSlow();
	}
	else disableblinkBatteryLED();


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




void BatL1Interrupt(){itrptSrc=p_BatL1Sw;}
void BatL2Interrupt(){itrptSrc=p_BatL2Sw;}
void IgnInterrupt(){itrptSrc=p_IgnSw;}
void ContWatchInterrupt(){itrptSrc=p_ContWatch;}
void SolSwInterrupt(){itrptSrc=p_SolSw;}
ISR(PCINT0_vect) {itrptSrc=99;}
void attachInterrupts(){
attachInterrupt(0, BatL1Interrupt,   CHANGE); 
attachInterrupt(1, BatL2Interrupt,   CHANGE); 
attachInterrupt(4, IgnInterrupt,     CHANGE); 
attachInterrupt(3, ContWatchInterrupt,  CHANGE);
attachInterrupt(2, SolSwInterrupt,   CHANGE); 
}

void detachInterrupts(){
detachInterrupt(0); 
detachInterrupt(1); 
detachInterrupt(2); 
detachInterrupt(3); 
detachInterrupt(4); 
}
//////Contactor//////////////////////////////////
void SW180ON(){

if(debounce(p_IgnSw)!=LOW) return;

if(prnt) Serial.println("SW180 ON");


digitalWrite(r_SW180, HIGH);	

timerSW180=-1;

manageLEDS();

}


void SW180OFF(){
if(timerSW180!=-1)
{ t.deleteTimer(timerSW180); timerSW180=-1;}

digitalWrite(r_SW180, LOW);

manageLEDS();
}

void SW80ON(){

if(digitalRead(p_ContWatch)!=LOW) return;

	if(prnt) Serial.println("SW80 ON"); 

digitalWrite(r_SW80, HIGH);	

timerSW80=-1;

manageLEDS();

}


void SW80OFF(){
	if(timerSW80!=-1)
{ t.deleteTimer(timerSW80); timerSW80=-1;}

digitalWrite(r_SW80, LOW);

manageLEDS();
}


void blinkContLED(){
	ContLEDH();
	t.setTimeout(100, ContLEDL);
}

void blinkContLED2(){
ContLEDH();
t.setTimeout(80, ContLEDL);
t.setTimeout(160, ContLEDH);
t.setTimeout(240, ContLEDL);
}

void ContLEDL(){
digitalWrite(r_ContLED,LOW);
}

void ContLEDH(){
digitalWrite(r_ContLED,HIGH);
}
void enableblinkContLEDFast(){
	t.disable(timerblinkContLEDSlow);
	t.enable(timerblinkContLEDFast);
	t.disable(timerblinkContLEDSlow2);
        t.disable(timerblinkContLEDFast2);
}

void enableblinkContLEDFast2(){
	t.disable(timerblinkContLEDSlow);
	t.disable(timerblinkContLEDFast);
	t.disable(timerblinkContLEDSlow2);
        t.enable(timerblinkContLEDFast2);
}

void enableblinkContLEDSlow(){
	t.enable(timerblinkContLEDSlow);
	t.disable(timerblinkContLEDFast);
	t.disable(timerblinkContLEDSlow2);
        t.disable(timerblinkContLEDFast2);
}
void enableblinkContLEDSlow2(){
	t.disable(timerblinkContLEDSlow);
	t.disable(timerblinkContLEDFast);
	t.enable(timerblinkContLEDSlow2);
        t.disable(timerblinkContLEDFast2);
}
void disableblinkContLED(){
	t.disable(timerblinkContLEDFast);
	t.disable(timerblinkContLEDSlow);
	t.disable(timerblinkContLEDSlow2);
        t.disable(timerblinkContLEDFast2);
	digitalWrite(r_ContLED,LOW);
}
void blinkBatteryLED(){
	digitalWrite(r_BatteryLED,HIGH);
	t.setTimeout(100, BatteryLEDL);
}

void blinkBatteryLED2(){
BatteryLEDH();
t.setTimeout(100, BatteryLEDL);
t.setTimeout(200, BatteryLEDH);
t.setTimeout(300, BatteryLEDL);
}
void BatteryLEDL(){
digitalWrite(r_BatteryLED,LOW);
}
void BatteryLEDH(){
digitalWrite(r_BatteryLED,HIGH);
}

void enableblinkBatteryLEDSlow(){
	t.enable(timerblinkBatteryLEDSlow);
	t.disable(timerblinkBatteryLEDSlow2);
}
void enableblinkBatteryLEDSlow2(){
	t.disable(timerblinkBatteryLEDSlow);
	t.enable(timerblinkBatteryLEDSlow2);
}
void disableblinkBatteryLED(){
  Serial.println("disabled batery led");
	t.disable(timerblinkBatteryLEDSlow);
	t.disable(timerblinkBatteryLEDSlow2);
	digitalWrite(r_BatteryLED,LOW);
}
//////Voltages///////////////////////////////////
void writeVoltages()
{
//if(prnt) Serial.print("ShuntP:");
//printDouble(v_ShuntP, 100);
//if(prnt) Serial.print("ShuntM:");
//printDouble(v_ShuntM, 100);
//if(prnt) Serial.print("diff:");
//double amps=1000*(v_ShuntP-v_ShuntM);
//printDouble(amps, 100);
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

void AllSolarto2(){
	if(digitalRead(p_SolSw)!=LOW) {disableblinkBatteryLED(); return;}

	if(prnt) Serial.println("All Panels to Controller 2"); 
	digitalWrite(r_SP2, LOW);
	digitalWrite(r_SP1, HIGH);
	whichBattIsCharged=2;

	manageLEDS();
}
void AllSolarto1(){
	if(digitalRead(p_SolSw)!=LOW)  {disableblinkBatteryLED(); return;}

	if(prnt) Serial.println("All Panels to Controller 1");

	digitalWrite(r_SP1, LOW);
	digitalWrite(r_SP2, HIGH);
	whichBattIsCharged=1;

	manageLEDS();
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




