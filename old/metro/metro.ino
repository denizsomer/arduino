#define encoder0PinA 2
#define encoder1PinA 3
volatile bool encA = false, encB = false;
#define encoder0PinB 4
#define encoder1PinB 5

int encoder0Pos=0, encoder1Pos = 0;

int dly=0;
volatile long lastTime;

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int resetPin = 8;  

int outputValue = 0;        // value output to the PWM (analog out)
double bpm=100.;

void setup() { 
	pinMode(encoder0PinA, INPUT);
	pinMode(encoder0PinB, INPUT);
	pinMode(encoder1PinA, INPUT);
	pinMode(encoder1PinB, INPUT);
	pinMode(resetPin, INPUT); digitalWrite(resetPin, HIGH);

	attachInterrupt(0, doEncoderA, CHANGE);
	attachInterrupt(1, doEncoderB, CHANGE); 
	Serial.begin (9600);
pinMode(13, OUTPUT);
}

void loop() {

	if(encA)
{
	if (digitalRead(encoder0PinA) == HIGH)
	{
		if (digitalRead(encoder0PinB) == LOW) encoder0Pos ++;	// CW 
		else								  encoder0Pos --; // CCW 
	} 
	else
	{
		if (digitalRead(encoder0PinB) == HIGH) encoder0Pos --; // CW
		else								   encoder0Pos ++; // CCW 
	}
	encA=false;
}

if(encB)
{
	if (digitalRead(encoder1PinA) == HIGH)
	{
		if (digitalRead(encoder1PinB) == LOW) encoder1Pos ++;	// CW 
		else								  encoder1Pos --; // CCW 
	} 
	else
	{
		if (digitalRead(encoder1PinB) == HIGH) encoder1Pos --; // CW
		else								   encoder1Pos ++; // CCW 
	} 
	encB=false;
}

	/*if(digitalRead(resetPin)==LOW){
	lastTime=millis()-dly;
	Serial.println("rset");
	}*/


  //bpm=100+encoder0Pos+encoder1Pos*0.1;

  //dly=1000;//(60./double(bpm))*1000;

//if(millis()-lastTime>dly)
//{
//	tone(13, 440, 20) ;
Serial.print("(");Serial.print(encoder0Pos);Serial.print("),(");
Serial.print(encoder1Pos); Serial.println(") ");//Serial.println(bpm);
//lastTime=millis();
//}
}

void doEncoderA(){encA=true;}
void doEncoderB(){encB=true;}

