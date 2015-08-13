/* How to set up additional interrupts in Arduino (ATMEL 328)
7/7/2010
Louis Van Blarigan
This code uses Pin Change interrupts to switch between two display states.
*/
/* Pin to interrupt map:
* D0-D7 = PCINT 16-23 = PCIE2 = pcmsk2
* D8-D13 = PCINT 0-5 = PCIE0 = pcmsk0
* A0-A5 (D14-D19) = PCINT 8-13 = PCIE1 = pcmsk1
*/
//#include <SoftwareSerial.h> //(Include the needed libraries)
#define interrupt_pin 7 //Define Analog Pin (analog pins are 16-21)
#define interrupt_pin2 8 //Define Analog Pin (analog pins are 16-21)
volatile int bump = 0;
//SoftwareSerial Serial = SoftwareSerial(13, 13); //Change LCD Tx and Rx Pins to pins of our choosing
void setup() {
 
PCICR |= (1<<PCIE2);
PCMSK2 |= (1<<PCINT23);
PCMSK2 |= (1<<PCINT22);
MCUCR = (1<<ISC00) | (1<<ISC01);
PCICR |= (1<<PCIE0);
PCMSK0 |= (1<<PCINT0);
//LCD Configuration
pinMode(13, OUTPUT);
Serial.begin(9600);
Serial.print("?f"); //Sends clear screen command to LCD
pinMode(interrupt_pin, INPUT); //Make pin an input
pinMode(interrupt_pin2, INPUT); //Make pin an input
digitalWrite(interrupt_pin,HIGH); //Enable pullup resistor on Analog Pin
digitalWrite(interrupt_pin2,HIGH); //Enable pullup resistor on Analog Pin
bump = 0;
interrupts();
}
void loop() {
if (bump == 0) {
Serial.println("0");
bump=-1;
//Serial.println("LOW");
}
else if (bump == 2){
Serial.println("2");
bump=-1;
}
}
ISR(PCINT2_vect) {
bump = 2;
}
ISR(PCINT0_vect) {
bump = 0;
}
