
#ifndef MyHttpClient_h
#define MyHttpClient_h


#include <Arduino.h>
#include <HttpClient.h>

class MyHttpClient{
  private:
    String url;
    HttpClient client;
    int led;
  public:
  MyHttpClient(String url);
  unsigned int sendRequest(unsigned int speed);
};




#endif
