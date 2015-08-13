/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:

int led, l0=A0, li=A15;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  for (led = l0; led < li; led++){
  pinMode(led, OUTPUT);     
  }
}

// the loop routine runs over and over again forever:
void loop() {
  
  for (led = l0; led < li; led++) analogWrite(led, 100);   // turn the LED on (HIGH is the voltage level)
  delay(200);               // wait for a second
  for (led = l0; led < li; led++) analogWrite(led, 100);    // turn the LED off by making the voltage LOW
  delay(200);   
}
