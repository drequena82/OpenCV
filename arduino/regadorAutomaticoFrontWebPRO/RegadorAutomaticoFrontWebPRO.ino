#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LiquidCrystal_I2C.h>
#include <FS.h>

#include "AHT10.h"
#include "Server.hpp"

#define TIMER_LCD 5000
#define FAILED_TIMES 3

LiquidCrystal_I2C lcd(0x3F,16,2);
AHT10 sensorTemp(AHT10_ADDRESS_0X38);

int activeSwitch = HIGH;
int timerBomba = 3000;
int umbralHum = 20;
int readStatus = 0;
int sensorValue = 0;
int timer = 0;
int humidity = 0;
long timeIni = 0;
long timeFin = 0;
long timeButton = 0;
int pinState = HIGH;
int humidityOld = 0;
int failedTimes = 0;
int isRiegoManual = 0;

void setup(void) 
{
  Serial.begin(115200);
  
  pinMode(D8,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(D0,INPUT);

  digitalWrite(D7,pinState);
  
  lcd.init();
  
   // Inicializar el LCD
  activateDisplay();

  showText("Conectando...",0,1);
  
  connectWiFi_STA();
  initServer();
  
  showText("Conectado !!",0,1);

  delay(1000);
  
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  while (sensorTemp.begin() != true)
  {
    showText("Inicializando",0,1);
  }
  
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
   /* 
   Serial.print("button mode: ");
   Serial.println(pressButton);
   */
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
      //Serial.println("Boton pulsado");
      
   }
   /*
   Serial.print("Humedad suelo: ");
   Serial.print(humidity);
   Serial.print(" % ");
   Serial.println(sensorValue);
  */
  if((humidity < umbralHum || isRiegoManual) && timer == 0 && activeSwitch == HIGH){
    //Asignamos la humedad del suelo antes de regar.
    humidityOld = humidity;
    isRiegoManual = 0;
    //Si no ha subido la humedad deja de regar
    if(failedTimes < FAILED_TIMES){
      activateDisplay();
        
      textoBomba = "Hum. baja " + String(humidity) + "%";
      //activamos el motor de la bomba 10 segundos
      showText(textoBomba,0,1);
      showText("Regando...",1,0);
      digitalWrite(D8, HIGH);
      timer = millis();
      
    }else{
      showText("Desposito vacío",0,1);
      showText("Stop!!",1,0);
      digitalWrite(D8, LOW);
      
    }
  }

  if(timer != 0 && activeSwitch == HIGH){
    activateDisplay();
    //Asignamos la humedad del suelo antes de regar.
    humidityOld = humidity;
    
    textoBomba = "Hum. baja " + String(humidity) + "%";

    //activamos el motor de la bomba 10 segundos
    showText(textoBomba,0,1);
    showText("Regando...",1,0);
    
    if((timer  + timerBomba) < millis()){
      textoBomba = "Humedad " + String(humidity) + "%";

      //activamos el motor de la bomba 10 segundos
      showText(textoBomba,0,1);
      showText("Fin regado",1,0);
    
      digitalWrite(D8, LOW);
      timer = 0;

      delay(1000);
      
      if(humidityOld >= humidity){
        showText("No ha regado!!",0,1);
        humidityOld = humidity;
        failedTimes = failedTimes + 1;
        delay(1000);
      }
      
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
  if(!getIsSaving()){
    sensorValues(sensorTemp.readTemperature(AHT10_USE_READ_DATA),sensorTemp.readHumidity(AHT10_USE_READ_DATA),humidity,timer,activeSwitch,umbralHum,timerBomba);
  }else{
    activeSwitch = getSwitch();
    umbralHum = getUmbral();
    timerBomba = getTiempo();
    setIsSaving(false);
  }

  if(getRegando()){
    Serial.println("Activo el flag de riego!!"); 
    isRiegoManual = 1;
    setRegando(false);
  }
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

  Serial.print("T: ");
  Serial.print(temp);
  Serial.println(" +-0.3 C ");
  
  Serial.print("H: ");
  Serial.print(hum);
  Serial.println(" +-2 %");
}

void activateBackLight(){
  lcd.backlight();  
}

void desactivateBackLight(){
  lcd.noBacklight();  
}

void activateDisplay(){
  lcd.display();
  lcd.backlight();  
}

void desactivateDisplay(){
  lcd.noDisplay();  
  lcd.noBacklight();
}
