#include "ReadSpeed.h"
#include "Ports.h"

#define MINPULSES 8   //8 imp/s = circa 3,6 km/h
#define MAXPULSES 86  //86 imp/s = circa 36 km/h
#define TBASE   1000  //1s timebase sampling
#define MINSPEED  3   //3 km/h
#define MAXSPEED  39  //39 km/h

//int sensor;
//int previous;
//int countPulses=0;
//long startBase;
//int vel;

ReadSpeed::  ReadSpeed(){
  this->port = SENSOR_PIN;
  this->portLed = MOVING_LED_PIN;
  pinMode(this->port, INPUT);
  pinMode(this->portLed, OUTPUT);
}

void ReadSpeed::begin(){
  this->startBase=millis();  
  this->countPulses=0;
  this->speed=0;
  digitalWrite(this->portLed, LOW);
}

void ReadSpeed:: execute(){
  int vel;
  int velV;
  //verifica scadenza base dei tempi
  if(millis()-startBase>=TBASE) { //base dei tempi scaduta: invia dato
    this->countPulses=constrain(this->countPulses,0,MAXPULSES);
    vel=map(this->countPulses,0,MAXPULSES,0,MAXSPEED);
    this->speed = vel; 
    velV=map(vel,0,MAXSPEED,0,255);
    analogWrite(this->portLed, velV);
    if(DEBUG_SERIAL){ 
       Serial.print("COMPUTED SPEED: ");
       Serial.println(this->speed);
     }  
    this->startBase=millis();
    this->countPulses=0;
  }
  //campionamento
  this->sensor=digitalRead(this->port);
  //rilevamento fronte di salita
  if ((this->sensor!=this->previous)&&(this->sensor==HIGH)) { //fronte di salita: conta
    this->countPulses++;    
  }
  //aggiorna stato precedente
  this->previous=this->sensor;
}


int ReadSpeed::getSpeed(){
  return this->speed;
}
  
int ReadSpeed::IsMoving(){
  boolean ris=false;
  //verifica se la velocità  supera la soglia minima
  if(this->speed > MINSPEED) { //velocità  sopra la soglia minima: in movimento
    ris=true;
  }
  return ris;
}






