/*
  DigitalReadSerial
 Reads a digital input on pin 2, prints the result to the serial monitor 
 
 This example code is in the public domain.
 */
 //phone
const int lengthSMS=134, delayPhone=1; //for K700i
//const int lengthSMS=132, delayPhone=5; //for R520m
char fromPhone[160], CIND[10]="+CIND: /0", CMGR[10]="+CMGR: /0", smsReceived[16]="0791447728008/0", cmd[4];
  char character;
  int nfromPhone, i,j, jCIND=0, jCMGR;
  int battLevel,sigLevel,battWarning, newMessage, newCall;

int pushButton = 2;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial3.begin(9600);
  
  Serial3.println("AT+CPMS=\"ME\"");

}

// the loop routine runs over and over again forever:
void loop() {
  
      nfromPhone=0;
      bool ended=false, ending=false;
 // while(1){
  while(Serial3.available()) 
  {
      character = Serial3.read();
      if(character=='K') ending=true;
      if(character=='\n'&&ending==true) {ended=true; ending=false; break;}
      else{
      fromPhone[nfromPhone++]=character;
      ended=false;
      }
      //delay(delayPhone);
 
  }
        /* nfromPhone = 0;
         fromPhone[nfromPhone] = '\0';
         char inChar, lastChar=' ';
         bool ended=false;
         
while(Serial3.available() > 0)
   {
      char inChar = Serial.read();


       if(
       //lastChar=='O'&&
       inChar == 'K')
      {
         ended = true;
         break;
      }
      else
      {
         if(nfromPhone < 160-1) // Array size
         {
            fromPhone[nfromPhone++] = inChar;
            fromPhone[nfromPhone] = '\0';
            lastChar= inChar;
         }
      }
   }*/

   if(ended)
   {
      for( i=0; i<nfromPhone; i++)     Serial.print(fromPhone[i]);
      Serial.println();
      Serial.print("ended");
      Serial.println(nfromPhone);
      Serial.println();
   }
  


  Serial3.println("AT+CIND?");
  delay(1000);        // delay in between reads for stability
  
  
}



