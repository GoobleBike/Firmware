#include "BrakeActuator.h"
#include "Ports.h"

BrakeActuator::BrakeActuator(){
    port =  BRAKE_PIN;
    led =   BRAKING_LED_PIN;
    pinMode(port, OUTPUT);
    pinMode(led, OUTPUT);
}


  
void BrakeActuator::setBrake(unsigned int value){
    unsigned int brake;
    if(DEBUG_SERIAL){
      Serial.print("RCV PEND: ");
      Serial.print(value);
     }
     value = constrain(value,0,20);
     brake = map(value,0,20,0,255);
     analogWrite(port, brake);
     analogWrite(led, brake);
    if(DEBUG_SERIAL){
      Serial.print(" CONSRT PEND: ");
      Serial.print(value);
      Serial.print(" ACT PWM: ");
      Serial.println(brake);
     }
}



