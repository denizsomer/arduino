

int rPin=2; //interrupt 0
int aPin=A15;
int ledPin=13;

volatile bool inter;
const unsigned int deldur=5;
unsigned long duration=-1;


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(rPin, INPUT);
  pinMode(aPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  
  inter=false;
  attachInterrupt(0, doSomething, CHANGE); //p_pirAl 
}

// the loop function runs over and over again forever
void loop() {

  if(inter){
    delay(deldur);
    if(duration!=-1&&analogRead(aPin)==0){
    Serial.println(millis()-duration);
    duration=-1;
    }
    else duration=millis();
    
    Serial.println("!");
    inter=false;
  }
 if(duration!=-1) digitalWrite(ledPin, HIGH);
}

void doSomething(void){
inter=true;
}
