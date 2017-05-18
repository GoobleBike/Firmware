//The Gooble Bike 2.0!
//pulse frequency
//determina la frequenza degli impulsi su una base dei tempi (default 1 S)
//e lo stampa in imp/S
//nessuna segnalazione sui led
//non c'è frequenza di campionamento: campiona alla max frequenza consentita da loop
//iDP 14/5/2017

//hw Gooble Bike 1.0
#define SENSOR  2
#define BRAKE   5
#define BRAKING 3
#define MOVING  9
#define TBASE   1000

int sensor;
int previous;
int countPulses=0;
long startBase;

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
  startBase=millis();
  Serial.begin(9600);
  while(!Serial);
  Serial.println("The Gooble Bike 2.0: Sensor Monitor Frequenza impulsi");
}

void loop() {
  //verifica scadenza base dei tempi
  if(millis()-startBase>=TBASE) { //base dei tempi scaduta: invia dato
    Serial.println(countPulses);  //stampa imp/Tbase
    startBase=millis();           //avvia nuovo Tbase
    countPulses=0;                //azzera contatore
  }
  //campionamento
  sensor=digitalRead(SENSOR);     //legge stato sensore
  //rilevamento fronte di salita
  if ((sensor!=previous)&&(sensor==HIGH)) { //fronte di salita: conta impulso
    countPulses++;    
  }
  //aggiorna stato precedente
  previous=sensor;
}
