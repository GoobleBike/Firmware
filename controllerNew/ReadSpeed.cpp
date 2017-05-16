#include "ReadSpeed.h"
#include "Ports.h"

#define MINSPEED 8 //8 imp/s = 3,6 km/h
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
  pinMode(this->port, INPUT);
}

void ReadSpeed:: execute(){
  //verifica scadenza base dei tempi
  if(millis()-startBase>=TBASE) { //base dei tempi scaduta: invia dato
    countPulses=constrain(countPulses,0,67);
    vel=map(countPulses,0,67,0,30);
    this->speed = vel; 
    if(DEBUG_SERIAL){ 
       Serial.print("Get Request Speed: ");
       Serial.print(this->speed);
       Serial.print("\n");
     }  
    startBase=millis();
    countPulses=0;
  }
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
  //verifica se la velocità supera la soglia minima
  if(speed > MINSPEED) { //velocità sopra la soglia minima: in movimento
    ris=true;
  }
  return ris;
}




