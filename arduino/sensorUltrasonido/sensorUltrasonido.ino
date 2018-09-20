#include <SoftwareSerial.h>
#include <AFMotor.h>

int num0a[] = {0,0,0,1,0,0,0};//arriba der 22,  abajo der 23,  arriba 24,centro 25,  arriba izq 26, abajo 27, abajo izq 28
int num1a[] = {0,0,1,1,1,1,1};
int num2a[] = {0,1,0,0,1,0,0};
int num3a[] = {0,0,0,0,1,0,1};
int num4a[] = {0,0,1,0,0,1,1};
int num5a[] = {1,0,0,0,0,0,1};
int num6a[] = {1,0,0,0,0,0,0};
int num7a[] = {0,0,0,1,1,1,1};
int num8a[] = {0,0,0,0,0,0,0};
int num9a[] = {0,0,0,0,0,1,1};

int num0b[] = {1,1,1,1,1,1,1};//arriba der 29, abajo der 30,  punto,abajo der,arriba,arriba izq,punto,abajo der, centro, abajo, abajo izq
int num1b[] = {0,0,1,1,1,1,1};
int num2b[] = {1,0,0,0,0,0,1};
int num3b[] = {0,0,0,0,1,0,1};
int num4b[] = {0,0,1,1,1,0,0};
int num5b[] = {0,1,0,0,1,0,0};
int num6b[] = {0,1,0,0,0,0,0};
int num7b[] = {0,0,1,0,1,1,1};
int num8b[] = {0,0,0,0,0,0,0};
int num9b[] = {0,0,1,0,1,0,0};

// D3, D5, D6, D11 <-- Pines para los motores
// D7, D8, D12     <-- Pines para shift register
// D9, D10         <-- Pines para los servos/steppers 

const int echoPin = 50;
const int triggerPin = 52;

SoftwareSerial bt(53,51);
AF_DCMotor MotorBL(1);
AF_DCMotor MotorBR(2);
AF_DCMotor MotorFR(3);
AF_DCMotor MotorFL(4);

void setup() 
{
  Serial.begin(9600);           // set up Serial library at 9600 bps
  bt.begin(9600);
  
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  for(int pin=22;pin<=35;pin++)
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
  
  MotorFL.run(RELEASE);
  MotorFR.run(RELEASE);
  MotorBL.run(RELEASE);
  MotorBR.run(RELEASE);
}

void loop() 
{
  int cm = ping(triggerPin, echoPin);
  
  Serial.print("Distancia: ");
  Serial.println(cm);
  
  if(cm < 100)
  {
    showNumber1(cm%10);
    showNumber2(cm/10);
  }else
  {
    showNumber1(9);
    showNumber2(9);
  }
  if(cm < 10)
  {
    MotorFL.run(FORWARD);
    MotorFR.run(FORWARD);
    MotorBL.run(FORWARD);
    MotorBR.run(FORWARD);
  }else
  {
    MotorFL.run(BACKWARD);
    MotorFR.run(BACKWARD);
    MotorBL.run(BACKWARD);
    MotorBR.run(BACKWARD);
  }
  
  MotorFL.setSpeed(cm);
  MotorFR.setSpeed(cm);
  MotorBL.setSpeed(cm);
  MotorBR.setSpeed(cm);
  
  
  if(bt.available())
  {
    Serial.write(bt.read());  
  }
  
  if(Serial.available())
  {
    String s = getLine();
    bt.print(s);
  
    Serial.println("--->" + s);  
  }
  
  bt.write(cm);
  
  delay(30);
}

String getLine()
{
  String s = "";
  if(Serial.available())
  {
    char c = Serial.read();
    while(c != '\n')
    {
      s = s + c;
      delay(25);
      c = Serial.read();      
    }
    
    Serial.println(s);
    
    return(s + '\n');
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

void showNumber1(int number)
{
  //arriba der, arriba, centro, arriba izq,punto,abajo der,abajo,abajo izq 
  
  int* ledNum;
  
  Serial.println(number);
  if(number == 0)
  {
     ledNum = num0a;
  }else if(number == 1)
  {
    ledNum = num1a;
  }else if(number == 2)
  {
     ledNum = num2a;
  }else if(number == 3)
  {
   ledNum = num3a;
  }else if(number == 4)
  {
   ledNum = num4a;
  }else if(number == 5)
  {
   ledNum = num5a;
  }else if(number == 6)
  {
    ledNum = num6a;
  }else if(number == 7)
  {
    ledNum = num7a;
  }else if(number == 8)
  {
    ledNum = num8a;
  }else if(number == 9)
  {
    ledNum = num9a;
  }
  
  for(int pin=22,pos=0;pin<=28;pin++,pos++)
  {
    digitalWrite(pin, ledNum[pos]);
  }
}

void showNumber2(int number)
{
  //arriba der, arriba, centro, arriba izq,punto,abajo der,abajo,abajo izq 
  
  int* ledNum;
  
  Serial.println(number);
  if(number == 0)
  {
     ledNum = num0b;
  }else if(number == 1)
  {
    ledNum = num1b;
  }else if(number == 2)
  {
     ledNum = num2b;
  }else if(number == 3)
  {
   ledNum = num3b;
  }else if(number == 4)
  {
   ledNum = num4b;
  }else if(number == 5)
  {
   ledNum = num5b;
  }else if(number == 6)
  {
    ledNum = num6b;
  }else if(number == 7)
  {
    ledNum = num7b;
  }else if(number == 8)
  {
    ledNum = num8b;
  }else if(number == 9)
  {
    ledNum = num9b;
  }
  
  for(int pin=29,pos=0;pin<=35;pin++,pos++)
  {
    digitalWrite(pin, ledNum[pos]);
  }
}


