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

const int echoPinL = 50;
const int triggerPinL = 52;

const int echoPinR = 47;
const int triggerPinR = 49;

int bluetoothControl = 0;

SoftwareSerial bt(53,51);
AF_DCMotor MotorBL(1);
AF_DCMotor MotorBR(2);
AF_DCMotor MotorFR(3);
AF_DCMotor MotorFL(4);

void setup() 
{
  Serial.begin(9600);           // set up Serial library at 9600 bps
  bt.begin(9600);
  
  pinMode(triggerPinL, OUTPUT);
  pinMode(echoPinL, INPUT);

  pinMode(triggerPinR, OUTPUT);
  pinMode(echoPinR, INPUT);

  for(int pin=22;pin<=35;pin++)
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
  
  for(int pin=36;pin<=44;pin=pin+2)
  {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
  }
  
  MotorFL.run(RELEASE);
  MotorFR.run(RELEASE);
  MotorBL.run(RELEASE);
  MotorBR.run(RELEASE);
  
  while(!bt.available())
  {
  }
  
  bt.print("Activar control bluetooth? ");
  
  if(bt.available())
  {
      char c = bt.read();
      
      while ( c != '\n')//Hasta que el character sea intro
      {     
        if(c == 's')
        {
          bluetoothControl = 1;
          
          bt.print(c);
        }else if(c == 'n')
        {
          bluetoothControl = 0;
          
          bt.print(c);
        }else
        {
          delay(25);
          c = bt.read();
        }
      }
    
    Serial.print("flag control bt: ");
    Serial.println(bluetoothControl);  
    
  }
  
  //delay(10000);
}

void loop() 
{
  int cmL = ping(triggerPinL, echoPinL);
  int cmR = ping(triggerPinR, echoPinR);
  
  int speedFR = 152;
  int speedFL = 152;
  int speedBR = 152;
  int speedBL = 152;
  
  Serial.print("Distancia: ");
  Serial.print(cmL);
  Serial.print(" - ");
  Serial.println(cmR);
  
  if(cmR < 100)
  {
    showNumber1(cmR%10);
    showNumber2(cmR/10);
  }else
  {
    showNumber1(9);
    showNumber2(9);
  }
  
  //VUMETRO
  printVumeter(cmL);
  
  if(bluetoothControl == 0)
  {
    
    if(cmL < 30) //Para girar a la derecha para las ruedas de la derecha
    {
      speedFR = 0;
      speedBR = 0;
      
      speedFL = 150;
      speedBL = 150;
    
      MotorFR.setSpeed(speedFR);
      MotorBR.setSpeed(speedBR);
    
      MotorFR.run(RELEASE);
      MotorBR.run(RELEASE);
      
      MotorFL.setSpeed(speedFL);
      MotorBL.setSpeed(speedBL);
      
      MotorFL.run(FORWARD);
      MotorBL.run(FORWARD);
      
      //delay(2000);
    }else if(cmR < 30)
    {
    
      speedFL = 0;
      speedBL = 0;
    
      speedFR = 150;
      speedBR = 150;
      
      MotorFL.setSpeed(speedFL);
      MotorBL.setSpeed(speedBL);
      
      MotorFL.run(RELEASE);
      MotorBL.run(RELEASE);
      
      MotorFR.setSpeed(speedFR);
      MotorBR.setSpeed(speedBR);
    
      MotorFR.run(FORWARD);
      MotorBR.run(FORWARD);
      
      //delay(2000);
    }else
    {
      if(cmL < 60)
      {
        speedFR = 75;
        speedBR = 75;
      }else if(cmL < 80)
      {
        speedFR = 125;
        speedBR = 125;
      }else
      {
        speedFR = 175;
        speedBR = 175;
      }
      
      MotorFR.setSpeed(speedFR);
      MotorBR.setSpeed(speedBR);
    
      MotorFR.run(FORWARD);
      MotorBR.run(FORWARD);
      
      if(cmR < 60)
      {
        speedFL = 75;
        speedBL = 75;
      }else if(cmR < 80)
      {
        speedFL = 125;
        speedBL = 125;
      }else
      {
        speedFL = 175;
        speedBL = 175;
      }
    
      MotorFL.setSpeed(speedFL);
      MotorBL.setSpeed(speedBL);
      
      MotorFL.run(FORWARD);
      MotorBL.run(FORWARD);
    }
    
  }else if(bluetoothControl == 0)
  {
    //Recogemos el movimiento
    char movement = ' ';
    
    if(movement == 'r') //Para girar a la derecha para las ruedas de la derecha
    {
      speedFR = 0;
      speedBR = 0;
      
      speedFL = 150;
      speedBL = 150;
    
      MotorFR.setSpeed(speedFR);
      MotorBR.setSpeed(speedBR);
    
      MotorFR.run(RELEASE);
      MotorBR.run(RELEASE);
      
      MotorFL.setSpeed(speedFL);
      MotorBL.setSpeed(speedBL);
      
      MotorFL.run(FORWARD);
      MotorBL.run(FORWARD);
      
      //Aumentamos el delay para que pueda girar
      delay(2000);
      
    }else if(movement == 'l')
    {
    
      speedFL = 0;
      speedBL = 0;
    
      speedFR = 150;
      speedBR = 150;
      
      MotorFL.setSpeed(speedFL);
      MotorBL.setSpeed(speedBL);
      
      MotorFL.run(RELEASE);
      MotorBL.run(RELEASE);
      
      MotorFR.setSpeed(speedFR);
      MotorBR.setSpeed(speedBR);
    
      MotorFR.run(FORWARD);
      MotorBR.run(FORWARD);
      
      //Aumentamos el delay para que pueda girar
      delay(2000);
    }else
    {
      
      speedFR = 125;
      speedBR = 125;
      
      MotorFR.setSpeed(speedFR);
      MotorBR.setSpeed(speedBR);
    
      MotorFR.run(FORWARD);
      MotorBR.run(FORWARD);
      
      speedFL = 125;
      speedBL = 125;
      
      MotorFL.setSpeed(speedFL);
      MotorBL.setSpeed(speedBL);
      
      MotorFL.run(FORWARD);
      MotorBL.run(FORWARD);
    }
    
    bt.print("Introducir movimiento: ");
  
    if(bt.available())
    {
        char c = bt.read();
        
        while ( c != '\n')//Hasta que el character sea intro
        {     
            delay(25);
            c = bt.read();
            
            movement = c;
            
            if(movement == 'r' || movement == 'l')
            {
              bt.print(movement);
            }
        }
    }
    
  }
  
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
  
  bt.write(cmL);
  
  delay(30);
}

void printVumeter(int cm)
{
  if(cm > 0)
  {
    digitalWrite(36, HIGH);
  }else
  {
    digitalWrite(36, LOW);
  }
  
  if(cm>20)
  {
    digitalWrite(38, HIGH);
  }else
  {
    digitalWrite(38, LOW);
  }
  
  if(cm>40)
  {
    digitalWrite(40, HIGH);
  }else
  {
    digitalWrite(40, LOW);
  }
  
  if(cm>60)
  {
    digitalWrite(42, HIGH);
  }else
  {
    digitalWrite(42, LOW);
  }
  
  if(cm>80)
  {
    digitalWrite(44, HIGH);
  }else
  {
    digitalWrite(44, LOW);
  }
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


