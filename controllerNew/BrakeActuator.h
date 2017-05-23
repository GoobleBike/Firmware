#ifndef BrakeActuator_h
#define BrakeActuator_h


#include <Arduino.h>


class BrakeActuator{
  private:
  int port;
  int led;
  public:
  BrakeActuator();
  void setBrake(unsigned int value);
};
#endif


