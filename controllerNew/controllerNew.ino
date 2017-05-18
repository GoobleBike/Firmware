#include "BreakeActuator.h"
#include "MyHttpClient.h"
#include "ReadSpeed.h"
#include "Delay.h"
#include "Ports.h"
#include <Bridge.h>

#define DELAY_TIME_SEND_REQUEST 1000




BreakeActuator breake;
MyHttpClient  clientHttp("http://192.168.188.130/gooble/api/setv_getp?id=0&v=");
ReadSpeed reader;
Delay delaySend(DELAY_TIME_SEND_REQUEST);
unsigned int valBreake;
unsigned int brake;
  
void setup() {
digitalWrite(PORT_INPUT_READ_LED,HIGH);
digitalWrite(PORT_OUTPUT_BREAK_LED,HIGH);
Bridge.begin();
if(DEBUG_SERIAL){
Serial.begin(9600);
Serial.print("GOOBLE DEBUG ......\n");
delay(4000);
}
digitalWrite(PORT_INPUT_READ_LED,LOW);
digitalWrite(PORT_OUTPUT_BREAK_LED,LOW);
}

void loop() {

  reader.execute();
  delaySend.execute();
  if(delaySend.isReached() == true /*&& reader.IsMoving() == true*/){
     valBreake = clientHttp.sendRequest(reader.getSpeed());
     if(reader.IsMoving() == true){
       breake.setBreak(valBreake);
     }
     else{
      breake.setBreak(0);
     }
  }
  else{
    // breake.setBreak(0);
  }
}


