/* Pin to interrupt map:

A8-A15 = PCINT 16-23 = PCIE2 = pcmsk2

PCINT 00 01 02 03 04 05 06 07= PCIE0 = pcmsk0
D     53 52 51 50 10 11 12 13= PCIE0 = pcmsk0

PCINT 08 09 10= PCIE1 = pcmsk1
D     02 15 14= PCIE1 = pcmsk1
*/

volatile int AlSrc=-1;

const int interrupt_pinA8= A8; //Define Analog Pin (analog pins are 16-21)
const int interrupt_pinA9 =A9; //Define Analog Pin (analog pins are 16-21)
const int interrupt_pinA10= A10; //Define Analog Pin (analog pins are 16-21)
const int interrupt_pin51= 51; //Define Analog Pin (analog pins are 16-21)
const int interrupt_pin53= 53; //Define Analog Pin (analog pins are 16-21)
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
PCMSK2 |= (1<<PCINT16);
PCMSK2 |= (1<<PCINT17);
PCMSK2 |= (1<<PCINT18);
MCUCR = (1<<ISC00) | (1<<ISC01);
PCICR |= (1<<PCIE0);
PCMSK0 |= (1<<PCINT0);
PCMSK0 |= (1<<PCINT2);

Serial.begin(9600);

pinMode(2, INPUT);    digitalWrite(2, HIGH);
pinMode(3, INPUT);    digitalWrite(3, HIGH);
pinMode(21, INPUT);    digitalWrite(21, HIGH);
pinMode(20, INPUT);    digitalWrite(20, HIGH);
pinMode(19, INPUT);    digitalWrite(19, HIGH);
pinMode(18, INPUT);    digitalWrite(18, HIGH);

pinMode(interrupt_pinA8, INPUT); //Make pin an input
pinMode(interrupt_pinA9, INPUT); //Make pin an input
pinMode(interrupt_pinA10, INPUT); //Make pin an input
pinMode(interrupt_pin51, INPUT); //Make pin an input
pinMode(interrupt_pin53, INPUT); //Make pin an input

digitalWrite(interrupt_pinA8,HIGH); //Enable pullup resistor on Analog Pin
digitalWrite(interrupt_pinA9,HIGH); //Enable pullup resistor on Analog Pin
digitalWrite(interrupt_pinA10,HIGH); //Enable pullup resistor on Analog Pin
digitalWrite(interrupt_pin51,HIGH); //Enable pullup resistor on Analog Pin
digitalWrite(interrupt_pin53,HIGH); //Enable pullup resistor on Analog Pin

}
void loop() {
if (bump == 0) {
bump=-1;
if (digitalRead(interrupt_pin51)==LOW) Serial.print("interrupt_pin51");
if (digitalRead(interrupt_pin53)==LOW) Serial.print("interrupt_pin53");
Serial.println();
}

if (bump == 2){
bump=-1;
if (digitalRead(interrupt_pinA8)==LOW) Serial.print("interrupt_pinA8");
if (digitalRead(interrupt_pinA9)==LOW) Serial.print("interrupt_pinA9");
if (digitalRead(interrupt_pinA10)==LOW) Serial.print("interrupt_pinA10");
Serial.println();
}

if (AlSrc>-1) {Serial.print("AlSrc");Serial.print(AlSrc); AlSrc=-1; Serial.println();}
}

///////interrupt handlers////////////////////////////////////////////////////////////////////////////////
void DoorSwInterrupt(){AlSrc=1;}

void HbrkSwInterrupt(){AlSrc=2;}

void IsoSwInterrupt(){AlSrc=3;}

void IgnInterrupt(){AlSrc=4;}

void pirAlInterrupt(){AlSrc=5; }

void pirTampInterrupt(){AlSrc=0;}


ISR(PCINT2_vect) {
bump = 2; //AnalogInterruptTriggered
}
ISR(PCINT0_vect) {
bump = 0; //AlSwTriggered=true;
}
