#include <SimpleTimer.h>
#include <SoftwareSerial.h>

SoftwareSerial Phone(10, 11); // RX, TX

int ncmd=0;
SimpleTimer timer;

    void setup()
    {

    Serial.begin(9600); //initialize serial communication
    Phone.begin(9600); //initialize serial communication
   /* Serial2.begin(9600); //initialize serial communication
    Serial3.begin(9600); //initialize serial communication*/
    
    timer.setInterval(5000, checkStatus);
//timer.setInterval(5000, listentoCall);
//timer.setInterval(10000, listentoSMS);

Phone.println("AT+CPMS=\"ME\"");
//sendSMS("lan");
Phone.println("AT+CIND=?");
    }

    void loop()
    {

      timer.run();
     // timerSMS.run();
      
      //readSerial(content);
      
      //void readSerial(String content)

char fromPhone[160], CIND[10]="+CIND: /0", CMGR[10]="+CMGR: /0", smsReceived[16]="0791447728008/0", cmd[4];
  char character;
  int nfromPhone, i,j, jCIND=0, jCMGR;
  int battLevel,sigLevel,battWarning, newMessage, newCall;
  
  nfromPhone=0;
 // while(1){
  while(Phone.available()) 
  {
      character = Phone.read();
      //if(character=='\n') {
      //Serial.println("Enter");
  //break;}
      fromPhone[nfromPhone++]=character;
      delay(5);
 
  }
  //  else break;
 // }
  

  if(nfromPhone>10) 
  {

  
  for( i=0; i<nfromPhone-10; i++)
  {
    jCIND=0, jCMGR=0;
    for( j=0; j<10; j++)
    {
  if(fromPhone[i+j]==CIND[j]) jCIND++;
  if(fromPhone[i+j]==CMGR[j]) jCMGR++;
    }
    if(jCIND==7) {
    jCIND=i; 
  Serial.print("jCIND=");Serial.println(jCIND);
  break;
}
    if(jCMGR==7) {
  jCMGR=i;
  break;
}
}

  
    }
    
    if(nfromPhone==132&&jCMGR>0) {
  i=jCMGR+14;
  while( fromPhone[i]!='\n'&&i<nfromPhone) i++;
  for( j=i-5; j<i; j++) cmd[j-i+5]=fromPhone[j]; 
  ncmd=encode(cmd);
  Serial.print("ncmd");
  Serial.println(ncmd);
  deleteSMS();
    }
  
  
  
  
  
  
  if(jCIND>0) 
  {
     
     battLevel=atoi(&fromPhone[7]);
     sigLevel=atoi(&fromPhone[9]);
     battWarning=atoi(&fromPhone[11]);
     newMessage=atoi(&fromPhone[19]);
     newCall=atoi(&fromPhone[21]);
    /* Serial.print(battLevel);
     Serial.print(sigLevel);
     Serial.print(battWarning);
     Serial.print(newMessage);
  Serial.println(newCall);*/
}
  
   /* i=0;
  while(i<10&&fromPhone[i]==cmgr[i]) i++;
  if(i>3)  {Serial.print(i);Serial.println("SIIEMCIARRR");}*/
 
  if(newMessage>0) 
 {timer.disable(0);
 readSMS();
 }
  
  
  }
    
  
  /*String content = "";
  char character;
  
  while(1){
  if(Phone.available()) 
  {
      character = Phone.read();
      if(character=='\n') {
      //Serial.println("Enter");
  break;}
      content+=character;
      delay(10);
 
  }
    else break;
  }
  
  content.trim();
  
  if(content != "") Serial.println(content);
  
 // if (content.startsWith("+CPAS:")&&content.endsWith("3")) {
   if(content=="RING"){
  //timer.disable(0);
  answerCall();
}
  //if (content.startsWith("+CPAS:")&&content.endsWith("4")) Serial.println("BUSY"); 
  
    if (content.startsWith("+CMGR:")) {
    Serial.print("Bu:");
    //  timerCall.disable(0);
    //timerSMS.disable(0);
  Serial.println(content);
//ayikla
//sil


deleteSMS();

      delay(2000);
      sendSMS("lan");
      delay(10000);
      */

    
    void listentoCall() {
      Phone.println("AT+CPAS");
}

    void checkStatus() {
      Phone.println("AT+CIND?");
}

    void readSMS() {
      Phone.println("AT+CPMS=\"ME\"");
      delay(1000);
      Phone.println("AT+CMGR=1");

}

    void deleteSMS() {
      Phone.println("AT+CMGD=1");
}


       void answerCall() {
      Phone.println("ATA");
} 

    void sendSMS(String msgTxt ) {
      
String msg = "AT*EAPP=0,0,\"";
msg+=msgTxt;
msg += "\"";
Phone.println(msg);

delay(3000);

Phone.println("AT+CKPD=\"S\"");

delay(3000);

Phone.println("AT+CKPD=\"07772259300\"");

delay(3000);

Phone.println("AT+CKPD=\"S\"");
}


int encode(char* cmd)		
{

int num;

     if( cmd[1] =='0') num =  0;
else if( cmd[1] =='1') num = 10;
else if( cmd[1] =='2') num = 20;
else if( cmd[1] =='3') num = 30;
else if( cmd[1] =='4') num = 40;
else if( cmd[1] =='5') num = 50;
else if( cmd[1] =='6') num = 60;
else if( cmd[1] =='7') num = 70;
else if( cmd[1] =='8') num = 80;
else if( cmd[1] =='9') num = 90;
	
		
if( cmd[3] =='8')	{if( cmd[0] =='3')	num+=0;
	                if( cmd[0] =='B')	num+=1;}
else if( cmd[3] =='9')	{if( cmd[0] =='3')	num+=2;
	                if( cmd[0] =='B')	num+=3;}
else if( cmd[3] =='A')	{if( cmd[0] =='3')	num+=4;
	                if( cmd[0] =='B')	num+=5;}
else if( cmd[3] =='B')	{if( cmd[0] =='3')	num+=6;
	                if( cmd[0] =='B')	num+=7;}
else if( cmd[3] =='C')	{if( cmd[0] =='3')	num+=8;
	                if( cmd[0] =='B')	num+=9;}
else		num=-1;
		
return num;		
}		

