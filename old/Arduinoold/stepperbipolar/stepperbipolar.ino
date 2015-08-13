int motorPin1a = 10;
int motorPin2a = 11;
int motorPin1b = 12;
int motorPin2b = 13;
int delayTime = 5;
int  dir=0;

void setup() {
  pinMode(motorPin1a, OUTPUT);
  pinMode(motorPin2a, OUTPUT);
  pinMode(motorPin1b, OUTPUT);
  pinMode(motorPin2b, OUTPUT);

}

void loop() {
  if(dir==1){
  digitalWrite(motorPin1a, HIGH);
  digitalWrite(motorPin2a, LOW);
  digitalWrite(motorPin1b, LOW);
  digitalWrite(motorPin2b, LOW);
  delay(delayTime);
  digitalWrite(motorPin1a, LOW);
  digitalWrite(motorPin2a, HIGH);
  digitalWrite(motorPin1b, LOW);
  digitalWrite(motorPin2b, LOW);
  delay(delayTime);
  digitalWrite(motorPin1a, LOW);
  digitalWrite(motorPin2a, LOW);
  digitalWrite(motorPin1b, HIGH);
  digitalWrite(motorPin2b, LOW);
  delay(delayTime);
  digitalWrite(motorPin1a, LOW);
  digitalWrite(motorPin2a, LOW);
  digitalWrite(motorPin1b, LOW);
  digitalWrite(motorPin2b, HIGH);
 delay(delayTime);
  }
    else if(dir==0){
   digitalWrite(motorPin1a, LOW);
  digitalWrite(motorPin2a, LOW);
  digitalWrite(motorPin1b, LOW);
  digitalWrite(motorPin2b, LOW);   
  }
  else {
  digitalWrite(motorPin1a, HIGH);
  digitalWrite(motorPin2a, LOW);
  digitalWrite(motorPin1b, LOW);
  digitalWrite(motorPin2b, LOW);
  delay(delayTime);
  digitalWrite(motorPin1a, LOW);
  digitalWrite(motorPin2a, LOW);
  digitalWrite(motorPin1b, LOW);
  digitalWrite(motorPin2b, HIGH);
  delay(delayTime);
  digitalWrite(motorPin1a, LOW);
  digitalWrite(motorPin2a, LOW);
  digitalWrite(motorPin1b, HIGH);
  digitalWrite(motorPin2b, LOW);
  delay(delayTime);
  digitalWrite(motorPin1a, LOW);
  digitalWrite(motorPin2a, HIGH);
  digitalWrite(motorPin1b, LOW);
  digitalWrite(motorPin2b, LOW);
  delay(delayTime);
  }

}
