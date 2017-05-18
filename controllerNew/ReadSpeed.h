#ifndef ReadSpeed_h
#define ReadSpeed_h


#include <Arduino.h>


class ReadSpeed{
  private:
  int port;
  int portLed;
  int speed;
  public:
  ReadSpeed();
  void execute();
  int getSpeed();
  int IsMoving();
};




#endif


