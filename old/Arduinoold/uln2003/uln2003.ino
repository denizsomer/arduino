 //nclude <WProgram.h>
 #include<Arduino.h>



// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.

  

 for (int i = 12; i < 24; i++) pinMode(2*i+1, OUTPUT);
 
  

}

// the loop routine runs over and over again forever:
void loop() {
    
 for (int i = 12; i < 24; i++) {
    // turn the pin on:
    digitalWrite(2*i+1, HIGH);  
    delay(500);                  
    // turn the pin off:
    digitalWrite(2*i+1, LOW);  
  delay(500);  
  }  

 }
