00/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
int num0a[] = {0,0,1,0,1,0,0,0};
int num1a[] = {0,1,1,1,1,0,1,1};
int num2a[] = {0,0,0,1,1,1,0,0};
int num3a[] = {0,0,0,1,1,0,0,1};
int num4a[] = {0,1,0,0,1,0,1,1};
int num5a[] = {1,0,0,0,1,0,0,1};
int num6a[] = {1,0,0,0,1,0,0,0};
int num7a[] = {0,0,1,1,1,0,1,1};
int num8a[] = {0,0,0,0,1,0,0,0};
int num9a[] = {0,0,0,0,1,0,1,1};

int num0b[] = {0,0,0,1,0,1,0,0};//arriba der,arriba,arriba izq,punto,abajo der, centro, abajo, abajo izq
int num1b[] = {0,1,1,1,0,1,1,1};
int num2b[] = {0,0,1,1,1,0,0,0};
int num3b[] = {0,0,1,1,0,0,0,1};
int num4b[] = {0,1,0,1,0,0,1,1};
int num5b[] = {1,0,0,1,0,0,0,1};
int num6b[] = {1,0,0,1,0,0,0,0};
int num7b[] = {0,0,1,1,0,1,1,1};
int num8b[] = {0,0,0,1,0,0,0,0};
int num9b[] = {0,0,0,1,0,0,1,1};

// the setup routine runs once when you press reset:
void setup() {                
  
  Serial.begin(9600);
  
  // initialize the digital pin as an output.
  for(int pin= 38;pin<54;pin++)
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, 1);
  }
  
  //arriba der, arriba, centro, arriba izq,punto,abajo der,abajo,abajo izq 
  
  //
}

// the loop routine runs over and over again forever:
void loop() {
  /*
  for(int pin= 38;pin<53;pin =pin + 2)
  {
    digitalWrite(pin, 0);
    
    Serial.println(pin);
    
    delay(1000);
    
    digitalWrite(pin, 1);
  }
  
  for(int pin= 39;pin<54;pin =pin + 2)
  {
    digitalWrite(pin, 0);
    
    Serial.println(pin);
    
    delay(1000);
    
    digitalWrite(pin, 1);
  }
  */
  
  for(int n = 0;n < 6;n++)
  {
    for(int n2 = 0;n2 < 10;n2++)
    {
      Serial.print(n);
      Serial.print(n2);
      showNumber1(n);
      showNumber2(n2);
      delay(1000);
    }
  }
  
  /*
  while(Serial.available() > 0)
  {
    char option = Serial.read();
    if(option == 'a')
    {
      for(int i=0;i<10;i++)
      {
        showNumber(i);
        delay(1000);
      }
    }else
    {
      option -= '0';
      if(option >= 0 && option <=9)
      {
        showNumber((int)option);
      }
    }
  }
  */
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
  
  for(int led = 35,pos = 0;led<50;led = led + 2,pos++)
  {
    digitalWrite(led, ledNum[pos]);
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
  
  for(int led = 34,pos = 0;led<49;led = led + 2,pos++)
  {
    digitalWrite(led, ledNum[pos]);
  }
}
