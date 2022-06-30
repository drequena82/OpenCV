/*************************************************** 
  This is an example for our Adafruit 16-channel PWM & Servo driver
  Servo test - this will drive 8 servos, one after the other on the
  first 8 pins of the PCA9685

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815
  
  These drivers use I2C to communicate, 2 pins are required to  
  interface.

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
// you can also call it with a different address and I2C interface
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  580 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// our servo # counter
uint8_t servonum = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("8 channel Servo test!");

  pwm.begin();
  // In theory the internal oscillator is 25MHz but it really isn't
  // that precise. You can 'calibrate' by tweaking this number till
  // you get the frequency you're expecting!
  //pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(60);  // Analog servos run at ~50 Hz updates

  delay(10);
}

// You can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. It's not precise!
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= SERVO_FREQ;   // Analog servos run at ~60 Hz updates
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

void setServo(int n_servo, int anguloFin,int pos0,int pos180) {
  int dutyFin;

  Serial.print("Angulo: ");
  Serial.println(anguloFin);

  Serial.print("Min: ");
  Serial.println(pos0);

  Serial.print("Max: ");
  Serial.println(pos180);
  
  dutyFin=map(anguloFin,0,180,pos0, pos180);

  Serial.print("valor servo: ");
  Serial.println(dutyFin);
  
  pwm.setPWM(n_servo, 0, dutyFin);  
}

void loop() {

  // Drive each servo one at a time using setPWM()
  Serial.println(servonum);

  setServo(0,0,SERVOMIN,SERVOMAX);

  delay(500);

  setServo(0,45,SERVOMIN,SERVOMAX);

  delay(500);

  setServo(0,90,SERVOMIN,SERVOMAX);

  delay(500);

  setServo(0,135,SERVOMIN,SERVOMAX);

  delay(500);

  setServo(0,180,SERVOMIN,SERVOMAX);

  delay(500);

  setServo(0,135,SERVOMIN,SERVOMAX);

  delay(500);

  setServo(0,90,SERVOMIN,SERVOMAX);

  delay(500);

  setServo(0,45,SERVOMIN,SERVOMAX);

  delay(500);

  setServo(0,0,SERVOMIN,SERVOMAX);

  delay(1500);
}
