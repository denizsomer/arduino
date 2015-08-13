/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 18;



// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, INPUT);  
 digitalWrite(led,HIGH); 
Serial.begin(9600);  
}

// the loop routine runs over and over again forever:
void loop() {
  Serial.println(digitalRead(led));    // turn the LED off by making the voltage LOW
}
