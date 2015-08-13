 #include <WProgram.h>
 #include<Arduino.h>
 #include <SimpleTimer.h>
SimpleTimer t;

// give it a name:
const int led = 13;
const int signal = 2;
int timercheckSMS;

void setup() {                

pinMode(led, OUTPUT);  
pinMode(signal, INPUT);
digitalWrite(signal, HIGH); 

timercheckSMS= t.setInterval(1000, writetoLED);
}

// the loop routine runs over and over again forever:
void loop() {
  t.run();
  
  if(digitalRead(signal)==LOW){
  t.disable(timercheckSMS); 
  }
   else  t.enable(timercheckSMS); 
  
 

 }
 
 void writetoLED(){
 digitalWrite(led, HIGH); 
 delay(200);
 digitalWrite(led, LOW); 
  }
