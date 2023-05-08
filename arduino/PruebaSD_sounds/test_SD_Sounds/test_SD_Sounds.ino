/*
  SD card basic file example

  This example shows how to create and destroy an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/
#include <SPI.h>
#include <SD.h>
#include <TMRpcm.h>

#define SD_ChipSelectPin 4

TMRpcm tmrpcm;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }

  //if (SD.exists("example.txt")) {
  //  Serial.println("example.txt exists.");
  //} else {
  //  Serial.println("example.txt doesn't exist.");
  //}

  // open a new file and immediately close it:
  //myFile = SD.open("ON.wav", FILE_READ);
  //myFile.close();

  // Check to see if the file exists:
  if (SD.exists("ON.wav")) {
    Serial.println("The file exists!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
  }

  tmrpcm.speakerPin = 10;
  tmrpcm.setVolume(5);
  tmrpcm.quality(1);

  tmrpcm.play("ON.wav");

  delay(5000);

  tmrpcm.play("OFF.wav");

  delay(1000);
  tmrpcm.play("HUM.wav");
  tmrpcm.loop(true);
  // delete the file:
  //Serial.println("Removing example.txt...");
  //SD.remove("example.txt");

  //if (SD.exists("example.txt")) {
    //Serial.println("example.txt exists.");
  //} else {
    //Serial.println("example.txt doesn't exist.");
  //}
}

void loop() {
  // nothing happens after setup finishes.
}



