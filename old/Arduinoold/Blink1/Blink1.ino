 #include <WProgram.h>
 #include<Arduino.h>
 #include <SimpleTimer.h>
SimpleTimer t;

// give it a name:
const int power = 13;
const int signal = 2;
int timercheckSMS;

void setup() {                

pinMode(power, OUTPUT);  
pinMode(signal, INPUT);
digitalWrite(signal, HIGH); 

timercheckSMS= t.setInterval(phnSMSInterval, prepareStorage);
}

// the loop routine runs over and over again forever:
void loop() {
  t.run();
  
  if(digitalRead(signal)==LOW){
  
  
  
  }
 

 }
