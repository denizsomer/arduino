/* Pin to interrupt map:

A8-A15 = PCINT 16-23 = PCIE2 = pcmsk2

PCINT 00 01 02 03 04 05 06 07= PCIE0 = pcmsk0
D     53 52 51 50 10 11 12 13= PCIE0 = pcmsk0

PCINT 08 09 10= PCIE1 = pcmsk1
D     02 15 14= PCIE1 = pcmsk1
*/

volatile int AlSrc=-1;

const int p_ContSw= 50; //Define Analog Pin (analog pins are 16-21)
const int p_SolSw= 52; //Define Analog Pin (analog pins are 16-21)
const int p_TotAlSw= 53; //Define Analog Pin (analog pins are 16-21)
const int p_PerAlSw= 51; //Define Analog Pin (analog pins are 16-21)
volatile int bump = -1;
//SoftwareSerial Serial = SoftwareSerial(13, 13); //Change LCD Tx and Rx Pins to pins of our choosing
void setup() {
	attachInterrupt(0, pirTampInterrupt, CHANGE); //p_pirAl 
attachInterrupt(1, pirAlInterrupt,   CHANGE); //p_pirTamp //MAY MAKE LOW LATER
attachInterrupt(2, DoorSwInterrupt,    CHANGE); //p_AlSw
attachInterrupt(3, HbrkSwInterrupt,  CHANGE); //p_HbrkSw
attachInterrupt(4, IgnInterrupt,     CHANGE); //p_Ign
attachInterrupt(5, IsoSwInterrupt,   CHANGE); //p_IsoSw
PCICR |= (1<<PCIE2);
PCMSK2 |= (1<<PCINT17);
PCMSK2 |= (1<<PCINT18);
PCMSK2 |= (1<<PCINT16);
MCUCR = (1<<ISC00) | (1<<ISC01);
PCICR |= (1<<PCIE0);
PCMSK0 |= (1<<PCINT0);
PCMSK0 |= (1<<PCINT1);
PCMSK0 |= (1<<PCINT2);
PCMSK0 |= (1<<PCINT3);

Serial.begin(9600);

pinMode(2, INPUT);    digitalWrite(2, HIGH);
pinMode(3, INPUT);    digitalWrite(3, HIGH);
pinMode(21, INPUT);    digitalWrite(21, HIGH);
pinMode(20, INPUT);    digitalWrite(20, HIGH);
pinMode(19, INPUT);    digitalWrite(19, HIGH);
pinMode(18, INPUT);    digitalWrite(18, HIGH);

pinMode(p_ContSw, INPUT); //Make pin an input
pinMode(p_SolSw, INPUT); //Make pin an input
pinMode(p_TotAlSw, INPUT); //Make pin an input
pinMode(p_PerAlSw, INPUT); //Make pin an input

digitalWrite(p_ContSw,HIGH); //Enable pullup resistor on Analog Pin
digitalWrite(p_SolSw,HIGH); //Enable pullup resistor on Analog Pin
digitalWrite(p_TotAlSw,HIGH); //Enable pullup resistor on Analog Pin
digitalWrite(p_PerAlSw,HIGH); //Enable pullup resistor on Analog Pin

}
void loop() {
if (bump == 0) {
if (digitalRead(p_TotAlSw)==LOW) Serial.println("p_TotAlSw");
else if (digitalRead(p_PerAlSw)==LOW) Serial.println("p_PerAlSw");
else if (digitalRead(p_ContSw)==LOW) Serial.println("p_ContSw");
else if (digitalRead(p_SolSw)==LOW) Serial.println("p_SolSw");
else  Serial.println("interrupt_????");
bump=-1;
}
else if (bump == 2){
bump=-1;
delay(100);
Serial.print(digitalRead(p_ContSw));
Serial.println(digitalRead(p_SolSw));
/*if (digitalRead(p_ContSw)==LOW) Serial.println("p_ContSw");
else if (digitalRead(p_UTB6)==LOW) Serial.println("p_UTB6");
else if (digitalRead(p_SolSw)==LOW) Serial.println("p_SolSw");
else  Serial.println("interrupt_pinA????");*/
}

if (AlSrc>-1) {Serial.print("AlSrc");Serial.println(AlSrc); AlSrc=-1; }
}

///////interrupt handlers////////////////////////////////////////////////////////////////////////////////
void DoorSwInterrupt(){AlSrc=1;}

void HbrkSwInterrupt(){AlSrc=2;}

void IsoSwInterrupt(){AlSrc=3;}

void IgnInterrupt(){AlSrc=4;}

void pirAlInterrupt(){AlSrc=5; }

void pirTampInterrupt(){AlSrc=0;}


ISR(PCINT2_vect) {
bump = 2;
}
ISR(PCINT0_vect) {
bump = 0;
}
