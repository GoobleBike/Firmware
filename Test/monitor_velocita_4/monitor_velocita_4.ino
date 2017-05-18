//The Gooble Bike 2.0!
//determina la velocità in km/h mappando la frequenza in imp/S
//e lo stampa in km/h
//nessuna segnalazione sui led
//non c'è frequenza di campionamento: campiona alla max frequenza consentita da loop
//calcoli:
  //diametro rullo: 4 cm
  //circonferenza rullo: 12,56 cm
  //diametro ruota: 26"
  //circonferenza ruota: 207 cm
  //giri ruota per giro rullo: 0,06
  //velocità km/h: giri ruota * circ. ruota * 60 * 60 /100/1000
  //1 imp/s -> 0,452 km/h
  //86 imp/s -> 38,8 km/h
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
int vel;

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
  Serial.println("The Gooble Bike 2.0: Sensor Monitor Velocità");
}

void loop() {
  //verifica scadenza base dei tempi
  if(millis()-startBase>=TBASE) { //base dei tempi scaduta: invia dato ed attua freno em
    countPulses=constrain(countPulses,0,86);  //limita la frequenza degli impulsi tra 0 ed 86 imp/sec
    vel=map(countPulses,0,86,0,39);           //mappa la frequenza degli impulsi in velocità (vedi calcoli)
    Serial.println(vel);                      //stampa la velocità in km/h
    startBase=millis();                       //avvia nuovo Tbase
    countPulses=0;                            //azzera contatore impulsi
  }
  //campionamento
  sensor=digitalRead(SENSOR);
  //rilevamento fronte di salita
  if ((sensor!=previous)&&(sensor==HIGH)) { //fronte di salita: conta
    countPulses++;    
  }
  //aggiorna stato precedente
  previous=sensor;
}
