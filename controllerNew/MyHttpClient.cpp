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
    char c;
    int a = 0;
    String newUrl;
    
     newUrl = this->url + speed;
     if(DEBUG_SERIAL){ 
         Serial.print("Send Request URL: "+ newUrl + "\n");
      } 
      client.get(newUrl);
      while (client.available()){
      //  digitalWrite(this->led, HIGH);
        c = client.read();
        if(DEBUG_SERIAL){ 
         Serial.print("Get Request VAL: ");
         Serial.print(c);
         Serial.print("\n");
        }
        buf[a] = c; 
        a++;
      }
        if(DEBUG_SERIAL){ 
         Serial.print("Get Request BUF: ");
         Serial.print(buf);
         Serial.print("\n");
        } 
      digitalWrite(this->led, LOW);
    
   return int(buf);
}

