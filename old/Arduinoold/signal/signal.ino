/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
const int pout = 12;
int time=0;
const int dlay=100;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  Serial.begin(9600);   
}

// the loop routine runs over and over again forever:
void loop() {
  
  if(time < 255) 
  {
    time=time+1;
  }
  else
  {
    time=0;
  }
  Serial.println(time);
  analogWrite(pout,-time);
//delay(2000);// wait for a second
}
