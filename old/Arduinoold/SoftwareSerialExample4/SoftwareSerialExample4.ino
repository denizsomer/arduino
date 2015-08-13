/*
  Software serial multple serial test
 
 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.
 
 The circuit: 
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)
 
 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts, 
 so only the following can be used for RX: 
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69
 
 Not all pins on the Leonardo support change interrupts, 
 so only the following can be used for RX: 
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
 
 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example
 
 This example code is in the public domain.
 
 */
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

//phone
const int lengthSMS=134, delayPhone=1, maxindex=160; //for K700i
//const int lengthSMS=132, delayPhone=5; //for R520m
char phnData[maxindex],  CIND[10]="+CIND: /0", 
//CMGR[10]="+CMGR: /0", 
CMGR[10]="+CMGR: /0",
CPMS[10]="+CPMS: /0",smsReceived[16]="0791447728008/0", cmd[4];
  char character;
  bool isCIND=false,isCMGR=false,isCPMS=false;
  bool ended=false, ending=false, starting=false, started=false;
  int i,j, jCIND, jCMGR, jCPMS, index;
  int phnBattLev, phnSigLev, phnBattWarn, phnCharger, phnService, 
  phnSounder, newMessage, newCall;

void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
 
  
    mySerial.println("AT");
  delay(2000);
  mySerial.println("AT+CPMS=\"ME\"");
  delay(2000);
}

void loop() // run over and over
{

  
  
     // Serial.println(millis()-phoneTime);
  //while((millis()-phoneTime>phoneInterval)&&mySerial.available()) 
 /* while(mySerial.available()) 
  {
      character = mySerial.read();
//Serial.print(character);Serial.print(starting);Serial.println(started);
//start of packet
      if(character=='\n'&&!started) { starting=true; Serial.println("starting");}
      if(character=='+'&&starting&&!started) 
      {
      started=true; 
      starting=false;
         index = 0;
         phnData[index] = '\0';
      Serial.println(" started");
    }
          if(started&&index < maxindex-1)
         {
            phnData[index++] = character;
            phnData[index] = '\0';
         }
         
        
//end of packet
       if(started&&character=='O'&&!ended) {ending=true;  Serial.println("ending");}
      if(started&&character=='K'&&ending&&!ended) {
      ended=true; 
    ending=false; 
  break;
}

         
  }
     
     if(ending) for( int k=0; k<index; k++)     Serial.print(phnData[k]);
     Serial.println();*/
     
  if(started&&ended) {
    for( i=0; i<index; i++)     Serial.print(phnData[i]);
    Serial.println();Serial.print("endof");Serial.println(index);
    started=false; ended=false; index=0;
  }
  

 /*isCIND=false;
 isCMGR=false; 
 isCPMS=false;
 
  if(index>10) 
  {      
  for( i=0; i<index-10; i++)
  {
    jCIND=0; jCMGR=0; jCPMS=0;
    for( j=0; j<10; j++)
    {
  if(phnData[i+j]==CIND[j]) jCIND++;
  if(phnData[i+j]==CMGR[j]) jCMGR++;
  if(phnData[i+j]==CPMS[j]) jCPMS++;
    }
    if(jCIND==7) {isCIND=true; jCIND=i; Serial.println("isCIND"); break;}
    if(jCMGR==7) {isCMGR=true; jCMGR=i; Serial.print(jCMGR);Serial.println("isCMGR"); break;}
    if(jCPMS==7) {isCPMS=true; Serial.println("isCPMS"); break;}
   Serial.print(i);
    Serial.print(",");Serial.print(jCIND);
    Serial.print(",");Serial.print(jCMGR);
    Serial.print(",");Serial.println(jCPMS);
  }

} //index>10
}//started and ended*/

//mySerial.println("AT");
mySerial.println("AT+CMGR=1");
delay(5000);

  /*if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available())
    mySerial.write(Serial.read());*/
}

