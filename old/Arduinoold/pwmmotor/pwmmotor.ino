/*
this goes on your arduino
for use with Processing example SimpleSerialArduinoscope

*/

// holds temp vals

int time=0;
const int dlay=1;
const int  pout=12;
const int  pin=7;

void setup() {
  // set 2
    pinMode(pout, OUTPUT);
    pinMode(pin, INPUT);
 
  
  Serial.begin(9600);  
}

void loop() {  
//int brightness = map( analogRead(0),0,1023, 0, 255);
//int brightness = analogRead(0)/10.;
//int brightness = digitalRead(pin);
 // Serial.println(brightness);
  //delay(dlay);     
  //if(brightness==HIGH)
  analogWrite(pout,1055);
  //delay(1000);
  //analogWrite(pout,0);
  //delay(100); 

  }
  

