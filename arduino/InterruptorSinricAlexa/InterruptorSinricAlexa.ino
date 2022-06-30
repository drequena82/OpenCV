/*
 * Example for how to use SinricPro Switch device:
 * - setup a switch device
 * - handle request using callback (turn on/off builtin led indicating device power state)
 * - send event to sinricPro server (flash button is used to turn on/off device manually)
 * 
 * If you encounter any issues:
 * - check the readme.md at https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md
 * - ensure all dependent libraries are installed
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#arduinoide
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#dependencies
 * - open serial monitor and check whats happening
 * - check full user documentation at https://sinricpro.github.io/esp8266-esp32-sdk
 * - visit https://github.com/sinricpro/esp8266-esp32-sdk/issues and check for existing issues or open a new one
 */

// Uncomment the following line to enable serial debug output
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 

#include <Arduino.h>
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include "SinricPro.h"
#include "SinricProSwitch.h"

#define WIFI_SSID         "HOMEFIBER"    
#define WIFI_PASS         "1A2B3C4D5E6F"
#define APP_KEY           "89a23f05-9a59-43ca-b889-950628ec3313"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "ba22c9d2-4225-4646-ac66-462f7f1a3e82-7f62173e-db80-4bc5-a9a1-b45e075b9098"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define SWITCH_LUZ_ID         "61b08c50695fce5c87487e3a"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define SWITCH_BOLA_ID         "61b08b30695fce5c87487d5c"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE         9600                // Change baudrate to your need

#define LUZ_PIN   D2   // GPIO for LED (inverted)
#define BOLA_PIN  D1   // GPIO for LED (inverted)

/* bool onPowerState(String deviceId, bool &state) 
 *
 * Callback for setPowerState request
 * parameters
 *  String deviceId (r)
 *    contains deviceId (useful if this callback used by multiple devices)
 *  bool &state (r/w)
 *    contains the requested state (true:on / false:off)
 *    must return the new state
 * 
 * return
 *  true if request should be marked as handled correctly / false if not
 */
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s turned %s (via SinricPro) \r\n", deviceId.c_str(), state?"on":"off");
  
  if(deviceId == "61b08c50695fce5c87487e3a"){
      digitalWrite(LUZ_PIN, state?HIGH:LOW);
  }else if(deviceId == "61b08b30695fce5c87487d5c"){
    digitalWrite(BOLA_PIN, state?HIGH:LOW);
  }
  return true; // request handled properly
}

// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

// setup function for SinricPro
void setupSinricPro() {
  // add device to SinricPro
  SinricProSwitch& mySwitch = SinricPro[SWITCH_LUZ_ID];

  // set callback function to device
  mySwitch.onPowerState(onPowerState);

  // add device to SinricPro
  SinricProSwitch& mySwitchBola = SinricPro[SWITCH_BOLA_ID];

  // set callback function to device
  mySwitchBola.onPowerState(onPowerState);
  
  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  //SinricPro.restoreDeviceStates(true); // Uncomment to restore the last known state from the server.
  SinricPro.begin(APP_KEY, APP_SECRET);
}

// main setup function
void setup() {
  pinMode(LUZ_PIN, OUTPUT); // define LED GPIO as output
  digitalWrite(LUZ_PIN, LOW); // turn off LED on bootup

  pinMode(BOLA_PIN, OUTPUT); // define LED GPIO as output
  digitalWrite(BOLA_PIN, LOW); // turn off LED on bootup
  
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
}
