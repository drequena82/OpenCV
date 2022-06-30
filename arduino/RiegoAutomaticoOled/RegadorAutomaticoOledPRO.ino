#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "AHT10.h"

#define TIMER_LCD 5000
#define FAILED_TIMES 3

#define D2 2 //PONER LOS PINES CORRECTOS
#define D12 12
#define D4 4
#define A1 15

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

AHT10 sensorTemp(AHT10_ADDRESS_0X38);

int timerBomba = 3000;
int umbralHum = 20;
int readStatus = 0;
int sensorValue = 0;
int timer = 0;
int humidity = 0;
long timeIni = 0;
long timeFin = 0;
long timeButton = 0;
int humidityOld = 0;
int failedTimes = 0;
int isRiegoManual = 0;

void setup(void) 
{
  Serial.begin(115200);
  
  pinMode(D2,OUTPUT);
  pinMode(D12,INPUT);
  pinMode(D4,INPUT);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    //Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();

  delay(1000);
  
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  while (sensorTemp.begin() != true)
  {
    showText("Inicializando...",0,1);
  }
  
  delay(1000);
  
  readStatus = sensorTemp.readRawData();

  delay(100);
  
  if(readStatus != AHT10_ERROR){
    //showSensor(sensorTemp.readTemperature(AHT10_USE_READ_DATA),sensorTemp.readHumidity(AHT10_USE_READ_DATA),0);
    showText("Sensor de temperatura y humedad conectado!!",0,1);
  }else{
    showText("Se ha perdido la conexión con los sensores",0,1);
  }
  
  delay(3000);
  
  display.clearDisplay();
}
 
void loop()
{ 
   int pressButton = 0;
   int pressButtonRiego = 0;
   String textoBomba = "";
   
   humidity = 0; 
   readStatus = sensorTemp.readRawData();
   sensorValue = analogRead(A1);   
   humidity=map(sensorValue,0,1024,100, 0);

   pressButton = digitalRead(D12);
   pressButtonRiego = digitalRead(D4);
   
   if(pressButton == HIGH){
    Serial.println("pulsado");
      timeButton = millis();
   }

   if(pressButtonRiego == HIGH){
    Serial.println("pulsadoR");
      isRiegoManual=1;
   }
   
  if((humidity < umbralHum || isRiegoManual) && timeButton == 0){
    //Asignamos la humedad del suelo antes de regar.
    humidityOld = humidity;
    
    
    //Si no ha subido la humedad deja de regar
    if(failedTimes < FAILED_TIMES || isRiegoManual == 1){
      textoBomba = "Humedad suelo: " + String(humidity) + "%";
      showText(textoBomba,"Regando...","","");
      digitalWrite(D2, HIGH);
      timer = millis();
      isRiegoManual = 0;
    }else{
      digitalWrite(D2, LOW);
      if(timeButton == 0){
        showText("Deposito vacio!!","Compruebe deposito","Reinicie :(","");

        delay(2000);

        off();

        timer = 0;
      }
    }
  }

  if(timer > 0 && timeButton == 0 ){
    //Asignamos la humedad del suelo antes de regar.
    humidityOld = humidity;
    textoBomba = "Humedad suelo: " + String(humidity) + "%";
    showText(textoBomba,"Regando...","","");
    
    if((timer  + timerBomba) < millis()){
      textoBomba = "Humedad " + String(humidity) + "%";
      showText(textoBomba,"Fin riego!!","","");
      digitalWrite(D2, LOW);
      timer = 0;

      delay(1000);
      
      off();
      
      if(humidityOld >= humidity){
        showText("No ha regado!!",0,1);
        humidityOld = humidity;
        failedTimes = failedTimes + 1;
        delay(1000);

        off();
      }
    }
  }else{   
    if(timeButton + TIMER_LCD > millis()){
      if (readStatus != AHT10_ERROR){
        showSensor(sensorTemp.readTemperature(AHT10_USE_READ_DATA),sensorTemp.readHumidity(AHT10_USE_READ_DATA),humidity);
      }else{
        showText("Se ha perdido la conexión con los sensores",0,1);
      }
    }else{
      timeButton=0;
      off();
    }
  }
}

void showText(String text1,String text2,String text3,String text4){
  
  display.clearDisplay();
  
  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  display.setCursor(0, 0);
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
   // Escribimos el número de segundos trascurridos

  display.println(text1);
  //Serial.println(text1);

  display.println(text2);
  //Serial.println(text2);

  display.println(text3);
  //Serial.println(text3);
  
  display.println(text4);
  //Serial.println(text4);
  
  display.display();
}

void showText(String text,int linea,int clearDisplay){
  
  if(clearDisplay == 1){
    display.clearDisplay();
  }
  
  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  display.setCursor(0, linea);
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
   // Escribimos el número de segundos trascurridos
  display.print(text);
  //Serial.println(text);
  display.display();
}

void showSensor(float temp,float hum,int moist){
  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
  display.print("Temp.: ");
  display.print(temp);
  display.print(" +-0.3 C ");
  
  display.println("");
  display.print("Humedad: ");
  display.print(hum);
  display.print(" +-2 %");

  display.println("");
  display.print("H. suelo: ");
  display.print(moist);
  display.print(" %");
   
  //Serial.print("T: ");
  //Serial.print(temp);
  //Serial.println(" +-0.3 C ");
  
  //Serial.print("H: ");
  //Serial.print(hum);
  //Serial.println(" +-2 %");

  //display.startscrollright(0x00, 0x0F);
  display.display();
  delay(200);
}

void off(){
  display.clearDisplay();
  display.display();
}
