//The Gooble Bike 2.0!
//Versione per Bologna Experience
//Classe HTTP client per la comunicazione della velocità e la ricezione della pendenza
//Invia una richiesta GET al server contenente l'ID del controller ed il valore della velocità in Km/h
//riceve la pendenza in %
//22/5/2017

#include "MyHttpClient.h"
#include "Ports.h"
#include <Arduino.h>
#include <stdio.h>

//costruttore parametrico con URL
MyHttpClient::MyHttpClient(String url){
  this->url = url;
  this->led = SEND_LED_PIN;
  pinMode(this->led, OUTPUT);
}

//costruttore di default senza URL
MyHttpClient::MyHttpClient(){
  this->led = SEND_LED_PIN;
  pinMode(this->led, OUTPUT);
}

//inizializzatore dell'URL
void MyHttpClient::begin(String url){
  this->url = url;
}

//Invio richiesta ed estrazione della risposta
//riceve la velocità in km/h
//restituisce la pendenza in %
unsigned int MyHttpClient::sendRequest(unsigned int speed){
  char c;   //carattere ricevuto
  int i=0;  //indice di ricezione
  char buf[8]={0,0,0,0,0,0,0,0};  //buffer di ricezione: ci si aspetta ###\n\r\n\r\null
  int ivalue=0;   //valore convertito
  String newUrl;  //composizione dell'url con parametri di get
  newUrl = this->url + speed;
  if(DEBUG_SERIAL){ 
    Serial.println("SEND REQ URL: "+ newUrl);
  } 
  digitalWrite(this->led, HIGH);    //led comm ON
  client.get(newUrl);               //invio richiesta
  //polling di ricezione
  while ((client.available())){    //acquisisce solo i primi due caratteri
    c = client.read();
    buf[i] = c; 
    i++;
  }
  //svuota il canale e led comm OFF
  client.flush();
  digitalWrite(this->led, LOW);
  if(DEBUG_SERIAL){ 
    Serial.print("RESP: ");
    Serial.print(buf);
  } 
  //converte ad intero positivo o nullo
  ivalue = atoi(buf);
  if(ivalue < 0){
    ivalue = 0;
  }
  if(DEBUG_SERIAL){ 
    Serial.print(" CONV: ");
    Serial.println(ivalue);
  } 
  return ivalue;
}



