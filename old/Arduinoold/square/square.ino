int motorPin1 = 12;
int motorPin2 = 11;
int motorPin3 = 10;
int motorPin4 = 9;
int motorPin5 = 8;
int motorPin6 = 7;

int fre = 10;

void setup() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(motorPin5, OUTPUT);
  pinMode(motorPin6, OUTPUT);
}

void loop() {
  digitalWrite(motorPin4,LOW);
  digitalWrite(motorPin5,LOW);
  digitalWrite(motorPin1,HIGH);
  delay(fre);
  digitalWrite(motorPin1,LOW);
  digitalWrite(motorPin2,HIGH);
  digitalWrite(motorPin6,HIGH);
  delay(fre);
  digitalWrite(motorPin2,LOW);
  digitalWrite(motorPin6,LOW);
  digitalWrite(motorPin3,HIGH);
  delay(fre);
  digitalWrite(motorPin3,LOW);
  digitalWrite(motorPin4,HIGH);
  digitalWrite(motorPin5,HIGH);
  delay(fre);
}
