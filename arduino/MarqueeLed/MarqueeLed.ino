#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Server.hpp"

void setup(void) {
  Serial.begin(115200);
  connectWiFi_STA;
  initServer();
}
 
void loop(){ 
   server.handleClient();
}
