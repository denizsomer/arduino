int aPin=A15;
int ledPin=13;

unsigned long duration=-1;
bool shutterisopen, shutterwasopen;


// the setup function runs once when you press reset or power the board
void setup() {

  pinMode(aPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  
  shutterwasopen=(analogRead(aPin)*100>10);

}

// the loop function runs over and over again forever
void loop() {
  
  shutterisopen=(analogRead(aPin)>2);
  
  if(shutterisopen){
    if(!shutterwasopen)  duration=millis();
    else digitalWrite(ledPin, HIGH);
    shutterwasopen=true;
    }
  else{ //shutter closed
  digitalWrite(ledPin, LOW);
    if(shutterwasopen) {
    Serial.println(millis()-duration);
    //duration=-1;
    }
    shutterwasopen=false;
    }
  }
 //if(duration!=-1) digitalWrite(ledPin, HIGH);
 
 //Serial.println(100*analogRead(aPin));



