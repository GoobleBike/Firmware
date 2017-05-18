//The Gooble Bike 2.0!
//Sensor monitor: plot
//rileva la forma d'onda del sensore di velocità
//e lo stampa sulla console per una diagnostica con plotter
//frequenza di campionamento regolabile
//iDP 14/5/2017

//hw Gooble Bike 1.0
#define SENSOR  2
#define BRAKE   5
#define BRAKING 3
#define MOVING  9
#define TCAMP   1

void setup() {
  pinMode(SENSOR,INPUT);
  pinMode(BRAKE,OUTPUT);
  pinMode(BRAKING,OUTPUT);
  pinMode(MOVING,OUTPUT);
  digitalWrite(BRAKE,LOW);
  digitalWrite(BRAKING,LOW);
  digitalWrite(MOVING,LOW);
  Serial.begin(9600);
  while(!Serial);
  Serial.println("The Gooble Bike 2.0: Sensor Monitor plot");
  Serial.print("Frequenza di campionamento: ");
  Serial.println(TCAMP);
}

void loop() {
  int sensor=digitalRead(SENSOR); //legge stato sensore
  digitalWrite(MOVING,sensor);    //riporta stato sensore sul led verde
  Serial.println(sensor);         //stampa stato sensore per plot
  delay(TCAMP);                   //attende intervallo di campionamento
}
