//The Gooble Bike 2.0!
//Versione per Bologna Experience
//con interrupt
//Basata su hw Arduino Yun e sul trainer Elite RealAxiom vers. 6 (wired)
//Legge gli impulsi del sensore di velocità e li converte in velocità [km/h] con una base dei tempi di 1s nel range 0-39 km/h.
//Invia ogni secondo la velocità al server attraverso una connessione HTTP client e riceve il valore della pendenza [%]
//Attua il valore di pendenza nel range 0-20% se il rullo è in movimento.
//23/5/2017

#include "Ports.h"
#include <Bridge.h>
#include <HttpClient.h>
#include <stdio.h>

void brakeAction(int theSlope,int theSpeed);
int sendSpeedGetSlope(int theSpeed, String theUrl, HttpClient theClient);
int computeSpeed(int theCount);

//costanti di sistema
#define SAMPLE_PERIOD 1000    //intervallo di campionamento del sensore di velocità
#define MAXPULSES 86          //numero massimo di impulsi al secondo
#define MAXSPEED 39           //velocità massima corrispondente al massimo di impulsi al secondo
#define MINSPEED  3           //minima velocità di attuazione del freno
#define MAXSLOPE 20           //massima pendenza gestibile


//variabili globali       
volatile int countPulses=0;   //conteggio impulsi/s: incrementato da ISR, resettato dalla scadenza dell'intervallo di campionamento
long startSampling;           //istante di inizio di campionamento: iniz. a millis ad ogni nuovo campionamento
int speed=0;                  //velocità in Km/hn nel range 0 MAXSPEED
int slope=0;

//gestione server
String urlProd="http://192.168.1.";                     //rete locale degli host
String urlDev="http://itis0001.belluzzifioravanti.it";  //host di sviluppo
String url;                                             //url con id
int id;                                                 //id del nodo letto dagli switch
HttpClient client;                                      //connessione client

//ISR: ad ogni fronte di salita di SENSOR_PIN incrementa il contatore di impulsi
void count(){
  countPulses++;
}

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
  if(id==0){
    url=urlDev+"/gooble/api/setv_getp?id="+id+"&v=";
  }
  else {
    url=urlProd+id+"/gooble/api/setv_getp?id="+id+"&v=";
  }
  //starting connection to Linino
  Bridge.begin();
  //local monitor
  if(DEBUG_SERIAL){
    Serial.begin(250000);
    Serial.println("GOOBLE DEBUG ......");
    Serial.print("ID ");
    Serial.println(id);
    Serial.print("URL: ");
    Serial.println(url);
    delay(4000);
  }
  //gestione interrupt
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), count, RISING);
  startSampling=millis();
}

void loop() {
  if(millis()-startSampling>=SAMPLE_PERIOD) {
    speed=computeSpeed(countPulses);
    //riarma campionamento
    countPulses=0;
    startSampling=millis();
    //comunicazione
    slope=sendSpeedGetSlope(speed,url,client);
    //attuazione freno
    brakeAction(slope,speed);
  }
}

//Azione di campionamento
//viene chiamata al termine di un periodo di campinamento di 1s
//riceve il contatore gestito dall'interrupt
//limita il valore nel range, lo converte in una velocità e restituisce la velocità
int computeSpeed(int theCount){
    int limitedCount;
    int theSpeed;
    int ledSpeed;
    if(DEBUG_SERIAL){ 
      Serial.print("SAMPLE: ");
      Serial.print("IMP/S: ");
      Serial.print(theCount);  
    }
    limitedCount=constrain(theCount,0,MAXPULSES);
    if(DEBUG_SERIAL){ 
      Serial.print(" CONSTR: ");
      Serial.print(limitedCount);  
    }
    theSpeed=map(limitedCount,0,MAXPULSES,0,MAXSPEED);
    if(DEBUG_SERIAL){ 
      Serial.print(" SPEED KM/H: ");
      Serial.println(theSpeed);      
    }  
    ledSpeed=map(theSpeed,0,MAXSPEED,0,255);
    analogWrite(MOVING_LED_PIN, theSpeed);
    return theSpeed;
}

//Azione di comunicazione
//riceve la velocità e l'url del server e l'oggetto client
//compone il comando di get con la velocità, lo attua, attende la risposta di pendenza, la converte in intero e la restituisce 
int sendSpeedGetSlope(int theSpeed, String theUrl, HttpClient theClient){
    String fullUrl;
    char buf[8]={0,0,0,0,0,0,0,0};       //buffer di ricezione: ci si aspetta ###\n\r\n\r\null
    int i=0;
    int ivalue;
    long start;
    digitalWrite(SEND_LED_PIN, HIGH);    //led comm ON
    fullUrl=theUrl+theSpeed;
    if(DEBUG_SERIAL){ 
        start=millis();
        Serial.println("SEND: REQ URL: "+ fullUrl);
    } 
    theClient.get(fullUrl);               //invio richiesta
    //polling di ricezione
    while (theClient.available()){    
      int c = theClient.read();
      buf[i] = c; 
      i++;
      if(i>7) break;
    }
    //svuota il canale e led comm OFF
    theClient.flush();
    if(DEBUG_SERIAL){ 
      Serial.print("RECV: RESP: ");
      Serial.print(buf);
    } 
    //converte ad intero positivo o nullo
    ivalue = atoi(buf);
    if(ivalue < 0){
      ivalue = 0;
    }
    if(DEBUG_SERIAL){ 
      Serial.print(" CONV: ");
      Serial.print(ivalue);
      Serial.print(" TIME mS: ");
      Serial.println(millis()-start);
    } 
    digitalWrite(SEND_LED_PIN, LOW);
    return ivalue;
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
    if(DEBUG_SERIAL){
        Serial.print("BRAKE: RCV SLOPE: ");
        Serial.print(theSlope);
    }
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
        Serial.print(" CONSRT SLOPE: ");
        Serial.print(value);
        Serial.print(" ACT PWM: ");
        Serial.println(brake);
        Serial.println();
    }    
}

