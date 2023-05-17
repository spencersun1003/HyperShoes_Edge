#include <Adafruit_ADS1X15.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include "Utilities.h"
#include "mySensor.h"
#include "Communication.h"

#define I2C_SDA 17
#define I2C_SCL 21
#define IMU_SDA 26
#define IMU_SCL 18

#define SAMPLE_RATE 20
#define UPLOAD_FREQUENCY 2//Hz

// const char* fmtMemCk = "Free: %d\tMaxAlloc: %d\t PSFree: %d\n";
// #define MEMCK Serial.printf(fmtMemCk,ESP.getFreeHeap(),ESP.getMaxAllocHeap(),ESP.getFreePsram())

// manual setting
char *ssid = "Robotics2.4G";//"UW MPSK";
char *password = "quickboat917";//"d76!e6H$^4";//"F4iYxp]/a>"; // 4DMNt=y4~r or d76!e6H$^4 is left or F4iYxp]/a> is right
unsigned long deviceID = 10000000; // 10000000 is left, 10000001 is right
//String server_url = "http://10.19.240.96:5001/esp32"; // Location to send POSTed data
String server_url = "http://10.155.234.127:5001/esp32";//"http://10.19.82.215:5001/esp32"; // Location to send POSTed data
// Set web server port number to 80
Shoesserver *server;

Sensor_Pressure *pressure_sensor;


int default_frequency=100;
int pending_frequency=1000;
int sampleRate = 10;//Hz
int uploadFrequency=UPLOAD_FREQUENCY;
int sampleTimes=sampleRate/uploadFrequency;//sample how many times before sending the data
const int fsr4Pin = 34;


void setup(void) {
  Serial.begin(115200);
  Serial.println("Serial initialized...");
  Serial.println(WiFi.macAddress());
  pinMode(fsr4Pin, INPUT);
  Serial.print("Trying to begin the wire...");
  Wire.begin(I2C_SDA, I2C_SCL, 1000000);
  server=new Shoesserver(deviceID,server_url,ssid,password,80);
  pressure_sensor=new Sensor_Pressure(SAMPLE_RATE/UPLOAD_FREQUENCY);
  // setupADS();
  // Serial.println("ADS Found!");
  // setupMPU();
  // Serial.println("MPU6050 Found!");
  Serial.print("Trying to connect wifi...");
  server->setupWifiConnection();
  Serial.print("Connected to WiFi network with IP Address: ");Serial.println(WiFi.localIP());
  server->registerDevice();
  Serial.println("Device Registered");
  server->setupServer();
  Serial.println("Server Setup Complete");

  // setupTimer();
  // Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

float calculate(float p1,float p2){
  float h=0.5;
  float d=0.4;
  float tana,tanb;
  float xa,xb,v;
  tanb=(p1+p2)*h/(d*p1);
  xb=h/tanb;

  return xb;
}

int this_sampleTimes=0;
void loop(void) {
  // Serial.print("Frequency: "); Serial.p rintln(frequency);
  delay(1000/sampleRate);
  pressure_sensor->sample();
  this_sampleTimes++;

  if (this_sampleTimes>=sampleTimes){
    Serial.print("check wifi connection...");
    if(server->WifiConnected()){
        Serial.println("Wifi connected...");
        server->upload(pressure_sensor->fetch_packedData());
        this_sampleTimes=0;
    }
    else {
      Serial.println("Wifi Not Connected");
    }
  }
  
}