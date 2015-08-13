#include <SimpleTimer.h>
SimpleTimer t;



//phone variables
const int R520m=0;
const int K700i=1;
const int phone=K700i;

const int BUSY=0;
const int FREE=1;

//1000 1sec
//60000 1min
//3600000 1hr
const unsigned int phnPDUInterval=10000, maxindex=250, maxSMS=140;

const unsigned long phnStatusInterval=3600001, phoneDiscardInterval=60000;

const char CIND[9]="+CIND:/0", CMGL[9]="+CMGL:/0", CMGR[9]="+CMGR:/0", myPDU[13]="7727523900/0",
CPMS[9]="+CPMS:/0", RING[7]="RING/0", CMGD[7]="CMGD/0" ;

char SMS[maxSMS], cmd[4], PDU[maxindex], character;

int whichBattIsCharged, nPDU,lengthSMS,PDUStatus, counterSerial,
jCIND,  jCMGL,  jCMGR,  jCMGD,  jCPMS,  jRING,
jCIND0, jCMGL0, jCMGR0, jCMGD0, jCPMS0, jRING0,
i,j, index, phnBattLev, phnSigLev, phnBattWarn, phnCharger, phnService, phnStatus,
phnSounder, newMessage, newCall, timercheckSMS, timercheckStatus;

bool isCIND, isCMGD, isCMGL, isCMGR, isCPMS, isRING, ended, ending, started;

unsigned long phoneTime;


void setup()
{
Serial.begin(9600); //initialize serial communication
Serial3.begin(38400); //initialize serial communication

//timertoggleSolar=t.setInterval(SolSwInterval, toggleSol);
/*timerVoltage=t.setInterval(readVoltageInterval, readwriteVoltages);
timercheckSMS= t.setInterval(phnPDUInterval, prepareStorage);
timercheckStatus=t.setInterval(phnStatusInterval, checkStatus);
timerReset=t.setInterval(resetInterval, resetVariables);*/

resetVariables();

}

///////////resets///////////////////////////////////////////////////////////////////
void resetVariables(){
Serial.println("resetting...........................");
delay(2000);
//t.setInterval(20000, takeandsendPicture);
//t.setInterval(10000, sendReport);

//Listening, pull-ups
pinMode(2, INPUT);    digitalWrite(2, HIGH);
pinMode(3, INPUT);    digitalWrite(3, HIGH);
pinMode(4, INPUT);    digitalWrite(4, HIGH);
pinMode(5, INPUT);    digitalWrite(5, HIGH);
pinMode(6, INPUT);    digitalWrite(6, HIGH);


resetSerial();
resetPhn();
}


void resetPhn(){
	phoneTime=0;
freephnStatus();
resetSMS();
resetPDU();
phoneInitialize();
delay(2000);
prepareStorage();
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


}
void resetPDU(){
index=0;
PDU[index]='\0';
PDUStatus=-1;
nPDU=-1;
}

void phoneInitialize(){
phoneWake();
delay(2000);
phoneHello();
delay(2000);
if (phone==K700i) phoneKeyboardMode();
}
void prepareStorage() {

//if(phnStatus==BUSY) return;
//busyphnStatus();

Serial.println("preparing Storage");

Serial3.println("AT+CPMS=\"ME\"");

}


void freephnStatus(){
	
phnStatus=FREE;
}
void busyphnStatus(){

phnStatus=BUSY;
}
void phoneWake(){Serial3.println("at+ckpd=\"P\"");}
void phoneHello(){Serial3.println("AT");}
void phoneKeyboardMode() {Serial3.println( "AT+CSCS=\"8859-1\"");}
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

	//if(phnStatus==BUSY) return;
	//busyphnStatus();

Serial.println("listing SMS");
Serial3.println("AT+CMGL=4");
}
void deleteSMS() {

	//if(phnStatus==BUSY) return;
	//busyphnStatus();

Serial.println("deleting SMS");
Serial3.print("AT+CMGD=");
Serial3.print(nPDU);
Serial3.println(",0"); 


}

void readSMS() {

	//if(phnStatus==BUSY) return;
	//busyphnStatus();

Serial.println("reading SMS");
Serial3.print("AT+CMGR=");
Serial3.println(nPDU);
}
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


void phoneYes(){
if (phone==K700i)Serial3.println("at+ckpd=\"[\"");
else            Serial3.println("at+ckpd=\"S\"");}

///////loop////////////////////////////////////////////////////////////////////////////////
void loop()
{

	if (digitalRead(2)==LOW) {prepareStorage(); delay(2000);}
	else if (digitalRead(3)==LOW) {listSMS(); nPDU=2; delay(2000);}
	else if (digitalRead(4)==LOW) {readSMS(); delay(2000);}
	else if (digitalRead(5)==LOW) {sendSMS(); delay(2000);}
	else if (digitalRead(6)==LOW) {deleteSMS(); delay(2000);}


	while(Serial3.available())
{

character = Serial3.read();
Serial.print(character);}



} //loop





