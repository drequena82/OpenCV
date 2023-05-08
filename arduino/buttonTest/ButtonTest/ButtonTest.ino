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

#define TIMER 2000 

bool ledOn = false;
unsigned int counterButton = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  delay(1000);
}

// the loop function runs over and over again forever
void loop() {
  
  isLedOn();

  if(ledOn){
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  }else{
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  }
  delay(30);
}

bool isLedOn(){
  unsigned int buttonPress = analogRead(A6);
  
  float voltage = buttonPress * (5.0 / 1023.0);
  // > y < (mayor y menor)
  // Mantener pulsado el botón para realizar acciones
  if(voltage > 3){
      Serial.println("BOTON ACTIVO");
    if (counterButton == 0){
      counterButton = millis();    
    }else if(millis() >= counterButton + TIMER){
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