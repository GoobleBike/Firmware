#include "BreakeActuator.h"
#include "Ports.h"

BreakeActuator::BreakeActuator(){
    port = PORT_OUTPUT_BREAK;
    led = PORT_OUTPUT_BREAK_LED;
    pinMode(port, OUTPUT);
    pinMode(led, OUTPUT);
}


  
void BreakeActuator::setBreak(unsigned int value){
    unsigned int brake;
      if(DEBUG_SERIAL){
      Serial.println("BRAKE VALUE:" + value);
     }
     value = constrain(value,0,20);
     brake = map(value,0,20,0,255);
     analogWrite(port, brake);
     analogWrite(led, brake);
}



