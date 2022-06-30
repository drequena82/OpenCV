// Include the correct display library

// For a connection via I2C using the Arduino Wire include:
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"
// Temperature sensor
#include "AHT10.h"

// Optionally include custom images
#include "images.h"


// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

// Initialize the OLED display using SPI:
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS

AHT10 sensorTemp(AHT10_ADDRESS_0X38);

#define DEMO_DURATION 3000
typedef void (*Demo)(void);

int demoMode = 0;
int counter = 1;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();

  display.resetDisplay();
  drawProgressBar();
  display.display();

  display.resetDisplay();
  drawIconComodore();
  display.display();
  
  delay(6000);

  
  //delay(1000);
  display.clear();
}

void drawTextComodore() {
  int readStatus = 0;
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 10, "*** COMMODORE BASIC ***");

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 20, "5119 BYTES FREE");

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 30, "READY.");

  readStatus = sensorTemp.readRawData();

  if (readStatus != AHT10_ERROR){
        display.drawString(0, 40,"> " + String(sensorTemp.readTemperature(AHT10_USE_READ_DATA)) + " ºC " + String(sensorTemp.readHumidity(AHT10_USE_READ_DATA)) + " %");
  }else{
        display.drawString(0,40, "No data!!");
  }
}

void drawProgressBar() {
    
    for(int i = 0; i <= 100; i = i + 5){
      delay(500);
      display.clear();
      
      
  
      // draw the percentage as String
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 15, String(i) + "%");

      // draw the progress bar  
      //display.drawProgressBar(0, 32, 120, 10, i);
      
      Serial.println("Progreso: " +  String(i));
      
      display.display();
      
    }

    display.clear();
    delay(1000);
    
}

void drawIconComodore() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(0, 0, 128, 64, commodore_logo_bits);
}

void loop() {
  display.clear();
  drawTextComodore();
  display.display();
}
