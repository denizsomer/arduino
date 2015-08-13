#include <SimpleTimer.h>
SimpleTimer timerAlarm;

const int power = 13;
const int signal = 12;
const int pirout = 3;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(signal, OUTPUT);   
pinMode(power, OUTPUT);  
  pinMode(pirout, INPUT);   

Serial.begin(9600);

}

// the loop routine runs over and over again forever:
void loop() {
  

int alarm=digitalRead(pirout);
int timerID;

if (alarm==LOW)
  {
    Serial.println(alarm);
    
  
  triggerAlarm();
  delay(4500);
    triggerAlarm();
  delay(9500);
    poweroffAlarm();

  }
 }
 
 void triggerAlarm() {
      digitalWrite(signal, HIGH);   
  delay(500);   
  digitalWrite(signal, LOW);
}

 void poweroffAlarm() {
      digitalWrite(power, HIGH);   
  delay(500);   
  digitalWrite(power, LOW);
}


