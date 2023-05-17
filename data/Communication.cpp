#include "Communication.h"

bool Shoesserver::WifiConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  else{
    return true;
  }
}

void Shoesserver::handleRecording(bool isRecording) {
  isUpload = isRecording;

  HTTPClient http;
  String serverPath = server_url + "/record";
  // Serial.print("serverPath: "); Serial.println(serverPath);
  http.begin(serverPath.c_str());
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "deviceID=" + String(deviceID) + "&record=" + String(isRecording);
  int httpResponseCode = http.POST(httpRequestData);
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void Shoesserver::setupWifiConnection(){
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }  
}

//register device to server
void Shoesserver::registerDevice() {
  HTTPClient http;
  String serverPath = server_url + "/register";
  Serial.print("serverPath: "); Serial.println(serverPath);
  http.begin(serverPath.c_str());
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "deviceID=" + String(deviceID);
  int httpResponseCode = http.POST(httpRequestData);
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

// setup server events, called when program starts
void Shoesserver::setupServer() {
  Serial.println("Setting up server");
  server->on("/start", HTTP_GET, [this](AsyncWebServerRequest *request){
    Serial.println("Start recording");
    int param = request->getParam("sampleRate")->value().toInt();
    this->sampleRate = (param == 0) ? (this->default_frequency) : (param);
    request->send(200, "text/plain", "RecordingStatus: " + String(true));
    Serial.print("Recording Status: "); Serial.println(true);
    this->handleRecording(true);
  });
  server->on("/stop", HTTP_GET, [this](AsyncWebServerRequest *request){
    Serial.println("Stop recording");
    this->sampleRate = pending_frequency;
    request->send(200, "text/plain", "RecordingStatus: " + String(false));
    Serial.print("Recording Status: "); Serial.println(false);
    this->handleRecording(false);
  });
  server->onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Page not found");
  });
  server->begin();
}

// upload data to server
void Shoesserver::upload(String queryString) {
  if (isUpload){
    Serial.print("Setup HTTP client");

    HTTPClient http;
    String serverPath = server_url + "/upload";
    // Serial.print("serverPath: "); Serial.println(serverPath);
    Serial.print("Starting HTTP session");
    http.begin(serverPath.c_str());
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = queryString + "&deviceID=" + String(deviceID);
    Serial.print("Start to HTTP post"); Serial.println(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
      if (payload.equals("Not authorized to upload")) {
        sampleRate = pending_frequency;
        Serial.print("Recording Status: "); Serial.println(false);
        this->handleRecording(false);
      }
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  else{
    Serial.println("isupload is False, Not uploading");
  }
}