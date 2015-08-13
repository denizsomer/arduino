

#include "AmbulanceP.h"
#include "TimerList.h"
extern TimerList t;





#include <tristarMaster.h>
extern tristarMaster master;

AmbulanceP::AmbulanceP(void)                       
{    
	itrptSrc=-1;

	
	SW180Timeout=NULL;
	SW80Timeout=NULL;

	timerblinkStatusLED=NULL;
	timerblinkContLEDSlow=NULL;
	timerblinkContLEDSlow2=NULL;
	timerblinkContLEDFast=NULL;
	timerblinkContLEDFast2=NULL;
	timerblinkBatteryLEDSlow=NULL;
	timerblinkBatteryLEDSlow2=NULL;


  return;
}




	                                          
AmbulanceP::~AmbulanceP(void)                     
{         
  return;
}


void AmbulanceP::initializePins(){
//initialize outputs
//uln2003-1
pinMode(o_SW180, OUTPUT);   digitalWrite(o_SW180, LOW);
pinMode(o_SW80, OUTPUT); digitalWrite(o_SW80, LOW);
pinMode(o_SP1, OUTPUT);       digitalWrite(o_SP1, LOW);
pinMode(o_SP2, OUTPUT);       digitalWrite(o_SP2, LOW);
//uln2003-2
pinMode(o_ContLED, OUTPUT);       digitalWrite(o_ContLED, LOW);
pinMode(o_BatteryLED, OUTPUT);       digitalWrite(o_BatteryLED, LOW);
pinMode(o_StatusLED, OUTPUT);       digitalWrite(o_StatusLED, LOW);

pinMode( i_IgnSw, INPUT);         digitalWrite( i_IgnSw, HIGH); 
pinMode( i_ContWatch, INPUT);      digitalWrite( i_ContWatch, HIGH);
pinMode( i_BatL1Sw, INPUT);      digitalWrite( i_BatL1Sw, HIGH); 
pinMode( i_BatL2Sw, INPUT);      digitalWrite( i_BatL2Sw, HIGH); 
pinMode( i_SolSw, INPUT);       digitalWrite( i_SolSw, HIGH); 
pinMode( i_ContWatch, INPUT);      digitalWrite( i_ContWatch, HIGH);


pinMode(i_ShuntM, INPUT); 
pinMode(i_ShuntP, INPUT);
pinMode(i_BattL1, INPUT);
pinMode(i_BattL2, INPUT);
pinMode(i_BattV, INPUT);

  pinMode(o_chipSelect, OUTPUT);   digitalWrite(o_chipSelect, HIGH);
}


///////////resets///////////////////////////////////////////////////////////////////
void AmbulanceP::reset(){
if(prnt) Serial.println("resetting...........................");

digitalWrite(o_StatusLED,HIGH);

resetTimers();

resetSolar();

resetInterrupts();

digitalWrite(o_StatusLED,LOW);

}


void AmbulanceP::resetTimers(){

//t.setInterval(readVoltageInterval, READWRITEVOLTAGES);

if(timerblinkStatusLED!=NULL) timerblinkStatusLED->deleteTimer(); timerblinkStatusLED=NULL;
timerblinkStatusLED= t.setInterval(500, BLINKSTATUSLED);
disableblinkStatusLED();

if(timerblinkContLEDSlow!=NULL) timerblinkContLEDSlow->deleteTimer(); timerblinkContLEDSlow=NULL;
timerblinkContLEDSlow= t.setInterval(2000, BLINKCONTLED);
timerblinkContLEDSlow->disable();

if(timerblinkContLEDSlow2!=NULL) timerblinkContLEDSlow2->deleteTimer(); timerblinkContLEDSlow2=NULL;
timerblinkContLEDSlow2= t.setInterval(2000, BLINKCONTLED2);
timerblinkContLEDSlow2->disable();

if(timerblinkContLEDFast!=NULL) timerblinkContLEDFast->deleteTimer(); timerblinkContLEDFast=NULL;
timerblinkContLEDFast= t.setInterval(500, BLINKCONTLED);
timerblinkContLEDFast->disable();

if(timerblinkContLEDFast2!=NULL) timerblinkContLEDFast2->deleteTimer(); timerblinkContLEDFast2=NULL;
timerblinkContLEDFast2= t.setInterval(500, BLINKCONTLED2);
timerblinkContLEDFast2->disable();

if(timerblinkBatteryLEDSlow!=NULL) timerblinkBatteryLEDSlow->deleteTimer(); timerblinkBatteryLEDSlow=NULL;
timerblinkBatteryLEDSlow= t.setInterval(2000, BLINKBATTERYLED);
timerblinkBatteryLEDSlow->disable();

if(timerblinkBatteryLEDSlow2!=NULL) timerblinkBatteryLEDSlow2->deleteTimer(); timerblinkBatteryLEDSlow2=NULL;
timerblinkBatteryLEDSlow2= t.setInterval(2000, BLINKBATTERYLED2);
timerblinkBatteryLEDSlow2->disable();


}





void AmbulanceP::resetSolar(){
	if(prnt) Serial.println("reset solar");
	digitalWrite(o_SP1, LOW);
	digitalWrite(o_SP2, LOW);

	whichBattIsCharged=0;

	manageLEDS();
}
void AmbulanceP::resetInterrupts(){
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
void AmbulanceP::run()
{

if(itrptSrc>-1) manageInterrupt(itrptSrc);

} //loop





///////interrupt handlers////////////////////////////////////////////////////////////////////////////////
void AmbulanceP::manageInterrupt(int iSrc){


if(prnt) {Serial.print("Interrupt: "); Serial.print(iSrc); Serial.println(" ...");}
	
switch(iSrc){
			case  i_BatL1Sw:
				case  i_BatL2Sw:
				if(debounce( i_BatL1Sw)==LOW)  
				{
					if(prnt) Serial.println("BatL1 SW ON");

					if(digitalRead( i_SolSw)==LOW) AllSolarto1();
					else SW80Off();
				}
				else if(debounce( i_BatL2Sw)==LOW)  
				{
					if(prnt) Serial.println("BatL2 SW ON");

					if(digitalRead( i_SolSw)==LOW) AllSolarto2();
					else  SW80On();
				}
			break;

			case  i_IgnSw:
				if(debounce( i_IgnSw)==LOW)  
				{
					if(prnt) Serial.println("IgnON");

     //               if(SW180Timeout!=NULL) SW180Timeout->deleteTimer();SW180Timeout=NULL;
					//SW180Timeout = t.setTimeout(IgnDelay, SW180ON);

					//if(SW80Timeout!=NULL) SW80Timeout->deleteTimer();SW80Timeout=NULL;
					//SW80Timeout = t.setTimeout(IgnDelay2NDBAT, SW80ON);

					master._device->set(0x7537,1);
					master._device->set(10000+0x7537,1);
				}
				else
				{
					//SW180Off();
					//	SW80Off();

						master._device->set(0x7537,0);
						master._device->set(10000+0x7537,0);

					if(prnt) Serial.println("IgnOFF");
				}
			break;

			case  i_ContWatch:
				 if(debounce( i_ContWatch) ==LOW )
				{
				 if(prnt) Serial.println("Contactor WATCH ON");

				 if(digitalRead( i_BatL2Sw)==LOW) SW80On();

				}
			 else  {
                if(prnt) Serial.println("Contactor WATCH OFF");
				SW80Off();
			 }
			break;


			case  i_SolSw:
				if (debounce( i_SolSw)==LOW)  {

					if(prnt) Serial.println("Solar SW ON");

						 if(digitalRead( i_BatL1Sw)==LOW)  AllSolarto1();
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

	

}

void AmbulanceP::manageLEDS(){


	if(digitalRead( i_ContWatch)==LOW)	
	{
		if(digitalRead( i_IgnSw)==LOW)
		{ 
			if(digitalRead( i_BatL2Sw)==LOW) enableblinkContLEDSlow2();
				else enableblinkContLEDSlow();
		}
		else  
                {    	if(digitalRead( i_BatL2Sw)==LOW) enableblinkContLEDFast2();
                                else enableblinkContLEDFast();
                
                }
	}
	else disableblinkContLED();



	if(digitalRead( i_SolSw)==LOW)	
	{
			if(digitalRead( i_BatL2Sw)==LOW) enableblinkBatteryLEDSlow2();
				else enableblinkBatteryLEDSlow();
	}
	else disableblinkBatteryLED();


}


bool AmbulanceP::debounce(int pin)
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




void AmbulanceP::BatL1Interrupt(){itrptSrc=i_BatL1Sw;}
void AmbulanceP::BatL2Interrupt(){itrptSrc=i_BatL2Sw;}
void AmbulanceP::IgnInterrupt(){itrptSrc=i_IgnSw;}
void AmbulanceP::ContWatchInterrupt(){itrptSrc=i_ContWatch;}
void AmbulanceP::SolSwInterrupt(){itrptSrc=i_SolSw;}




//////Contactor//////////////////////////////////
void AmbulanceP::SW180On(){

if(debounce( i_IgnSw)!=LOW) return;

if(prnt) Serial.println("SW180 ON");


digitalWrite(o_SW180, HIGH);	

manageLEDS();

}


void AmbulanceP::SW180Off(){
if(SW180Timeout!=NULL) SW180Timeout->deleteTimer(); SW180Timeout=NULL;

digitalWrite(o_SW180, LOW);

manageLEDS();
}

void AmbulanceP::SW80On(){

if(digitalRead( i_ContWatch)!=LOW) return;

	if(prnt) Serial.println("SW80 ON"); 

digitalWrite(o_SW80, HIGH);	

manageLEDS();

}


void AmbulanceP::SW80Off(){
 if(SW80Timeout!=NULL) SW80Timeout->deleteTimer();  SW80Timeout=NULL;

digitalWrite(o_SW80, LOW);

manageLEDS();
}


void AmbulanceP::blinkContLED(){
	ContLEDH();
	t.setTimeout(100, CONTLEDL);
}

void AmbulanceP::blinkContLED2(){
ContLEDH();
t.setTimeout(80, CONTLEDL);
t.setTimeout(160, CONTLEDH);
t.setTimeout(240, CONTLEDL);
}

void AmbulanceP::ContLEDL(){
digitalWrite(o_ContLED,LOW);
}

void AmbulanceP::ContLEDH(){
digitalWrite(o_ContLED,HIGH);
}
void AmbulanceP::enableblinkContLEDFast(){
	timerblinkContLEDSlow->disable();
	timerblinkContLEDFast->enable();
	timerblinkContLEDSlow2->disable();
    timerblinkContLEDFast2->disable();
}

void AmbulanceP::enableblinkContLEDFast2(){
	timerblinkContLEDSlow->disable();
	timerblinkContLEDFast->disable();
	timerblinkContLEDSlow2->disable();
    timerblinkContLEDFast2->enable();
}

void AmbulanceP::enableblinkContLEDSlow(){
	timerblinkContLEDSlow->enable();
	timerblinkContLEDFast->disable();
	timerblinkContLEDSlow2->disable();
    timerblinkContLEDFast2->disable();
}
void AmbulanceP::enableblinkContLEDSlow2(){
	timerblinkContLEDSlow->disable();
	timerblinkContLEDFast->disable();
	timerblinkContLEDSlow2->enable();
    timerblinkContLEDFast2->disable();
}
void AmbulanceP::disableblinkContLED(){
	timerblinkContLEDFast->disable();
	timerblinkContLEDSlow->disable();
	timerblinkContLEDSlow2->disable();
    timerblinkContLEDFast2->disable();
	digitalWrite(o_ContLED,LOW);
}
void AmbulanceP::blinkBatteryLED(){
	digitalWrite(o_BatteryLED,HIGH);
	t.setTimeout(100, BATTERYLEDL);
}

void AmbulanceP::blinkBatteryLED2(){
BatteryLEDH();
t.setTimeout(100, BATTERYLEDL);
t.setTimeout(200, BATTERYLEDH);
t.setTimeout(300, BATTERYLEDL);
}
void AmbulanceP::BatteryLEDL(){
digitalWrite(o_BatteryLED,LOW);
}
void AmbulanceP::BatteryLEDH(){
digitalWrite(o_BatteryLED,HIGH);
}

void AmbulanceP::enableblinkBatteryLEDSlow(){
	timerblinkBatteryLEDSlow->enable();
	timerblinkBatteryLEDSlow2->disable();
}
void AmbulanceP::enableblinkBatteryLEDSlow2(){
	timerblinkBatteryLEDSlow->disable();
	timerblinkBatteryLEDSlow2->enable();
}
void AmbulanceP::disableblinkBatteryLED(){
  if(prnt) Serial.println("disabled batery led");
	timerblinkBatteryLEDSlow->disable();
	timerblinkBatteryLEDSlow2->disable();
	digitalWrite(o_BatteryLED,LOW);
}
//////Voltages///////////////////////////////////
//void AmbulanceP::writeVoltages()
//{
////if(prnt) Serial.print("ShuntP:");
////printDouble(v_ShuntP, 100);
////if(prnt) Serial.print("ShuntM:");
////printDouble(v_ShuntM, 100);
////if(prnt) Serial.print("diff:");
////float amps=1000*(v_ShuntP-v_ShuntM);
////printDouble(amps, 100);
//if(prnt) Serial.print("mV,LB1:");
//printDouble(v_BattL1, 100);
//if(prnt) Serial.print("V,LB2:");
//printDouble(v_BattL2, 100);
//if(prnt) Serial.print("V,VB: ");
//printDouble(v_BattV, 100);
//if(prnt) Serial.println("V");
//}
//
//
//int AmbulanceP::readVoltages()
//{
//
//int nsamp=100;
//float sensorValue, dsamp=float(nsamp);
//v_ShuntP=0.; v_ShuntM=0.; v_BattV=0.; v_BattL1=0.; v_BattL2=0.;
//
//for(int counter=0; counter < nsamp; counter++)
//{
//delay(10/nsamp);
//
//sensorValue = float(analogRead(pv_ShuntP));
//v_ShuntP+=sensorValue * f0 /dsamp;
//
//sensorValue = float(analogRead(pv_ShuntM));
//v_ShuntM+=sensorValue * f0 /dsamp;
//
//sensorValue = float(analogRead(pv_BattV));
//v_BattV+=sensorValue * f1 /dsamp;
//
//sensorValue = float(analogRead(pv_BattL1));
//v_BattL1+=sensorValue * f3 /dsamp;   
//
//sensorValue = float(analogRead(pv_BattL2));
//v_BattL2+=sensorValue * f3 /dsamp; 
//}
//
///*if(whichBattIsCharged!=0) {v_BattV-=0.12; v_BattL-=0.12;}
//if(v_BattL<=v_MIN||v_BattV<=v_MIN) {//shudown all power consuming relays etc...
//	nexe=93; return 1;}
//else if(v_BattL>=v_MAX||v_BattV>=v_MAX)return -1;
//else*/ 
//return 0;
//}
//
//void AmbulanceP::readwriteVoltages(){
//readVoltages();
//writeVoltages();
//}
//Relays////////////////////////////////////////////////////////////////////////////////////

void AmbulanceP::AllSolarto2(){

	if(prnt) Serial.println("All Panels to Controller 2"); 
	digitalWrite(o_SP2, LOW);
	digitalWrite(o_SP1, HIGH);
	whichBattIsCharged=2;

	manageLEDS();
}
void AmbulanceP::AllSolarto1(){

	if(prnt) Serial.println("All Panels to Controller 1");

	digitalWrite(o_SP1, LOW);
	digitalWrite(o_SP2, HIGH);
	whichBattIsCharged=1;

	manageLEDS();
}




void AmbulanceP::printFloat( float val, int precision){

int digits=int(val);

unsigned int frac;

frac = int((val - float(digits)) * abs(precision));


if(prnt) Serial.print(digits);
if(prnt) Serial.print('.');
if(frac<10) if(prnt) Serial.print("0");
if(prnt) Serial.print(frac);      
}






//Status led///////////////////////////////////////////////////////////////////////////////////
void AmbulanceP::blinkStatusLED(){
	//Serial.println("blink blink");
	StatusLEDON();
	//t.setTimeout(30, STATUSLEDOFF);
}


void AmbulanceP::enableblinkStatusLED(void){
	timerblinkStatusLED->enable();
}

void AmbulanceP::disableblinkStatusLED(void){
	timerblinkStatusLED->disable();
}

void AmbulanceP::StatusLEDON(void) { digitalWrite(o_StatusLED,HIGH);}

void AmbulanceP::StatusLEDOFF(void){digitalWrite(o_StatusLED,LOW);}
