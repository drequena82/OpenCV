#include <MPU6050.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <AFMotor.h>
#include <SoftwareSerial.h>

// D3, D5, D6, D11 <-- Pines para los motores
// D7, D8, D12     <-- Pines para shift register
// D9, D10         <-- Pines para los servos/steppers 
const int echoPin = 50;
const int triggerPin = 52;
SoftwareSerial btPort(53,51); // RX, TX (TX DEL MODULO BT,RX DEL MODULO BT)

const int ledPin = 13;

AF_DCMotor MotorFL(1);//A BACKGUARD
AF_DCMotor MotorFR(2);//B
AF_DCMotor MotorRL(3);//B
AF_DCMotor MotorRR(4);//A

int speedFL = 100;
int speedFR = 100;
int speedRL = 100;
int speedRR = 100;
int param = 10;
int serialSpeed = 9600;
int bluetoothControl = 0;
int floorDistance = 0;

void setup() 
{
  Serial.begin(serialSpeed);           // set up Serial library at 9600 bps
  btPort.begin(serialSpeed);
  
  pinMode(ledPin, OUTPUT);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  MotorFL.run(RELEASE);
  MotorFR.run(RELEASE);
  MotorRL.run(RELEASE);
  MotorRR.run(RELEASE);  
  
  while(!btPort.available())
  {
  }
  
  if(btPort.available())
  {
      char c = btPort.read();
      
      while ( c != '\n')//Hasta que el character sea intro
      {     
        if(c == 'b' && !bluetoothControl)
        {
          bluetoothControl = 1;
        }else
        {
          delay(25);
          c = btPort.read();
        }
      }
    
    Serial.print("flag control bt: ");
    Serial.println(bluetoothControl);  
    
    btPort.println("BT activado");
  }
  
}

void loop() 
{
  char option = 'z';

  //Control por bluetooth
  if(bluetoothControl)
  {
    
    delay(30);

    while(btPort.available())
    {
      option=btPort.read();
      if(option == 's')
      {
        speedFL = 0;
        speedFR = 0;
        speedRL = 0;
        speedRR = 0;
        
        restart();
        
        MotorFL.run(RELEASE);
        MotorFR.run(RELEASE);
        MotorRL.run(RELEASE);
        MotorRR.run(RELEASE);
        
        Serial.println("Stop");
        btPort.println("Stop");
      }else if(option == 'i')
      {
        MotorFL.run(BACKWARD);
        MotorFR.run(FORWARD);
        MotorRL.run(FORWARD);
        MotorRR.run(BACKWARD);
        
        speedFL = 100;
        speedFR = 100;
        speedRL = 100;
        speedRR = 100;

        restart();
        Serial.println("inicio");
        btPort.println("start");
      }else if(option == 'r')
      {
        restart();
        Serial.println("restart");
        btPort.println("restart");
      }else if(option=='u') 
      {
        up(param);
        Serial.println("up");
        btPort.println("up");
      }else if(option=='d') 
      {
        down(param);
        Serial.println("down");
        btPort.println("down");
      }else if(option=='l') 
      {
        left(param);
        Serial.println("left");
        btPort.println("left");
      }else if(option=='r') 
      {
        right(param);
        Serial.println("right");
        btPort.println("right");
      }else if(option=='f') 
      {
        forward(param);
        Serial.println("forward");
        btPort.println("forward");
      }else if(option=='k') 
      {
        back(param);
        Serial.println("back");
        btPort.println("back");
      }
    }
  }else
  {
    //TODO:
    floorDistance = ping(triggerPin, echoPin);
    Serial.print("Distancia: ");
    Serial.println(floorDistance);

    //Control de choque con el sensor de ultrasonido
    if(floorDistance <= 10)
    {
      up(param);
    }
  }
}

int ping(int triggerPin, int echoPin) 
{
  long duration, distanceCm;
  
  digitalWrite(triggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(triggerPin, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos
  
  distanceCm = duration * 10 / 292/ 2;   //convertimos a distancia, en cm
  return distanceCm;
}

void restart()
{
    int average = speedFL + speedRL + speedRL + speedRR;
    
    speedFL = average / 4;
    speedRL = average / 4;
    speedFR = average / 4;
    speedRR = average / 4;

    updateSpeed();
}

void up(int num)
{
    speedFL +=num;
    speedRL +=num;
    speedFR +=num;
    speedRR +=num;

    updateSpeed();
}

void down(int num)
{
    speedFL -=num;
    speedRL -=num;
    speedFR -=num;
    speedRR -=num;

    updateSpeed();
}

void left(int num)
{
    speedFL -=num;
    speedRL -=num;
    speedFR +=num;
    speedRR +=num;

    updateSpeed();
}

void right(int num)
{
    speedFL +=num;
    speedRL +=num;
    speedFR -=num;
    speedRR -=num;

    updateSpeed();
}

void forward(int num)
{
    speedFL -=num;
    speedRL +=num;
    speedFR -=num;
    speedRR +=num;

    updateSpeed();
}

void back(int num)
{
    speedFL +=num;
    speedRL -=num;
    speedFR +=num;
    speedRR -=num;

    updateSpeed();
}

void updateSpeed()
{
   if(speedRL > 0 && speedRL < 255)
   {
     MotorFL.setSpeed(speedRL);
   }
   if(speedRR > 0 && speedRR < 255)
   {
     MotorFR.setSpeed(speedRR);
   }
   if(speedFR > 0 && speedFR < 255)
   {
     MotorRL.setSpeed(speedFR);
   }
   if(speedFL > 0 && speedFL < 255)
   {
     MotorRR.setSpeed(speedFL);
   }
   
   Serial.print("speed motor 1: ");
   Serial.println(speedRL);
   
   Serial.print("speed motor 2: ");
   Serial.println(speedRR);
      
   Serial.print("speed motor 3: ");
   Serial.println(speedFR);
   
   Serial.print("speed motor 4: ");
   Serial.println(speedFL);
   
   btPort.print("speed motor 1: ");
   btPort.println(speedRL);
   
   btPort.print("speed motor 2: ");
   btPort.println(speedRR);
      
   btPort.print("speed motor 3: ");
   btPort.println(speedFR);
   
   btPort.print("speed motor 4: ");
   btPort.println(speedFL);
}
