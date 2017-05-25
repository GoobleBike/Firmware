void setup() {
  pinMode(2,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP);
  pinMode(8,INPUT_PULLUP);
  
  pinMode(3,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(12,OUTPUT);
  digitalWrite(5,LOW);
  //lamp test
  digitalWrite(3,HIGH);
  digitalWrite(9,HIGH);
  digitalWrite(12,HIGH);
  delay(500);
  digitalWrite(3,LOW);
  digitalWrite(9,LOW);
  digitalWrite(12,LOW);
  delay(500);
}

void loop() {
  int sensor;
//test switches
  int sw1=!digitalRead(6);
  int sw2=!digitalRead(7);
  int sw3=!digitalRead(8);
  digitalWrite(3,sw1);
  digitalWrite(9,sw2);
  digitalWrite(12,sw3);
////loopback
//  digitalWrite(5,HIGH);
//  delay(150);
//  sensor=digitalRead(2);
//  digitalWrite(3,sensor);
//  digitalWrite(5,LOW);
//  delay(150);
//  sensor=digitalRead(2);
//  digitalWrite(3,sensor);
////pwm
//  analogWrite(5,32);
//  sensor=digitalRead(2);
//  digitalWrite(3,sensor);  
}
