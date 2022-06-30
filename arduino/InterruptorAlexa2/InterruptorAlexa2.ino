#include <ESP8266WiFi.h>
#include "credentials.h"
#include <Espalexa.h>
Espalexa espAlexa;

// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE     115200

#define ID_INTERRUPTOR           "flexo"

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup(bool useStaticIP = false) {

    Serial.println("Connecting...");
     WiFi.mode(WIFI_STA);
     WiFi.begin(ssid, password);
     if(useStaticIP) WiFi.config(ip, gateway, subnet);
     while (WiFi.status() != WL_CONNECTED) 
     { 
       delay(100);  
       Serial.print('.'); 
     }
   
     Serial.println("");
     Serial.print("Iniciado STA:\t");
     Serial.println(ssid);
     Serial.print("IP address:\t");
     Serial.println(WiFi.localIP());
     

}

void setup() {

    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println("Iniciando dispositivo");

    // LEDs
    pinMode(D1, OUTPUT);
    digitalWrite(D1, LOW);
    
    // Wifi
    wifiSetup();

    espAlexa.addDevice(ID_INTERRUPTOR,foco);
    espAlexa.begin();

}

void loop() {
  espAlexa.loop();
  delay(1);
}

void foco(uint8_t brillo){
     if(brillo){
      digitalWrite(D1, LOW);}
     else{
      digitalWrite(D1, HIGH);}
} 
