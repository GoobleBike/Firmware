//The Gooble Bike 2.0!
//test del freno attua la frenata in modo direttamente proporzionale alla velocità
//stampa sia la velocità in km/h che l'intensità di frenata in unità PWM (0-255)
//riporta l'intensità di frenata sul led BRAKING
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
int brake;

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
  Serial.println("The Gooble Bike 2.0: Sensor Monitor Freno EM");
}

void loop() {
  //verifica scadenza base dei tempi
  if(millis()-startBase>=TBASE) { //base dei tempi scaduta: invia dato ed attua freno em
    countPulses=constrain(countPulses,0,86);  //limita la frequenza degli impulsi tra 0 ed 86 imp/sec
    vel=map(countPulses,0,86,0,39);           //mappa la frequenza degli impulsi in velocità (vedi calcoli)
    brake=map(vel,0,39,0,255);                //mappa la velocità in frenata 
    if (vel<3) {                              //sicurezza: se < 3 km/h spegne il freno
      brake=0;
    }
    analogWrite(BRAKE,brake);                 //attua il freno
    analogWrite(BRAKING,brake);               //segnala la frenata sul led
    Serial.print(vel);                        //stampa vel
    Serial.print('\t');                       //tab per plot
    Serial.println(brake);                    //stampa frenata
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

