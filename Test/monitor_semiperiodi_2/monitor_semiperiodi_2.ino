//The Gooble Bike 2.0!
//detects semiperiods
//determina il semiperiodo della forma d'onda del sensore di velocità
//e lo stampa in unità di campionamento (default 1 mS)
//il led verde lampeggia in toggle tra i semiperiodi
//frequenza di campionamento regolabile
//iDP 14/5/2017

//hw Gooble Bike 1.0
#define SENSOR  2
#define BRAKE   5
#define BRAKING 3
#define MOVING  9
#define TCAMP   1

int sensor;
int previous;
int status;
int countLOW=0;
int countHIGH=0;
void setup() {
  pinMode(SENSOR,INPUT);
  pinMode(BRAKE,OUTPUT);
  pinMode(BRAKING,OUTPUT);
  pinMode(MOVING,OUTPUT);
  digitalWrite(BRAKE,LOW);
  digitalWrite(BRAKING,LOW);
  digitalWrite(MOVING,LOW);
  sensor=digitalRead(SENSOR);
  previous=sensor;
  status=sensor;
  Serial.begin(9600);
  while(!Serial);
  Serial.println("The Gooble Bike 2.0: Sensor Monitor semiperiodi");
  Serial.print("Frequenza di campionamento: ");
  Serial.println(TCAMP);
}

void loop() {
  //campionamento
  sensor=digitalRead(SENSOR);   //legge sensore di velocità
  //rilevamento fronti
  if(sensor!=previous) {        //fronte del segnale
    if(sensor==HIGH) {          //fronte positivo
      //stampa conteggio semiperiodo LOW, inizia conteggio HIGH
      Serial.print("L: ");  
      Serial.println(countLOW);  
      status=HIGH;
      countHIGH=1;
      digitalWrite(MOVING,HIGH);  //toggle led verde
    }
    else { //fronte negativo
      //stampa conteggio semiperiodo HIGH, inizia conteggio LOW
      Serial.print("H: ");  
      Serial.println(countHIGH);  
      status=LOW;
      countLOW=1;      
      digitalWrite(MOVING,LOW); //toggle led verde
    }
  }
  else { //nessun fronte conta TCAMP in base allo stato
    if (status==HIGH) { //incrementa contatore HIGH
      countHIGH++; 
    }
    else { //incrementa contatore LOW
      countLOW++;       
    }
  }
  //aggiorna stato precedente
  previous=sensor;
  //intervallo di campionamento  
  delay(TCAMP);
}
