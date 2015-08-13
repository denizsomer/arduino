
int nPDU=1, PDUStatus=1;
const int R520m=0, K700i=1;
const int phone=K700i;
bool once=false, phnStatus;

const int BUSY=0;
const int FREE=1;

void freephnStatus(){
	
phnStatus=FREE;
}
void busyphnStatus(){

phnStatus=BUSY;
}




void setup()
{
Serial.begin(9600); //initialize serial communication
Serial3.begin(9600); //initialize serial communication

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


		if (digitalRead(2)==LOW) {prepSMS();}
	else if (digitalRead(3)==LOW) {listSMS(); }
	else if (digitalRead(4)==LOW) {readSMS(); }
	else if (digitalRead(5)==LOW) {sendSMS(); }
	else if (digitalRead(6)==LOW) {deleteSMS(); }
        else if (digitalRead(7)==LOW) { freephnStatus();}


	while(Serial3.available())
{

char character = Serial3.read();
Serial.print(character);}





} //loop

void sendnewSMS(){
  
  	if(phnStatus==BUSY) return;
	busyphnStatus();

Serial3.print( "AT*EAPP=0,0,\"sdvsdvsdvsd\"");
Serial3.println( );}

void phoneInitialize(){
phoneWake();
delay(2000);
phoneHello();
delay(2000);
if (phone==K700i) phoneKeyboardMode();
}
void prepSMS() {

if(phnStatus==BUSY) return;
busyphnStatus();

Serial.println("preparing Storage");

Serial3.println("AT+CPMS=\"ME\"");

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
//phonepurgeSMS();
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
//phonepurgeSMS();
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

	if(phnStatus==BUSY) return;
	busyphnStatus();

Serial.println("deleting SMS");
Serial3.print("AT+CMGD=");
Serial3.print(nPDU);
Serial3.println(",0"); 


}

void readSMS() {

	if(phnStatus==BUSY) return;
	busyphnStatus();

Serial.println("reading SMS");
Serial3.print("AT+CMGR=");
Serial3.println(nPDU);
}

void phoneYes(){
if (phone==K700i)Serial3.println("at+ckpd=\"[\"");
else            Serial3.println("at+ckpd=\"S\"");}



