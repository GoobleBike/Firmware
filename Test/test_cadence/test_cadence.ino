void setup() {
  pinMode(11,INPUT_PULLUP);
  pinMode(13,OUTPUT);
  Serial.begin(250000);
}

void loop() {
  int val=digitalRead(11);
  digitalWrite(13,val);
  Serial.println(val);
  delay(10);
}
