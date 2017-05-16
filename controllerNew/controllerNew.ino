#include "BreakeActuator.h"
#include "MyHttpClient.h"
#include "ReadSpeed.h"
#include "Delay.h"
#include "Ports.h"

#define DELAY_TIME_SEND_REQUEST 2000




BreakeActuator breake;
MyHttpClient  clientHttp("http:\\\\192.168.1.117\\gooble.php?bubu=");
ReadSpeed reader;
Delay delaySend(DELAY_TIME_SEND_REQUEST);
int valBreake;

void setup() {
  
if(DEBUG_SERIAL){
Serial.begin(9600);
Serial.print("GOOBLE DEBUG ......\n");
delay(4000);
}

}

void loop() {
  reader.execute();
  delaySend.execute();
  if(delaySend.isReached() == true && reader.IsMoving() == true){
    if(DEBUG_SERIAL){
      Serial.print("IF\n");
    }
     valBreake = clientHttp.sendRequest(reader.getSpeed());
     breake.setBreak(valBreake);
  }
  else{
     breake.setBreak(0);
  }
}
