#include "ReadSpeed.h"
#include "Ports.h"

#define MINSPEED 3 //8 imp/s = 3,6 km/h
#define MAXSPEED 86 //86 imp/s = 36 km/h
#define SPEEDTIMEBASE 1000 //timebase = 1 s

#define TCAMP   1
#define TBASE   1000

int sensor;
int previous;
int countPulses=0;
long startBase;
int vel;

ReadSpeed::  ReadSpeed(){
  this->port = PORT_INPUT_READ;
  this->portLed = PORT_INPUT_READ_LED;
  pinMode(this->port, INPUT);
  pinMode(this->portLed, OUTPUT);
}

void ReadSpeed:: execute(){
  int velV;
  //verifica scadenza base dei tempi
  if(millis()-startBase>=TBASE) { //base dei tempi scaduta: invia dato
    countPulses=constrain(countPulses,0,MAXSPEED);
    vel=map(countPulses,0,MAXSPEED,0,39);
    this->speed = vel; 
    velV=map(vel,0,39,0,255);
    analogWrite(this->portLed, velV);
    if(DEBUG_SERIAL){ 
       Serial.print("Get Request Speed: ");
       Serial.print(this->speed);
       Serial.print("\n");
     }  
    startBase=millis();
    countPulses=0;
  }
 // digitalWrite(this->portLed, LOW);
  //campionamento
  sensor=digitalRead(this->port);
  //rilevamento fronte di salita
  if ((sensor!=previous)&&(sensor==HIGH)) { //fronte di salita: conta
    countPulses++;    
  }
  //aggiorna stato precedente
  previous=sensor;
  //intervallo di campionamento  
  //delay(TCAMP); 
}


int ReadSpeed::getSpeed(){
  return this->speed;
}
  
int ReadSpeed::IsMoving(){
  boolean ris=false;
  //verifica se la velocitÃƒÂ  supera la soglia minima
  if(this->speed > MINSPEED) { //velocitÃƒÂ  sopra la soglia minima: in movimento
    ris=true;
  }
  return ris;
}






