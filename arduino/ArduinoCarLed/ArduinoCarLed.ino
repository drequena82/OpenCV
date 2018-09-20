/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
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

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  for(int pin=22;pin<=35;pin++)
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
  
}

// the loop routine runs over and over again forever:
void loop() {
  
  
  for(int num=0;num<99;num=num+10)
  {
    showNumber1(num%10);
    showNumber2(num/10);
    
    delay(1000);
  }
  
  
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
