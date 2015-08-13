    //3.7V LiPo Battery Meter
    //3.7V LiPo Battery Meter
    //Use two 1.2 K Ohm resisters as a voltage divider from the positive
    //and negative terminal of the battery to read reference voltage from.
    //we wouldnt want to send to much voltage to our Arduino.
    //Make sure to tie the ground back to the Arduino. Otherwise, expect some odd readings.

    int sensorPin = 0; //set up the sensor pin for reading the voltage
    int sensorValue = 0; //variable to store the value coming from the sensor
 
    void setup()
    {
    //This is the default value, but we can set it anyways
    analogReference(DEFAULT); //5V Reference on UNO
    Serial.begin(9600); //initialize serial communication
    }

    void loop()
    {
    sensorValue = analogRead(sensorPin); 

 
     Serial.print("{Charge,T,");
     Serial.print(sensorValue * 3.0*5.0/1023.);
     Serial.print("}");
     Serial.println();

    delay(60000); //give a delay between readings.
    }

