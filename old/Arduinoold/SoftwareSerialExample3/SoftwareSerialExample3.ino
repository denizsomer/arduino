
//#include <SoftwareSerial.h>

//SoftwareSerial Serial3(10, 11); // RX, TX

//phone
const int lengthSMS=134, delayPhone=1, maxindex=160; //for K700i
//const int lengthSMS=132, delayPhone=5; //for R520m
char phnData[maxindex],  CIND[10]="+CIND: /0", EOP[7]="\nOK/0",  ERR[10]="ERROR: /0", 
//CMGR[10]="+CMGR: /0", 
CMGR[10]="+CMGR: /0",
CPMS[10]="+CPMS: /0",smsReceived[16]="0791447728008/0", cmd[4];
  char character;
  bool isCIND=false,isCMGR=false,isCPMS=false;
  bool ended=false, ending=false, starting=false, started=false;
  int i,j, jCIND=0, jCMGR=0, jCPMS=0, index, jEOP=0, jERR=0;
  int phnBattLev, phnSigLev, phnBattWarn, phnCharger, phnService, 
  phnSounder, newMessage, newCall;

void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  Serial3.begin(9600);
  Serial3.println("AT");
  delay(2000);
  Serial3.println("AT+CPMS=\"ME\"");
  delay(2000);
}

void loop() // run over and over
{
  
        
     // Serial.println(millis()-phoneTime);
  //while((millis()-phoneTime>phoneInterval)&&Serial3.available()) 
  while(Serial3.available()) 
  {
      character = Serial3.read();
//Serial.print(character);Serial.print(starting);Serial.println(started);
//start of packet
      if(character==CIND[jCIND]&&!started) { jCIND++; Serial.print(character);Serial.println(jCIND);  Serial.println("starting CIND");}
      if(character==CMGR[jCMGR]&&!started) { jCMGR++; Serial.print(character);Serial.println(jCMGR);  Serial.println("starting CMGR");}
      if(character==CPMS[jCPMS]&&!started) { jCPMS++; Serial.print(character);Serial.println(jCPMS);  Serial.println("starting CPMS");}
      if(character==ERR[jERR]) { jERR++; Serial.print(character);Serial.println(jERR);  Serial.println("erroring");}
      if(character==EOP[jEOP]&&!ended&&started)   { jEOP++; Serial.print(character);Serial.println(jEOP);  Serial.println("ending");}
      if(jERR>5){
        
        Serial.print(jERR);Serial.println(" ------------------ERROR!!!!------------------");
        started=false;
        ended=false;
          jCIND=0; jCMGR=0; jCPMS=0; jEOP=0; jERR=0;
      }
      if((jCPMS==7||jCIND==7||jCMGR==7)&&!started) 
      {
      started=true; 
         index = 0;
         phnData[index] = '\0';
      Serial.print(jCIND);Serial.print(jCMGR);Serial.print(jCPMS);Serial.println(" ------------------started------------------");
    }
          if(started&&(index < maxindex-1))
         {
            phnData[index++] = character;
            phnData[index] = '\0';
         }
         
        
//end of packet
      if(jEOP==3&&!ended) {
      ended=true; 
      
      Serial.println("---------------ended----------------------");
  break;
}

         
  }
     
     if(jEOP>0) {Serial.print(index);Serial.print("ENDING:"); for( int k=0; k<index; k++)     Serial.print(phnData[k]);
     Serial.println();}
     
  if(started&&ended) {
    Serial.print("ENDED:"); for( i=0; i<index; i++)     Serial.print(phnData[i]);
    Serial.println();Serial.print("endof");Serial.println(index);
    started=false; ended=false;
        
  jCIND=0; jCMGR=0; jCPMS=0; jEOP=0; jERR=0;

 isCIND=false;
 isCMGR=false; 
 isCPMS=false;
 }//started and ended
 
  /*if(index>10) 
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

} //index>10 */



//Serial3.println("AT+CPMS=\"ME\"");
//delay(5000);
Serial3.println("AT+CMGR=1");
delay(10000);

  /*if (Serial3.available())
    Serial.write(Serial3.read());
  if (Serial.available())
    Serial3.write(Serial.read());*/
}

