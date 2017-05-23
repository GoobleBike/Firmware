//The Gooble Bike 2.0!
//Versione per Bologna Experience
//Basata su hw Arduino Yun e sul trainer Elite RealAxiom vers. 6 (wired)
//Legge gli impulsi del sensore di velocità e li converte in velocità [km/h] con una base dei tempi di 1s nel range 0-39 km/h.
//Invia ogni secondo la velocità al server attraverso una connessione HTTP client e riceve il valore della pendenza [%]
//Attua il valore di pendenza nel range 0-20% se il rullo è in movimento.
//20/5/2017

#include "BrakeActuator.h"
#include "MyHttpClient.h"
#include "ReadSpeed.h"
#include "Delay.h"
#include "Ports.h"
#include <Bridge.h>

#define DELAY_TIME_SEND_REQUEST 1000

String urlProd="http://192.168.1.";
String urlDev="http://itis0001.belluzzifioravanti.it";
String url;


BrakeActuator brake;
MyHttpClient clientHttp;
ReadSpeed reader;
Delay delaySend(DELAY_TIME_SEND_REQUEST);
unsigned int brakeValue;
int id;

//MyHttpClient  clientHttp("http://192.168.188.130/gooble/api/setv_getp?id=0&v=");
//MyHttpClient  clientHttp("http://itis0001.belluzzifioravanti.it/gooble/api/setv_getp?id=0&v=");


void setup() {
  //setting pins
  pinMode(SW1_PIN,INPUT_PULLUP);
  pinMode(SW2_PIN,INPUT_PULLUP);
  pinMode(SW3_PIN,INPUT_PULLUP);
  pinMode(SENSOR_PIN,INPUT);
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
  clientHttp.begin(url);
  //starting connection to Linino
  Bridge.begin();
  if(DEBUG_SERIAL){
    Serial.begin(9600);
    Serial.println("GOOBLE DEBUG ......");
    Serial.print("ID ");
    Serial.println(id);
    Serial.print("URL: ");
    Serial.println(url);
    delay(4000);
  }
  reader.begin();
}

void loop() {
  //campionamento sensore
  reader.execute();
  //polling trasmissione 
  delaySend.execute();
  if(delaySend.isReached() == true){ //polling di tx scaduto
    //trasmette velocità e riceve pendenza
     brakeValue = clientHttp.sendRequest(reader.getSpeed());
     if(reader.IsMoving() == true){ //sistema in movimento
       //attua la pendenza ricevuta
       brake.setBrake(brakeValue);
     }
     else{ //sistema fermo
      //mette in sicurezza il freno frenata =0
      brake.setBrake(0);
     }
  }
}


