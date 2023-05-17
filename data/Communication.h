#ifndef Communication_h
#define Communication_h
#include <HTTPClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include "Utilities.h"
#include <Arduino.h>

class Shoesserver {
    
    private:
        int default_frequency ;
        int pending_frequency ;
        int sampleRate ;
        bool isUpload ;
        unsigned long deviceID;
        String server_url;
        char *ssid;
        char *password;
        AsyncWebServer *server;
    
    public:
      Shoesserver(unsigned long deviceID,
          String server_url,
          char *ssid,
          char *password,
          int Port=80,int default_frequency=100, int pending_frequency=1000,int sampleRate=20) {
          this->deviceID = deviceID;
          this->server_url = server_url;
          this->ssid = ssid;
          this->password = password;
          this->server=new AsyncWebServer(Port);
          this->default_frequency=default_frequency;
          this->pending_frequency=pending_frequency;
          this->sampleRate=sampleRate;
          this->isUpload=true;
      }
        void setupWifiConnection();
        void registerDevice();
        void setupServer();
        void handleRecording(bool isRecording);
        void upload(String queryString);
        bool WifiConnected();
};


#endif
