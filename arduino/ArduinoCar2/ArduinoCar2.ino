#include <AFMotor.h>


int ledL1 = 22;
int ledL2 = 23;

int ledR1 = 24;
int ledR2 = 25;

const int echoPinL = 26;
const int triggerPinL = 27;

const int echoPinR = 28;
const int triggerPinR = 29;

AF_DCMotor MotorBL(1);
AF_DCMotor MotorBR(2);
AF_DCMotor MotorFR(3);
AF_DCMotor MotorFL(4);

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  Serial.begin(9600);
  
  MotorFL.run(RELEASE);
  MotorFR.run(RELEASE);
  MotorBL.run(RELEASE);
  MotorBR.run(RELEASE);
  
  pinMode(triggerPinR, OUTPUT);
  pinMode(echoPinR, INPUT);
  pinMode(triggerPinL, OUTPUT);
  pinMode(echoPinL, INPUT);
  
  pinMode(ledL1, OUTPUT); 
  pinMode(ledL2, OUTPUT); 
  pinMode(ledR1, OUTPUT); 
  pinMode(ledR2, OUTPUT); 
  
  digitalWrite(ledL1, LOW);   
  digitalWrite(ledL2, HIGH);   
  
  digitalWrite(ledR1, LOW);   
  digitalWrite(ledR2, HIGH);   
       
}

// the loop routine runs over and over again forever:
void loop() {
  
  int distanceL = 0;
  int distanceR = 0;
  
  distanceL = ping(triggerPinL,echoPinL);
  
  if(distanceL > 30)
  {
    digitalWrite(ledL1, LOW);   
    digitalWrite(ledL2, HIGH);    
  }else
  {
    digitalWrite(ledL1, HIGH);   
    digitalWrite(ledL2, LOW);
  }
  
  distanceR = ping(triggerPinR,echoPinR);
  
  if(distanceR < 30)
  {              
    digitalWrite(ledR1, LOW);   
    digitalWrite(ledR2, HIGH);    
  }else
  {
    digitalWrite(ledR1, HIGH);   
    digitalWrite(ledR2, LOW);      
  }  
  
  Serial.print("DistanciaL: ");
  Serial.println(distanceL);
  
  Serial.print("DistanciaR: ");
  Serial.println(distanceR);
  
  speedControl(distanceL,distanceR);
  
  delay(100);
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

void speedControl(int distanceL,int distanceR)
{
  int speedFR = 152;
  int speedFL = 152;
  int speedBR = 152;
  int speedBL = 152;
  
  if(distanceL < 30) //Para girar a la derecha para las ruedas de la derecha
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
    }else if(distanceR < 30)
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
      if(distanceL < 60)
      {
        speedFR = 75;
        speedBR = 75;
      }else if(distanceL < 80)
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
      
      if(distanceR < 60)
      {
        speedFL = 75;
        speedBL = 75;
      }else if(distanceR < 80)
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
}
