#include <SimpleTimer.h>
SimpleTimer timer;

    void setup()
    {
    Serial.begin(9600); //initialize serial communication
    Serial1.begin(9600); //initialize serial communication
    Serial2.begin(9600); //initialize serial communication
    Serial3.begin(9600); //initialize serial communication
    
//timer.setInterval(2000, listentoCall);
//sendSMS("lan");

    }

    void loop()
    {
      //timer.run();
      
      //startCall("+907772259300");
      Serial.print("AT+CKPD=\"s\"");
      delay(2000);
 
    
    }
    
    void listentoCall() {
      Serial1.println("AT+CPAS");
}
       void answerCall() {
      Serial1.println("ATA");
} 

    void sendSMS(String msgTxt ) {
      
String msg = "AT*EAPP=0,0,\"";
msg+=msgTxt;
msg += "\"";
Serial1.println(msg);

delay(3000);

Serial1.println("AT+CKPD=\"S\"");

delay(3000);

Serial1.println("AT+CKPD=\"07772259300\"");

delay(3000);

Serial1.println("AT+CKPD=\"S\"");
}

    void startCall(String callTxt ) {

String msg = "AT+CKPD=\"";
msg+=callTxt;
msg += "\"";
Serial1.println(msg);

delay(3000);

//Serial1.println("AT+CKPD=\"S\"");
}


