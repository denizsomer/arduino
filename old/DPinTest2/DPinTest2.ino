/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:

int led, l0=0, li=54, lm=27;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  Serial.begin(9600);
  for (led = l0; led < li; led++){
  pinMode(led, INPUT);   digitalWrite(led, HIGH);  
  }
}

// the loop routine runs over and over again forever:
void loop() {
  
  for (led = l0; led < lm; led++) {Serial.print(led); Serial.print(":"); Serial.print(digitalRead(led));  Serial.print(", "); }   
  Serial.println();
  for (led = lm; led < li; led++) {Serial.print(led); Serial.print(":"); Serial.print(digitalRead(led));  Serial.print(", "); }   
  Serial.println();
  delay(1000);               // wait for a second
  
}
