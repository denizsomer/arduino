int motorPin1 = 13;
int motorPin2 = 12;
int motorPin1b = 11;
int motorPin2b = 10;
int delayTime = 500;

void setup() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin1b, OUTPUT);
  pinMode(motorPin2b, OUTPUT);
}

void loop() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin1b, LOW);
  digitalWrite(motorPin2b, HIGH);
  delay(delayTime);
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin1b, LOW);
  digitalWrite(motorPin2b, LOW);
  delay(delayTime);
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin1b, HIGH);
  digitalWrite(motorPin2b, LOW);
  delay(delayTime);
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin1b, HIGH);
  digitalWrite(motorPin2b, HIGH);
 delay(delayTime);
}
