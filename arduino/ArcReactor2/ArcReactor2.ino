#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include "Server.hpp"
#include <FS.h>

#define MAX 1023

short bt;
boolean active = false;

void setup(void) {
  Serial.begin(115200);
  pinMode(OUTPUT,D0);
  bootType = 0;
  connectWiFiSTA;
  initServer();
}

int getLevel(unsigned int level){
  return map(level,0,MAX,0,255);
}

void rise(){
  for(int index=0;index < MAX ;index++){
      analogWrite(D0,getLevel(index));
      delay(50);
    }  
}

void blink(){
  for(int index=0;index < 5 ;index++){
      analogWrite(D0,0);
      delay(100 * (5 - index));

      analogWrite(D0,MAX);
      delay(100 * (5 - index));
      
    }
}

void loop(){ 
   //server.handleClient();
   bt=getBootType();
   if(bt == 0){
    //Desactivamos la lampara
    analogWrite(D0,0);
    active = false;
   }else if(bt == 1 && !active){
    //Activamos la lampara
    analogWrite(D0,MAX);
    active = true;    
   }else if(bt == 2 && !active){
    //Activamos la lampara con fundido
    rise();
    active = true;
   }else if(bt == 3 && !active){
    //Activamos la lampara con parpadeo
    blink();
    active = true;
   }else if(bt == 4 && !active){
    //Activamos la lampara con fundido y parpadeo
    blink();
    analogWrite(D0,0);
    rise();
    active = true;
   }
}
