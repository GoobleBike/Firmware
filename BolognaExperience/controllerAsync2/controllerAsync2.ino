//The Gooble Bike 2.0!
//Versione per Bologna Experience
//Async
//Basata su hw Arduino Yun e sul trainer Elite RealAxiom vers. 6 (wired)
//Legge gli impulsi del sensore di velocità e li converte in velocità [km/h] con una base dei tempi di 1s nel range 0-39 km/h.
//Invia ogni secondo la velocità al server attraverso una connessione HTTP client e riceve il valore della pendenza [%]
//Attua il valore di pendenza nel range 0-20% se il rullo è in movimento.
//29/5/2017

#include "Ports.h"
#include <Bridge.h>
#include <HttpClient.h>
#include <stdio.h>
#include "MemoryFree.h"

int computeSpeed(int theCount,long tBase);
void sendSpeed(int theSpeed, String theUrl, HttpClient* theClient);
void rcvSlope(HttpClient* theClient);
void brakeAction(int theSlope,int theSpeed);
void mngTmo();

//costanti di sistema
#define SAMPLE_PERIOD 1000    //intervallo di campionamento del sensore di velocità
#define MAXPULSES 67          //numero massimo di impulsi al secondo
#define MAXSPEED 30           //velocità massima corrispondente al massimo di impulsi al secondo
#define MINSPEED  3           //minima velocità di attuazione del freno
#define MAXSLOPE 20           //massima pendenza gestibile
#define MINSEMIP  7           //durata minima di un semiperiodo
#define TXTIMEOUT 5000        //durata timeout di comunicazione


//variabili globali

//flag di sincronizzazione
int txPending=0;                //misura completata in attesa di trasmissione
int txRunning=0;                //com in corso
int rxRunning=0;                //fase rx di com in corso
int tmoRunning=0;               //timeout di comunicazione

//variabili di campionamento
volatile int filtro=0;                 //var. stato filtro antidisturbo: 0=idle, 1=rilevato fronte di salita
volatile long startSemip;              //istante di inizio del fronte di salita
volatile int duration;                 //durata effettiva del semiperiodo
volatile int countPulses=0;            //numero di impulsi contati

//variabili di misura velocità
long startSampling;           //istante di inizio di un campionamento
long curSampleTime;           //tempo passato dall'inizio campionamento
int speed=0;                  //velocità in Km/h

//variabili di comunicazione
HttpClient* client;                                      //connessione client
String url;                                             //url con id
char buf[8]={0,0,0,0,0,0,0,0};                         //buffer di ricezione: ci si aspetta ###\n\r\n\r\null
int i;                                                //indice nel buffer
int c;                                                //carattere ricevuto
int rxValue;                                          //valore ricevuto
long startComTime;                                    //istante di inizio della comunicazione
long comDuration;                                      //durata della com
long maxComDuration=0;

//variabili di timeout
long startTimeout;        //istante di inzio del timeout di comunicazione
long curTmoTime;          //durata corrente del tmo
long tmoCount=0;


//gestione server
String urlProd="http://192.168.2.";                     //rete locale degli host
String urlDev="http://itis0001.belluzzifioravanti.it";  //host di sviluppo
int id;                                                 //id del nodo letto dagli switch
String curIp;
String ip[5]={"","101","102","103","104"};

void setup() {
  
  //setting pins
  pinMode(SW1_PIN,INPUT_PULLUP);
  pinMode(SW2_PIN,INPUT_PULLUP);
  pinMode(SW3_PIN,INPUT_PULLUP);
  pinMode(SENSOR_PIN,INPUT_PULLUP);
  pinMode(BRAKE_PIN,OUTPUT);
  digitalWrite(BRAKE_PIN,LOW);
  pinMode(MOVING_LED_PIN,OUTPUT);
  pinMode(BRAKING_LED_PIN,OUTPUT);
  pinMode(SEND_LED_PIN,OUTPUT);
  
  //lamp test
  digitalWrite(MOVING_LED_PIN,HIGH);
  digitalWrite(BRAKING_LED_PIN,HIGH);
  digitalWrite(SEND_LED_PIN,HIGH);
  delay(2000);
  //end lamp test
  digitalWrite(MOVING_LED_PIN,LOW);
  digitalWrite(BRAKING_LED_PIN,LOW);
  digitalWrite(SEND_LED_PIN,LOW);
  
  //detecting server
  int sw1=!digitalRead(SW1_PIN);
  int sw2=!digitalRead(SW2_PIN);
  int sw3=!digitalRead(SW3_PIN);
  id=sw1*4+sw2*2+sw3;
  //temp
  id=2;
  if(id==0){
    url=urlDev+"/gooble/api/setv_getp?id="+id+"&v=";
  }
  else {
    curIp=ip[id];    
    url=urlProd+curIp+"/gooble/api/setv_getp?id="+id+"&v=";
  }

  //setting status var
  txPending=0;
  txRunning=0;
  rxRunning=0;
  tmoRunning=0;
  countPulses=0;
  
  //starting connection to Linino
  Bridge.begin();
  //local monitor
  if(DEBUG_SERIAL){
    Serial.begin(9600);
    delay(2000);
    Serial.println("GOOBLE DEBUG ......");
    Serial.print("ID ");
    Serial.println(id);
    Serial.print("URL: ");
    Serial.println(url);
    Serial.print("FREE MEMORY ");
    Serial.println(freeMemory());
    delay(4000);
  }
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), rising, RISING);
  startSampling=millis();
}

//ISR sul fronte positivo del segnale SENSOR: avvia il filtro antidisturbo e commuta interrupt sul fronte negativo
void rising() {
  filtro=1;
  startSemip=millis();      
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), falling, FALLING);
}

//ISR sul fronte negativo del segnale SENSOR: controlla il filtro antidisturbo e commuta interrupt sul fronte positivo
void falling(){
  filtro=0;
  duration=millis()-startSemip;      
  if(duration>MINSEMIP) { //durata semiperiodo > del minimo: impulso valido
    countPulses++;
  }
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), rising, RISING);  
}

void loop() {
  
  //determinazione imp/s e calcolo velocità  
  curSampleTime=millis()-startSampling;
  if(curSampleTime>=SAMPLE_PERIOD) { //periodo di campionamento terminato: calcola velocità
      speed=computeSpeed(countPulses,curSampleTime);    //calcolo velocità
      countPulses=0;                               //riavvia timebase
      startSampling=millis();
      txPending=true;                             //richiede trasmissione
  }
  
  //gestione avvio comunicazione
  if(txPending==true) { //tx pendente verifica stato delle comunicazioni
    if(txRunning==false) {  //tx non in corso avvia nuova tx
      client=new HttpClient();
      sendSpeed(speed,url,client);       //get asincrona nessuna risposta immediata
      txRunning=true;                     //tx in corso
      startTimeout=millis();              //avvia tmo tx
      tmoRunning=true;        
    }
  }
  
  //attesa risposta comunicazione
  if(txRunning==true) {   //tx in corso: verifica se è terminata
    if(rxRunning==false) { //rx non ancora iniziata
      int resp=client->ready();
      if(resp==true) {  //risposta disponibile
        rxRunning=true;
        for(i=0;i<8;i++) buf[i]=0;    //azzera il buf di rx
        i=0;
      }
    }
  }  
  
  //lettura risposta ed attuazione freno
  if(txRunning==true) { //comunicazione in corso
    if(rxRunning==true) { //ricezione in corso
      if(client->available()) { //ancora dati nel buffer
        c=client->read();
        buf[i]=c;
        i++;
        if(i>7) { //limite buffer raggiunto: termina comunicazione,ferma il tmo
          rcvSlope(client);
          delete client;
        }
      }
      else { //buffer svuotato prima del limite
        rcvSlope(client);
        delete client;
      }
    }
  }

  //gestione timeout comunicazione
  if(tmoRunning==true) { //timeout di comunicazione in corso
    curTmoTime=millis()-startTimeout;    
    if(curTmoTime>=TXTIMEOUT) { //timeout scaduto, annulla comunicazione e si prepara a ripartire
      mngTmo();
      delete client;
    }
  }
}  

////Azione di campionamento
////viene chiamata al termine di un periodo di campinamento di 1s
////riceve il contatore gestito dall'interrupt
////limita il valore nel range, lo converte in una velocità e restituisce la velocità
int computeSpeed(int theCount,long tBase){
      int limitedCount;
      int theSpeed;
      int ledSpeed;
      if(DEBUG_SERIAL){ 
        Serial.print("FREE MEMORY ");
        Serial.println(freeMemory());
        Serial.print("SAMPLE: ");
        Serial.print("TBASE: ");
        Serial.print(curSampleTime);  
        Serial.print(" IMP/S: ");
        Serial.print(theCount);  
      }
      //float fCount = ((float)theCount * 1000.0f)/(float)tBase;
      //int rCount= (int)(fCount+0.5f);    
      limitedCount=constrain(theCount,0,MAXPULSES);
      if(DEBUG_SERIAL){ 
        Serial.print(" CONSTR: ");
        Serial.print(limitedCount);  
      }
      theSpeed=map(limitedCount,0,MAXPULSES,0,MAXSPEED);
      //temp
      //theSpeed=10;
      if(DEBUG_SERIAL){ 
        Serial.print(" SPEED KM/H: ");
        Serial.println(theSpeed);      
      }  
      ledSpeed=map(theSpeed,0,MAXSPEED,0,255);
      analogWrite(MOVING_LED_PIN, theSpeed);
      return theSpeed;
}

//Azione di avvio trasmissione
//compone il comando di get con la velocità, lo attua, tx asincrona, nessuna attesa di risposta
void sendSpeed(int theSpeed, String theUrl, HttpClient* theClient){
    String fullUrl;
    fullUrl=theUrl+theSpeed;
    startComTime=millis();              //inizia misuta tempo di com
    if(DEBUG_SERIAL){
            Serial.println("SEND: REQ URL: "+ fullUrl);
    }
    digitalWrite(SEND_LED_PIN, HIGH);    //led comm ON
    theClient->getAsynchronously(fullUrl);      //get asincrona nessua risposta immediata
}

//gestione dato ricevuto
void rcvSlope(HttpClient* theClient){ 
  digitalWrite(SEND_LED_PIN, LOW);    //led comm ON
  comDuration=millis()-startComTime;
  if(comDuration>maxComDuration){
    maxComDuration=comDuration;
  }
  txPending=false;
  txRunning=false;
  rxRunning=false;            
  tmoRunning=false;
  theClient->flush();
  rxValue = atoi(buf);  //calcola pendenza
  if(rxValue < 0){
    rxValue = 0;
  }
  if(DEBUG_SERIAL){
      Serial.print("RVC: RCV BUF: ");
      Serial.print(buf);
      Serial.print(" RCV SLOPE: ");
      Serial.print(rxValue);
      Serial.print(" COM TIME: ");
      Serial.print(comDuration);
      Serial.print(" MAX COM TIME: ");
      Serial.println(maxComDuration);
  }    
  brakeAction(rxValue,speed);     //attua pendenza                      
}

//Azione di frenata
//riceve la pendenza e la velocità
//se la velocità è >= MINSPEED
//limita la pendenza al 20%, la mette in scala tra 0-255
//altrimenti mette la pendenza a 0
//attua l'uscita al trainer ed il led BRAKING
void brakeAction(int theSlope,int theSpeed) {
    int value;
    int brake;
    //attua solo se velocità >=MINSPEED
    if (theSpeed>=MINSPEED) value=theSlope;
    else                    value=0;
    //limitazione del range a 20%
    value = constrain(value,0,MAXSLOPE);
    //messa in scala per PWM
    brake = map(value,0,MAXSLOPE,0,255);
    analogWrite(BRAKE_PIN, brake);
    analogWrite(BRAKING_LED_PIN, brake);
    if(DEBUG_SERIAL){
       Serial.print("ACT: CONSRT SLOPE: ");
       Serial.print(value);
       Serial.print(" PWM: ");
       Serial.println(brake);
       Serial.println();
    }    
}

//gestione timeout: ferma tutto e riavvia
void mngTmo(){
  txPending=false;
  txRunning=false;
  rxRunning=false;
  tmoRunning=false;
  tmoCount++;
  if(DEBUG_SERIAL){
      Serial.print("TMO: TMO TIME: ");
      Serial.print(curTmoTime);
      Serial.print(" TMO TIME: ");
      Serial.println(tmoCount);

  }    
}


