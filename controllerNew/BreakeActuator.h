#ifndef BreakeActuator_h
#define BreakeActuator_h


#include <Arduino.h>


class BreakeActuator{
  private:
  int port;
  int led;
  public:
  BreakeActuator();
  void setBreak(unsigned int value);

};




#endif


