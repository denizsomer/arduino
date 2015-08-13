


#include "AmbulanceC.h"


AmbulanceC::AmbulanceC(void)                       
{        
itrptSrc=-1;

timerblinkStatusLED=NULL;
timerblinkAlLEDSlow=NULL;
timerblinkAlLEDMedium=NULL;
timerblinkAlLEDFast=NULL;
timerblinkScene=NULL;
timercheckStatus=NULL;

tapTimeout=NULL;
alarmTimeout=NULL;
srnTimeout1=NULL;
srnTimeout2=NULL;
doorTimeout=NULL;


  return;
}




	                                          
AmbulanceC::~AmbulanceC(void)                     
{         
  return;
}





void AmbulanceC::initializePins(){


//initialize outputs
pinMode(o_SrnOff, OUTPUT); digitalWrite(o_SrnOff, LOW);
pinMode(o_SrnOn, OUTPUT); digitalWrite(o_SrnOn, LOW);
pinMode(o_LHScene, OUTPUT); digitalWrite(o_LHScene, LOW);
pinMode(o_RHScene, OUTPUT); digitalWrite(o_RHScene, LOW);

pinMode(o_AlPow, OUTPUT); digitalWrite(o_AlPow, LOW);
pinMode(o_Sink, OUTPUT); digitalWrite(o_Sink, LOW);
pinMode(23, OUTPUT); digitalWrite(23, LOW);
pinMode(25, OUTPUT); digitalWrite(25, LOW);


pinMode(o_LED1, OUTPUT); digitalWrite(o_LED1, LOW);
pinMode(o_LED3, OUTPUT); digitalWrite(o_LED3, LOW);
pinMode(o_StatusLED, OUTPUT); digitalWrite(o_StatusLED, LOW);
pinMode(o_AlLED, OUTPUT); digitalWrite(o_AlLED, LOW);
pinMode(o_LED5, OUTPUT); digitalWrite(o_LED5, LOW);

//Listening, pull-ups
pinMode(i_pirTamp, INPUT); digitalWrite(i_pirTamp, HIGH); 
pinMode(i_pirAl, INPUT);   digitalWrite(i_pirAl, HIGH); 

pinMode(i_TotAlSw, INPUT); digitalWrite(i_TotAlSw, HIGH); 
pinMode(i_PerAlSw, INPUT); digitalWrite(i_PerAlSw, HIGH);
pinMode(i_DoorSw, INPUT);  digitalWrite(i_DoorSw, HIGH); 

pinMode(i_IsoSw, INPUT);   digitalWrite(i_IsoSw, HIGH);  
pinMode(i_EmSw, INPUT);    digitalWrite(i_EmSw, HIGH); 
pinMode(i_HbrkSw, INPUT);  digitalWrite(i_HbrkSw, HIGH); 
pinMode(i_TapSw, INPUT);   digitalWrite(i_TapSw, HIGH);

}




void AmbulanceC::run()
{
if(itrptSrc>-1) { manageInterrupt(itrptSrc);}
}

void AmbulanceC::reset(void){

digitalWrite(o_StatusLED,HIGH);

initializeTimers();

resetScene();

resetAl();

resetSrn();

resetInterrupts();

digitalWrite(o_StatusLED,LOW);
}

void AmbulanceC::initializeTimers(void){
if(timerblinkStatusLED!=NULL) timerblinkStatusLED->deleteTimer();timerblinkStatusLED=NULL;
timerblinkStatusLED= t.setInterval(500, BLINKSTATUSLED);
disableblinkStatusLED();

if(timerblinkAlLEDSlow!=NULL) timerblinkAlLEDSlow->deleteTimer(); timerblinkAlLEDSlow=NULL;
timerblinkAlLEDSlow= t.setInterval(5000, BLINKALLED);

if(timerblinkAlLEDMedium!=NULL) timerblinkAlLEDMedium->deleteTimer(); timerblinkAlLEDMedium=NULL;
timerblinkAlLEDMedium= t.setInterval(5000, BLINKALLED2);

if(timerblinkAlLEDFast!=NULL) timerblinkAlLEDFast->deleteTimer(); timerblinkAlLEDFast=NULL;
timerblinkAlLEDFast= t.setInterval(200, BLINKALLED);

disableblinkAlLED();

if(timerblinkScene!=NULL) timerblinkScene->deleteTimer(); timerblinkScene=NULL;
timerblinkScene= t.setInterval(500, BLINKSCENE);
disableblinkScene();
}
void AmbulanceC::resetAl(void){
nAls=0;
AlArmed=false;

}
void AmbulanceC::resetSrn(void){

	if(srnTimeout1!=NULL) srnTimeout1->deleteTimer(); srnTimeout1=NULL;
	if(srnTimeout2!=NULL) srnTimeout2->deleteTimer(); srnTimeout2=NULL;

AlSrc=-1;

disableblinkScene();

srnOff() ;

}

void AmbulanceC::resetScene(void){

scene=leftScene	;
}

void AmbulanceC::resetInterrupts(void){
int i;

itrptSrc=-1;

setItrptLevel(_ALL);
setAlLevel(_ALL);

s_IsoSw  =!digitalRead(i_IsoSw);  
s_EmSw   =!digitalRead(i_EmSw); 
s_HbrkSw =!digitalRead(i_HbrkSw);
s_TapSw  =!digitalRead(i_TapSw); 

testInterrupts();

//leave AlSw to end, to set the itrpLevel
manageInterrupt(18);

}

void AmbulanceC::testInterrupts(){
int i,idelay=0;

for(i=2; i<4; i++){
manageInterrupt(i);
delay(idelay);
}

for(i=0; i<4; i++){
manageInterrupt(99);
delay(idelay);
}

manageInterrupt(20);
delay(idelay);

}


//Alarm///////////////////////////////////////////////////////////////////////////////
void AmbulanceC::manageInterrupt(int iSrc){


if(iSrc==99)
{     if(s_EmSw  !=debounce(i_EmSw))  {s_EmSw=debounce(i_EmSw);    iSrc=i_EmSw; }
//else if(s_IsoSw !=debounce(i_IsoSw)) {s_IsoSw=debounce(i_IsoSw);   iSrc=i_IsoSw; }
else if(s_HbrkSw!=debounce(i_HbrkSw)){s_HbrkSw=debounce(i_HbrkSw); iSrc=i_HbrkSw; }
else if(s_TapSw !=debounce(i_TapSw)) {s_TapSw=debounce(i_TapSw);   iSrc=i_TapSw; }
}

if(ItrptLevel==_ALL)
{
		//if(prnt) {Serial.print("Interrupt: "); Serial.print(iSrc); Serial.println(" ...");}

		switch(iSrc)
		{			
				if(dbg) {Serial.print("....................AlLevel:"); Serial.println(AlLevel);}

				case i_pirTamp:
					if(AlLevel!=_ALL) break;
					if(dbg) Serial.print("pirTamp: ");
					if(debounce(i_pirTamp)!=LOW){
				if(dbg) Serial.println("Tampered!");

				if(AlArmed) {
				AlSrc=itrptSrc; 
				playSrn();
				}}
					else{if(dbg) Serial.println("OK");}
				break;

				case i_pirAl:
					if(AlLevel!=_ALL) break;
					if(dbg) Serial.print("pirAl: ");
					if(debounce(i_pirAl) !=LOW){
				if(dbg) Serial.println("Intruder!");

				if(AlArmed) {AlSrc=itrptSrc;
				playSrn();
				}}
					else{if(dbg) Serial.println("OK");}
				break;
			

			case i_DoorSw:
				if(dbg) Serial.print("DoorSw: ");
				if(debounce(i_DoorSw) ==LOW){
			if(dbg) Serial.println("Door open");

			if(AlArmed){AlSrc=itrptSrc; 
			if(doorTimeout!=NULL) doorTimeout->deleteTimer(); doorTimeout=NULL;
			doorTimeout=t.setTimeout(doorAlDelay, PLAYSRN);
			} }
				else{
				if(dbg) Serial.println("Door closed");
				}
			break;

			case i_IsoSw:
				if(AlLevel!=_ALL) break;
				if(dbg) Serial.print("IsoSw: ");
				if(s_IsoSw==LOW){
					if(dbg) Serial.println("IsoSwClosed");
					if(AlArmed)
					{
					AlSrc=itrptSrc;
						playSrn();
					}
				}
				else{
					if(dbg) Serial.println("IsoSwOpened");
				}
			break; 

			case i_HbrkSw:
				if(dbg) Serial.print("HBrkSw: ");
				if(s_HbrkSw==LOW)
				{
					if(dbg) Serial.println("HBrkReleased");
					if(AlArmed){
					AlSrc=itrptSrc; 
					playSrn();
					}
				}
				else
				{
					if(dbg) Serial.println("HBrkPulled");
				}
			break;

			default: goto more; break;

		}
		itrptSrc=-1; return;
	}

more:

if(ItrptLevel>=_LIMITED)
	{
		//if(prnt) {Serial.print("Interrupt: "); Serial.print(iSrc); Serial.println(" ...");}

		switch(iSrc)
		{
			case i_TotAlSw:
			case i_PerAlSw:
				if(dbg) Serial.print("AlSw: ");
				if (debounce(i_PerAlSw)==LOW) 
				{
 				if(dbg) Serial.println("limited");
					setAlLevel(_LIMITED); 
						if(!AlArmed) {
						if(alarmTimeout!=NULL) alarmTimeout->deleteTimer(); alarmTimeout=NULL;
						alarmTimeout=t.setTimeout(alarmArmDelay, MANAGEALARM); 
						enableblinkAlLEDFast();
						}
				}
				else if (debounce(i_TotAlSw)==LOW) 
				{
				if(dbg) Serial.println("all");
					setAlLevel(_ALL);
						if(!AlArmed) {
						if(alarmTimeout!=NULL) alarmTimeout->deleteTimer(); alarmTimeout=NULL;
						alarmTimeout=t.setTimeout(alarmArmDelay, MANAGEALARM); 
						enableblinkAlLEDFast();
						}
				}
				else
				{
					if(dbg) Serial.println("none");
					if(alarmTimeout!=NULL) alarmTimeout->deleteTimer(); alarmTimeout=NULL;
					if(doorTimeout!=NULL) doorTimeout->deleteTimer(); doorTimeout=NULL;

					setAlLevel(_NONE);
					manageAlarm(); 
				}


			break;
			
			 

			case i_EmSw:
				if(dbg) Serial.print("EmergencySw: ");
				if(s_EmSw==LOW)
				{
					if(dbg) Serial.println("Emergency Off");
				}
				else
				{
					if(dbg) Serial.println("Emergency On");
					EmergencyON();

				}
			break;

								
			case i_TapSw:
				if(dbg) Serial.print("TapSw: ");
				if(s_TapSw==LOW) TapOn();
				else	TapOff();	
			break;

			default: break;

		}
	}

	


	itrptSrc=-1; return;

}


void AmbulanceC::manageAlarm(void){

	resetSrn();
		resetAl();
if(AlLevel==_NONE) disarm();
else arm();
}

void AmbulanceC::setAlLevel(byte level){
	//if(dbg) {Serial.print("AlLevel:"); Serial.println(level);}
	AlLevel=level;
	lastAlLevel=AlLevel;
}


void AmbulanceC::setItrptLevel(byte level){
	//if(dbg) {Serial.print("ItrptLevel:"); Serial.println(level);}

	ItrptLevel=level;
	lastItrptLevel=ItrptLevel;
}


void AmbulanceC::arm(void){

	if(ItrptLevel!=_NONE) setItrptLevel(_ALL);

	AlArmed=true;

	digitalWrite(o_AlPow, LOW);
	
	enableblinkAlLEDSlow();

	if(prnt) {Serial.print("Alarm Armed!");Serial.println(AlLevel);}

s_IsoSw  =HIGH;  
s_EmSw   =LOW; 
s_HbrkSw =HIGH;
s_TapSw  =HIGH; 

testInterrupts();

	}
void AmbulanceC::disarm(void){

	if(ItrptLevel!=_NONE) setItrptLevel(_LIMITED);

	digitalWrite(o_AlPow, HIGH);
	
	disableblinkAlLED();

	if(prnt) Serial.println("Alarm Disarmed");
	}




//Alarm led///////////////////////////////////////////////////////////////////////////////
void AmbulanceC::blinkAlLED(){
	AlLEDH();
	t.setTimeout(30, ALLEDL);
}

void AmbulanceC::blinkAlLED2(){
	AlLEDH();
	t.setTimeout(80, ALLEDL);
	t.setTimeout(160, ALLEDH);
	t.setTimeout(240, ALLEDL);
}

void AmbulanceC::enableblinkAlLEDFast(void){
	timerblinkAlLEDSlow->disable();
	timerblinkAlLEDMedium->disable();
	
	timerblinkAlLEDFast->enable();

}
void AmbulanceC::enableblinkAlLEDSlow(void){
	timerblinkAlLEDFast->disable();

		if(AlLevel==_ALL) timerblinkAlLEDSlow->enable();
	else if(AlLevel==_LIMITED) timerblinkAlLEDMedium->enable();

}
void AmbulanceC::disableblinkAlLED(void){
	timerblinkAlLEDFast->disable();
	timerblinkAlLEDMedium->disable();
	timerblinkAlLEDSlow->disable();
	digitalWrite(o_AlLED,LOW);
}
void AmbulanceC::AlLEDL(){digitalWrite(o_AlLED,LOW);}
void AmbulanceC::AlLEDH(){digitalWrite(o_AlLED,HIGH);}
//Alarm led///////////////////////////////////////////////////////////////////////////////
void AmbulanceC::playSrn()
{
if(ItrptLevel!=_NONE) MINIMIZE();

if(prnt) {Serial.print("Play siren");}

if(!AlArmed) return;

nAls++; if(nAls>=maxnAls) {if(prnt) Serial.print("nAls>=maxnAls,disarming"); disarm(); return;}

srnOn() ; 
enableblinkScene();

	if(srnTimeout1!=NULL) srnTimeout1->deleteTimer(); srnTimeout1=NULL;
	if(srnTimeout2!=NULL) srnTimeout2->deleteTimer(); srnTimeout2=NULL;


srnTimeout1=t.setTimeout(2*SrnDuration, SRNON);
srnTimeout2=t.setTimeout(3*SrnDuration, SRNOFF);
//t.setTimeout(3*SrnDuration+1000, RESETSRN);
t.setTimeout(3*SrnDuration+1000, RESTORE);

//enableblinkAlLEDFast();
if(prnt) {Serial.print("Pin");Serial.print(AlSrc);}

phone.alarm();
}




void AmbulanceC::playWrn(int WrnDuration) {
srnOn();
t.setTimeout(WrnDuration, SRNOFF);
}

void AmbulanceC::EmergencyON(void){

	SILENCE();
	AlLevel=_LIMITED; manageAlarm();

	playSrn();
}


void AmbulanceC::srnOn(void){
	if(prnt) Serial.println("SrnOn");
digitalWrite(o_SrnOn, HIGH);
t.setTimeout(500, SRNONL);
}

void AmbulanceC::srnOnL(void){
digitalWrite(o_SrnOn, LOW);
}

void AmbulanceC::srnOff(void){
	if(prnt) Serial.println("SrnOff");
digitalWrite(o_SrnOff, HIGH);
	delay(500);
	digitalWrite(o_SrnOff, LOW);
}









//scene///////////////////////////////////////////////////////////////////////////////
void AmbulanceC::blinkScene(void){
	if(prnt) Serial.println("blinkScene()");
	if(!AlArmed) return;

	if(scene==leftScene) RHSceneOn();
	else LHSceneOn();
	scene=!scene;
}
void AmbulanceC::enableblinkScene(void){
	timerblinkScene->enable();
}
void AmbulanceC::disableblinkScene(void){
	timerblinkScene->disable();
	digitalWrite(o_RHScene, LOW);
	digitalWrite(o_LHScene, LOW);
}

void AmbulanceC::LHSceneOn(void){
	if(prnt) Serial.println("LHSceneOn()"); 
	digitalWrite(o_RHScene, LOW);
	digitalWrite(o_LHScene, HIGH);
}
void AmbulanceC::RHSceneOn(void){
	if(prnt) Serial.println("RHSceneOn()");
	digitalWrite(o_LHScene, LOW);
	digitalWrite(o_RHScene, HIGH);
}






//Tap///////////////////////////////////////////////////////////////////////////////////
void AmbulanceC::TapOn(void){digitalWrite(o_Sink, HIGH);
		if(dbg) Serial.println("Tap ON");

tapTimeout=t.setTimeout(60000, TAPOFF);
}
void AmbulanceC::TapOff(){
if(tapTimeout!=NULL) tapTimeout->deleteTimer(); 
			if(dbg) Serial.println("Tap OFF");

		digitalWrite(o_Sink, LOW);}






//Interrupts///////////////////////////////////////////////////////////////////////////////////
bool AmbulanceC::debounce(int pin)
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




void AmbulanceC::pirTampInterrupt(void){itrptSrc=i_pirTamp;}
void AmbulanceC::pirAlInterrupt(void){itrptSrc=i_pirAl;}
void AmbulanceC::TotAlInterrupt(void){itrptSrc=i_TotAlSw;}
void AmbulanceC::PerAlInterrupt(void){itrptSrc=i_PerAlSw;}
void AmbulanceC::DoorSwInterrupt(void){itrptSrc=i_DoorSw;}
//Reports///////////////////////////////////////////////////////////////////////////////////
void AmbulanceC::reportOutputs(MyString * _SMS){
char state[2];
_SMS->append(",Sink06: "); //ULN2-2
itoa(digitalRead(o_Sink),state,10); _SMS->append(state);
_SMS->append(",SolL08: "); //ULN3-4
itoa(digitalRead(o_LHScene),state,10); _SMS->append(state);
_SMS->append(",LHScene: "); //ULN3-3
//rel5v
itoa(digitalRead(o_RHScene),state,10); _SMS->append(state);
_SMS->append(",RHScene: ");
itoa(digitalRead(o_SrnOn),state,10); _SMS->append(state);
_SMS->append(",SrnOff11: ");
itoa(digitalRead(o_SrnOff),state,10); _SMS->append(state);
_SMS->append(",AlPow: ");
itoa(digitalRead(o_AlPow),state,10); _SMS->append(state);
return;
}

void AmbulanceC::reportAl(MyString * _SMS)//30
{
char state;

if(AlArmed){
	if(AlLevel==_ALL) _SMS->append("_ALL");
	else if(AlLevel==_LIMITED) _SMS->append("_LIMITED");
}
else _SMS->append("DisArmed");

if(nAls>0)
{
_SMS->append(" nAls:"); //+6=15
_SMS->appendInt(nAls);//+1=16

/*for(int i=0; i<5; i++){
state=Source[AlSrc][i];
_SMS->append(&state);//+5=30
}*/
}

}




void AmbulanceC::reportState(MyString * _SMS){
char state[2];
_SMS->append("pirAl");
if(digitalRead(i_pirAl)  !=LOW) _SMS->append("+ "); 
else _SMS->append("- ");

_SMS->append(", pirTmp");
if(digitalRead(i_pirTamp)!=LOW) _SMS->append("+ "); 
else _SMS->append("- ");

_SMS->append("Door");
if(digitalRead(i_DoorSw) ==LOW) _SMS->append("+ "); 
else _SMS->append("- ");

_SMS->append(", Hbrk");
if(digitalRead(i_HbrkSw) ==LOW) _SMS->append("+ "); 
else _SMS->append("- ");

_SMS->append(", Iso"); 
if(s_IsoSw  ==LOW) _SMS->append("+ "); 
else _SMS->append("- ");

_SMS->append(", Ign");
if(i_TapSw==LOW)  _SMS->append("+ "); 
else _SMS->append("- ");

if (i_TotAlSw==LOW)   _SMS->append(", TotAlSw:+");
else if (i_PerAlSw==LOW) _SMS->append(", PerAlSw:+");
else _SMS->append(", AlSw-");

return;
}








//Status led///////////////////////////////////////////////////////////////////////////////////
void AmbulanceC::blinkStatusLED(){
	//Serial.println("blink blink");
	StatusLEDON();
	//t.setTimeout(30, STATUSLEDOFF);
}


void AmbulanceC::enableblinkStatusLED(void){
	timerblinkStatusLED->enable();
}

void AmbulanceC::disableblinkStatusLED(void){
	timerblinkStatusLED->disable();
}

void AmbulanceC::StatusLEDON(void) { digitalWrite(o_StatusLED,HIGH);}

void AmbulanceC::StatusLEDOFF(void){digitalWrite(o_StatusLED,LOW);}
