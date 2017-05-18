#include "MyHttpClient.h"
#include "Ports.h"
#include <Arduino.h>
#include <stdio.h>

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
    int a;
    String newUrl;
    
    
     buf[2] = 0x00;
     newUrl = this->url + speed;
     if(DEBUG_SERIAL){ 
         Serial.print("Send Request URL: "+ newUrl + "\n");
      } 
      client.get(newUrl);
      while (client.available()){
      //  digitalWrite(this->led, HIGH);
        c = client.read();
        if(DEBUG_SERIAL){ 
         //Serial.print("Get Request VAL: ");
         //Serial.print(c);
         //Serial.print("\n");
        }
       buf[a] = c; 
       a++;
       if(a == 2){
          break;
       }
      }
        client.flush();
       /* if(DEBUG_SERIAL){ 
         Serial.print("Get Request BUF: ");
         Serial.print(buf);
         Serial.print("\n");
        } */
      digitalWrite(this->led, LOW);
      a = atoi(buf);
      if(a < 0){
        a = 0;
      }
    /* if(DEBUG_SERIAL){ 
       Serial.print("Get Request BUF INT: ");
       Serial.print(a);
       Serial.print("\n");
    } */
   return a;
}



