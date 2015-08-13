 #include <WProgram.h>
 #include<Arduino.h>
/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
char SMS[140];
int lengthSMS=0;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
 Serial.begin(9600);
 deleteSMS

}

// the loop routine runs over and over again forever:
void loop() {
  

  if(lengthSMS<10 )appendtoSMS("layn", 4);
  else deleteSMS();

  Serial.print("buffer");
  Serial.println(SMS);
  
 delay(5000); 
 }
 
 void appendtoSMS(char *partSMS, int len){
 if(lengthSMS+len>=140) return;
 if(lengthSMS>0)lengthSMS--;
   strcpy(&SMS[lengthSMS], partSMS);
   lengthSMS+=len;
   SMS[lengthSMS++]='\0';
 }
 
  void deleteSMS(){
   strcpy(SMS, "");
   lengthSMS=0;
   SMS[1]='\0';
 }
