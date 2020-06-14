#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LiquidCrystal_I2C.h>

#include "AHT10.h"
#include "config.h"  // Sustituir con datos de vuestra red
#include "Server.hpp"
#include "ESP8266_Utils.hpp"

#define TIMER_BOMBA 10000
#define UMBRAL_HUM 20

LiquidCrystal_I2C lcd(0x3F,16,2);
AHT10 sensorTemp(AHT10_ADDRESS_0X38);

int readStatus = 0;
int sensorValue = 0;
int timer = 0;
int humidity = 0;

void setup(void) 
{
  Serial.begin(115200);
  
  pinMode(D8,OUTPUT);
    
   // Inicializar el LCD
  lcd.init();
  
  //Encender la luz de fondo.b
  lcd.backlight();

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  while (sensorTemp.begin() != true)
  {
    showText("Inicializando",0,1);
  }
  
  ConnectWiFi_STA();
 
  InitServer();
}
 
void loop()
{ 
   String textoBomba = "";
   humidity = 0; 
   readStatus = sensorTemp.readRawData();
   sensorValue = analogRead(A0);   
   humidity=map(sensorValue,0,1024,100, 0);

  
   Serial.print("Humedad suelo: ");
   Serial.print(humidity);
   Serial.print(" % ");
   Serial.println(sensorValue);

  if(humidity < UMBRAL_HUM && timer == 0){

    textoBomba = "Hum. baja " + String(humidity) + "%";
    //activamos el motor de la bomba 10 segundos
    showText(textoBomba,0,1);
    showText("Regando...",1,0);
    digitalWrite(D8, HIGH);
    timer = millis();
  }

  if(timer != 0){
    textoBomba = "Hum. baja " + String(humidity) + "%";
    //activamos el motor de la bomba 10 segundos
    showText(textoBomba,0,1);
    showText("Regando...",1,0);
    
    if((timer  + TIMER_BOMBA) < millis()){
      textoBomba = "Humedad " + String(humidity) + "%";
      //activamos el motor de la bomba 10 segundos
      showText(textoBomba,0,1);
      showText("Fin regado",1,0);
    
      digitalWrite(D8, LOW);
      timer = 0;

      delay(1000);
    }
  }else{   
   if (readStatus != AHT10_ERROR){
    showSensor(sensorTemp.readTemperature(AHT10_USE_READ_DATA),sensorTemp.readHumidity(AHT10_USE_READ_DATA));
   }else{
    showText("Se ha perdido la conexión con los sensores",0,1);
   }
  }
   sensorValues(sensorTemp.readTemperature(AHT10_USE_READ_DATA),sensorTemp.readHumidity(AHT10_USE_READ_DATA),humidity,timer);
   server.handleClient();
}

void showText(String text,int linea,int clear){
  
  if(clear == 1){
    lcd.clear();
  }
  
  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  lcd.setCursor(0, linea);
   // Escribimos el número de segundos trascurridos
  lcd.print(text);
}

void showSensor(float temp,float hum){
  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  lcd.clear();
  lcd.setCursor(0,0);
  
  lcd.print("T: ");
  lcd.print(temp);
  lcd.print(" +-0.3 C ");
  
  lcd.setCursor(0,1);
  lcd.print("H: ");
  lcd.print(hum);
  lcd.print(" +-2 %");
}
