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

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  Serial.begin(9600);

  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  Serial3.begin(9600);
  Serial3.println("AT");
  delay(2000);
  Serial3.println("AT+CPMS=\"ME\"");
  delay(2000);    
}

// the loop routine runs over and over again forever:
void loop() {

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
 
}
