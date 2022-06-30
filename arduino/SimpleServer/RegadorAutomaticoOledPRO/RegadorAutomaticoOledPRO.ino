#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "AHT10.h"
#include "Server.hpp"
#define TIMER_BOMBA 10000
#define UMBRAL_HUM 20
#define TIMER_LCD 10000
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

AHT10 sensorTemp(AHT10_ADDRESS_0X38);

int readStatus = 0;
int sensorValue = 0;
int timer = 0;
int humidity = 0;
long timeIni = 0;
long timeFin = 0;
long timeButton = 0;
int pinState = HIGH;

void setup(void) 
{
  Serial.begin(115200);
  
  pinMode(D8,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(D0,INPUT);

  digitalWrite(D7,pinState);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
   // Inicializar el LCD
  activateDisplay();
  
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  while (sensorTemp.begin() != true)
  {
    showText("Inicializando",0,1);
  }

  showText("Conectando...",0,1);
  
  connectWiFi_STA();
  initServer();
  showText("Conectado !!",0,1);

  delay(1000);
  
  desactivateDisplay();
  
  delay(1000);
  
  readStatus = sensorTemp.readRawData();

  delay(100);
  
  activateDisplay();
  
  if(readStatus != AHT10_ERROR){
    showSensor(sensorTemp.readTemperature(AHT10_USE_READ_DATA),sensorTemp.readHumidity(AHT10_USE_READ_DATA));
  }else{
    showText("Se ha perdido la conexión con los sensores",0,1);
  }
  
  delay(5000);
  
  desactivateDisplay();
}
 
void loop()
{ 
   int pressButton = 0;
   String textoBomba = "";
   
   humidity = 0; 
   readStatus = sensorTemp.readRawData();
   sensorValue = analogRead(A0);   
   humidity=map(sensorValue,0,1024,100, 0);

   pressButton = digitalRead(D0);
    
   Serial.print("button mode: ");
   Serial.println(pressButton);
   
    if(pressButton == pinState && 
      timeButton == 0 
      ){
        if(pinState == HIGH){
          pinState = LOW;
        }else{
          pinState = HIGH;
        }
          
      digitalWrite(D7,pinState);
      timeButton = millis();
      activateDisplay(); 
      Serial.println("Boton pulsado");
      
   }
   
   Serial.print("Humedad suelo: ");
   Serial.print(humidity);
   Serial.print(" % ");
   Serial.println(sensorValue);
  
  if(humidity < UMBRAL_HUM && timer == 0){
    
    activateDisplay();
    
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
    if(timeButton + TIMER_LCD > millis()){
      if (readStatus != AHT10_ERROR){
        showSensor(sensorTemp.readTemperature(AHT10_USE_READ_DATA),sensorTemp.readHumidity(AHT10_USE_READ_DATA));
      }else{
        showText("Se ha perdido la conexión con los sensores",0,1);
      }
    }else{
      timeButton=0;
      desactivateDisplay();
    }
  }
  
  sensorValues(sensorTemp.readTemperature(AHT10_USE_READ_DATA),sensorTemp.readHumidity(AHT10_USE_READ_DATA),humidity,timer);
  server.handleClient();
}

void showText(String text,int linea,int clear){
  
  if(clear == 1){
    display.clearDisplay();
  }
  
  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  display.setCursor(0, linea);
   // Escribimos el número de segundos trascurridos
  display.print(text);
}

void showSensor(float temp,float hum){
  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
  display.print("T: ");
  display.print(temp);
  display.print(" +-0.3 C ");
  
  display.setCursor(0,1);
  display.print("H: ");
  display.print(hum);
  display.print(" +-2 %");

  Serial.print("T: ");
  Serial.print(temp);
  Serial.println(" +-0.3 C ");
  
  Serial.print("H: ");
  Serial.print(hum);
  Serial.println(" +-2 %");
}

void activateBackLight(){
}

void desactivateBackLight(){  
}

void activateDisplay(){
  display.display(); 
}

void desactivateDisplay(){

  //display.noDisplay();  
}
