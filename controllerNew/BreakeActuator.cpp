#include "BreakeActuator.h"
#include "Ports.h"

BreakeActuator::BreakeActuator(){
    port = PORT_OUTPUT_BREAK;
    led = PORT_OUTPUT_BREAK_LED;
    pinMode(port, OUTPUT);
    pinMode(led, OUTPUT);
}


  
void BreakeActuator::setBreak(unsigned int value){
     int brake;
     
     brake=map(value,0,30,0,255);
     analogWrite(port, brake);
     analogWrite(led, brake);
}

