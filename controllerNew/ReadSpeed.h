#ifndef ReadSpeed_h
#define ReadSpeed_h


#include <Arduino.h>


class ReadSpeed{
  private:
  int port;
  int portLed;
  int speed;
  long startBase;
  int countPulses;
  int sensor;
  int previous;
  public:
  volatile int countPulse;
  ReadSpeed();
  void begin();
  void execute();
  int getSpeed();
  int IsMoving();
};




#endif


