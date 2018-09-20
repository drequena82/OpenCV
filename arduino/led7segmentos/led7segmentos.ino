/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int leda = 12;
int ledb = 11;
int ledc = 10;
int ledd = 9;
int lede = 8;
int ledf = 7;
int ledg = 6;

// the setup routine runs once when you press reset:
void setup() {                
  
  Serial.begin(9600);
  
  // initialize the digital pin as an output.
  pinMode(leda, OUTPUT);
  pinMode(ledb, OUTPUT);
  pinMode(ledc, OUTPUT);
  pinMode(ledd, OUTPUT);
  pinMode(lede, OUTPUT);
  pinMode(ledf, OUTPUT);
  pinMode(ledg, OUTPUT);
  
  digitalWrite(leda, 1);    // turn the LED off by making the voltage 0
  digitalWrite(ledb, 1);    // turn the LED off by making the voltage 0
  digitalWrite(ledc, 1);    // turn the LED off by making the voltage 0
  digitalWrite(ledd, 1);    // turn the LED off by making the voltage 0
  digitalWrite(lede, 1);    // turn the LED off by making the voltage 0
  digitalWrite(ledf, 1);    // turn the LED off by making the voltage 0
  digitalWrite(ledg, 1);    // turn the LED off by making the voltage 0
  
}

// the loop routine runs over and over again forever:
void loop() {
  
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
  
  /*
  digitalWrite(leda, 0);    // arriba
  digitalWrite(ledb, 0);    // arribaizq
  digitalWrite(ledc, 0);    // abajoizq
  digitalWrite(ledd, 0);    // arribader
  digitalWrite(lede, 0);    // centro
  digitalWrite(ledf, 0);    // abajoder
  digitalWrite(ledg, 0);    // abajo
  delay(1000);
  digitalWrite(leda, 1);    // arriba
  digitalWrite(ledb, 1);    // arribaizq 
  digitalWrite(ledc, 1);    // abajoizq
  digitalWrite(ledd, 1);    // arribader
  digitalWrite(lede, 1);    // centro 
  digitalWrite(ledf, 1);    // abajoder 
  digitalWrite(ledg, 1);    // abajo
  delay(1000);
  */
}

void showNumber(int number)
{
  //int ledNum[][10] = {{0,0,0,0,1,0,0},{1,1,1,0,0,1,1},{0,1,0,0,0,1,0},{0,1,1,0,1,0,0},{1,0,1,0,0,0,1},{0,0,1,1,0,0,0},{0,0,0,1,0,0,0},{0,1,1,0,1,0,1},{0,0,0,0,0,0,0},{0,0,1,0,0,0,1}};
  //int ledNumber[] = ledNum[number];
  
  int ledNum[7];
  
  Serial.println(number);
  if(number == 0)
  {
    ledNum[0] = 0;
    ledNum[1] = 0;
    ledNum[2] = 0;
    ledNum[3] = 0;
    ledNum[4] = 1;
    ledNum[5] = 0;
    ledNum[6] = 0;
  }else if(number == 1)
  {
    //ledNum[] = {1,1,1,0,0,1,1};
    ledNum[0] = 1;
    ledNum[1] = 1;
    ledNum[2] = 1;
    ledNum[3] = 0;
    ledNum[4] = 1;
    ledNum[5] = 0;
    ledNum[6] = 1;
  }else if(number == 2)
  {
    //ledNum[] = {0,1,0,0,0,1,0};
    ledNum[0] = 0;
    ledNum[1] = 1;
    ledNum[2] = 0;
    ledNum[3] = 0;
    ledNum[4] = 0;
    ledNum[5] = 1;
    ledNum[6] = 0;
  }else if(number == 3)
  {
    //ledNum[7] = {0,1,1,0,1,0,0};
    ledNum[0] = 0;
    ledNum[1] = 1;
    ledNum[2] = 1;
    ledNum[3] = 0;
    ledNum[4] = 0;
    ledNum[5] = 0;
    ledNum[6] = 0;
  }else if(number == 4)
  {
    //ledNum[7] = {1,0,1,0,0,0,1};
    ledNum[0] = 1;
    ledNum[1] = 0;
    ledNum[2] = 1;
    ledNum[3] = 0;
    ledNum[4] = 0;
    ledNum[5] = 0;
    ledNum[6] = 1;
  }else if(number == 5)
  {
    //ledNum[7] = {0,0,1,1,0,0,0};
    ledNum[0] = 0;
    ledNum[1] = 0;
    ledNum[2] = 1;
    ledNum[3] = 1;
    ledNum[4] = 0;
    ledNum[5] = 0;
    ledNum[6] = 0;
  }else if(number == 6)
  {
     //ledNum[7] = {0,0,0,1,0,0,0};
    ledNum[0] = 0;
    ledNum[1] = 0;
    ledNum[2] = 0;
    ledNum[3] = 1;
    ledNum[4] = 0;
    ledNum[5] = 0;
    ledNum[6] = 0;
  }else if(number == 7)
  {
    //ledNum[7] = {0,1,1,0,1,0,1};
    ledNum[0] = 0;
    ledNum[1] = 1;
    ledNum[2] = 1;
    ledNum[3] = 0;
    ledNum[4] = 1;
    ledNum[5] = 0;
    ledNum[6] = 1;
  }else if(number == 8)
  {
    //ledNum[7] = {0,0,0,0,0,0,0};
    ledNum[0] = 0;
    ledNum[1] = 0;
    ledNum[2] = 0;
    ledNum[3] = 0;
    ledNum[4] = 0;
    ledNum[5] = 0;
    ledNum[6] = 0;
  }else if(number == 9)
  {
    //ledNum[7] = {0,0,1,0,0,0,1};
    ledNum[0] = 0;
    ledNum[1] = 0;
    ledNum[2] = 1;
    ledNum[3] = 0;
    ledNum[4] = 0;
    ledNum[5] = 0;
    ledNum[6] = 1;
  }
  
  
  for(int led = 12,pos = 0;led>=6;led--,pos++)
  {
    digitalWrite(led, ledNum[pos]);
  }
}
