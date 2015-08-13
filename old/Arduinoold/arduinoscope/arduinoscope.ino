/*
this goes on your arduino
for use with Processing example SimpleSerialArduinoscope

*/

// holds temp vals

int time=0;
const int dlay=1;
const int  pout=3;

void setup() {
  // set 2
    pinMode(pout, OUTPUT);
 
  
  Serial.begin(115200);  
}

void loop() {  
  if(time < 255) 
  {
    time=time+dlay;
  }
  else
  {
    time=0;
  }

  delay(dlay*100);     
  analogWrite(pout,time);
  

  //digitalWrite(2,LOW);
  for (int i=0;i<6;i++){
    Serial.print(analogRead(i));
    Serial.print(" ");
  }
  
  // read all digital ports, split by " "
  //for (int i=2;i<14;i++){
  //  Serial.print(digitalRead(i));
  //  Serial.print(" ");
//  }
  
  // frame is marked by LF
  Serial.println();
}
