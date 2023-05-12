/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/
#include <SPI.h>
#include <SD.h>
#include <TMRpcm.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define DEBUG 1
#define SD_ChipSelectPin 4
#define TIMER 2000 
#define LED_PIN 3
#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 9

int red = 0;
int green = 0;
int blue = 0;
byte color = 2;

Adafruit_MPU6050 mpu;
TMRpcm tmrpcm;
bool ledOn = false;
bool saberOnBefore = false;
unsigned int counterButton = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // initialize digital pin LED_BUILTIN as an output.
  //pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

   if (!SD.begin(4)) {
     println("initialization failed!");
     while (1);
   }

   tmrpcm.speakerPin = 10;
   tmrpcm.setVolume(5);
   tmrpcm.quality(1);
  
  // Try to initialize!
  if (!mpu.begin()) {
    println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  println("MPU6050 Found!");

  //setupt motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);

  // colores del sable
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  delay(1000);
}

// the loop function runs over and over again forever
void loop() {
  
  isLedOn();

  if(ledOn){
    if(!saberOnBefore){
      onFadeSaber();
      saberOnBefore = true;
    }
    standBySound();
    detectMove();
    digitalWrite(LED_PIN, HIGH);  
    //digitalWrite(LED_BUILTIN, HIGH);
  }else{
    if(saberOnBefore){
      offFadeSaber();
      saberOnBefore = false;
    }
    digitalWrite(LED_PIN, LOW);   
    //digitalWrite(LED_BUILTIN, LOW);
    tmrpcm.disable();
  }
  delay(30);
}

void standBySound(){
  if(tmrpcm.isPlaying() == 0){
    tmrpcm.disable();
    playSound("HUM.wav");
  }
}

void isLedOn(){
  unsigned int buttonPress = analogRead(A6);
  
  float voltage = buttonPress * (5.0 / 1023.0);
  // > y < (mayor y menor)
  // Mantener pulsado el botón para realizar acciones
  if(voltage > 3){
    if (counterButton == 0){
      counterButton = millis();    
      println("PULSACION");
      //if(ledOn){
        //println("PULSACION CORTA");
        //color = color + 1;
        // Tenemos 6 colores
        //color = color % 6;
        //changeColor();
      //}
    }else if(millis() >= counterButton + TIMER){
      println("PULSACION LARGA");
      flashLed();
      counterButton = 0;
      ledOn = !ledOn;
    }
  }else{
    counterButton = 0;
  }
}

void flashLed(){
  for(unsigned int i = 0; i < 4 ; i ++){
    digitalWrite(LED_BUILTIN,HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN,LOW);
    delay(100);
  }
}

void changeColor(){
  setColor();
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);  
}

void detectMove(){
  if (mpu.getMotionInterruptStatus()) {                            
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      
      print("accel x: ");
      println(a.acceleration.x);
      print("accel y: ");
      println(a.acceleration.y);
      print("accel z: ");
      println(a.acceleration.z);
      println("");
      print("gyro x: ");
      println(g.gyro.x);
      print("gyro y: ");
      println(g.gyro.y);
      print("gyro z: ");
      print(g.gyro.z);
      println("");

      //Recogemos los valores y decidimos los audios que se van a ejecutar.
      if(!tmrpcm.isPlaying()){
        int swingNum = random(1,5);
        switch(swingNum){
          case 1:
            playSound("SWS1.wav");
            break;
          case 2:
            playSound("SWS2.wav");
            break;
          case 3:
            playSound("SWS3.wav");
            break;
          case 4:
            playSound("SWS4.wav");
            break;
          case 5:
            playSound("SWS5.wav");
            break;
          default:
            playSound("SWS1.wav");
            break;
        }
      }
  }
}
void playSound(char* sound){
  if(tmrpcm.isPlaying() == 0){
    tmrpcm.disable();
    tmrpcm.play(sound);
  }
}

void setColor() {
  switch (color) {
    // 0 - red, 1 - green, 2 - blue, 3 - pink, 4 - yellow, 5 - ice blue
    case 0:
      red = 255;
      green = 0;
      blue = 0;
      break;
    case 1:
      red = 0;
      green = 0;
      blue = 255;
      break;
    case 2:
      red = 0;
      green = 255;
      blue = 0;
      break;
    case 3:
      red = 255;
      green = 0;
      blue = 255;
      break;
    case 4:
      red = 255;
      green = 255;
      blue = 0;
      break;
    case 5:
      red = 0;
      green = 255;
      blue = 255;
      break;
  }
}

void onFadeSaber(){
  // asignamos los colores
  setColor();

  //Sonidos al arrancar
  playSound("ON.wav");

  // fade in from min to max in increments of 5 points:
  for (int fadeR = 0, fadeG = 0, fadeB = 0; fadeR <= red || fadeG <= green || fadeB <= blue; fadeR += 5, fadeG += 5, fadeB += 5) {
    // sets the value (range from 0 to 255):
    if(fadeR <= red){
      analogWrite(RED_PIN, fadeR);
    }
    
    if(fadeG <= green){
      analogWrite(GREEN_PIN, fadeG);
    }

    if(fadeB <= blue){
      analogWrite(BLUE_PIN, fadeB);
    }
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
}

void offFadeSaber(){

  // Sonidos al apagar.
  playSound("OFF.wav");

  // fade in from min to max in increments of 5 points:
  for (int fadeR = red, fadeG = green, fadeB = blue; fadeR >= 0 || fadeG >= 0 || fadeB >= 0; fadeR -= 5, fadeG -= 5, fadeB -= 5) {
    // sets the value (range from 0 to 255):
    if(fadeR >= 0){
      analogWrite(RED_PIN, fadeR);
    }
    
    if(fadeG >= 0){
      analogWrite(GREEN_PIN, fadeG);
    }

    if(fadeB >= 0){
      analogWrite(BLUE_PIN, fadeB);
    }
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
}

void clashSaber(){
  playSound("SK8.wav");

  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);
  delay(100);
  
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
  delay(100);
  
  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);
  delay(100);
  
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

void print(String str){
  if(DEBUG == 1){
    Serial.print(str);
  }
}


void println(String str){
  if(DEBUG == 1){
    Serial.println(str);
  }
}

void print(float str){
  if(DEBUG == 1){
    Serial.print(str);
  }
}


void println(float str){
  if(DEBUG == 1){
    Serial.println(str);
  }
}


