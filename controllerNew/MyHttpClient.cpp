#include "MyHttpClient.h"
#include "Ports.h"
#include <Arduino.h>

MyHttpClient::MyHttpClient(String url){
  this->url = url;
   if(DEBUG_SERIAL){ 
    Serial.print("URL: "+ this->url + "\n");
   }
  this->led = PORT_LED_SEND_REQUEST;
  pinMode(this->led, OUTPUT);
}


unsigned int MyHttpClient::sendRequest(unsigned int speed){
    char buf[3];
    int a = 0;
    
     this->url = this->url + speed;
     if(DEBUG_SERIAL){ 
         Serial.print("Send Request URL: "+ this->url + "\n");
      } 
      client.get(this->url);
      while (client.available()) {
        digitalWrite(this->led, HIGH);
        buf[a] = client.read();
        a++;
        if(DEBUG_SERIAL){ 
         Serial.print("Get Request VAL: ");
         Serial.print(buf);
         Serial.print("\n");
        } 
      }
      digitalWrite(this->led, LOW);
    
   return int(buf);
}

