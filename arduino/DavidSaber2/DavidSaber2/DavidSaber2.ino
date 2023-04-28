#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <TMRpcm.h>

Adafruit_MPU6050 mpu;
TMRpcm tmrpcm;

#define SD_ChipSelectPin 10

// ---------------------------- SETTINGS -------------------------------
#define BTN_TIMEOUT 800     // button hold delay, ms
#define BRIGHTNESS 255      // max LED brightness (0 - 255)

#define SWING_TIMEOUT 500   // timeout between swings
#define SWING_L_THR 150     // swing angle speed threshold
#define SWING_THR 300       // fast swing angle speed threshold
#define STRIKE_THR 150      // hit acceleration threshold
#define STRIKE_S_THR 320    // hard hit acceleration threshold
#define FLASH_DELAY 80      // flash time while hit

#define PULSE_ALLOW 1       // blade pulsation (1 - allow, 0 - disallow)
#define PULSE_AMPL 20       // pulse amplitude
#define PULSE_DELAY 30      // delay between pulses

#define DEBUG 0             // debug information in Serial (1 - allow, 0 - disallow)
// ---------------------------- SETTINGS -------------------------------

#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 9
#define LED_PIN 2
#define BTN 13
#define IMU_GND A1
#define SD_GND A0

int red = 0;
int green = 0;
int blue = 0;
byte color = 1;

// --------------------------------- SOUNDS ----------------------------------
const char strike1[] PROGMEM = "SK1.wav";
const char strike2[] PROGMEM = "SK2.wav";
const char strike3[] PROGMEM = "SK3.wav";
const char strike4[] PROGMEM = "SK4.wav";
const char strike5[] PROGMEM = "SK5.wav";
const char strike6[] PROGMEM = "SK6.wav";
const char strike7[] PROGMEM = "SK7.wav";
const char strike8[] PROGMEM = "SK8.wav";

const char* const strikes[] PROGMEM  = {
  strike1, strike2, strike3, strike4, strike5, strike6, strike7, strike8
};

int strike_time[8] = {779, 563, 687, 702, 673, 661, 666, 635};

const char strike_s1[] PROGMEM = "SKS1.wav";
const char strike_s2[] PROGMEM = "SKS2.wav";
const char strike_s3[] PROGMEM = "SKS3.wav";
const char strike_s4[] PROGMEM = "SKS4.wav";
const char strike_s5[] PROGMEM = "SKS5.wav";
const char strike_s6[] PROGMEM = "SKS6.wav";
const char strike_s7[] PROGMEM = "SKS7.wav";
const char strike_s8[] PROGMEM = "SKS8.wav";

const char* const strikes_short[] PROGMEM = {
  strike_s1, strike_s2, strike_s3, strike_s4,
  strike_s5, strike_s6, strike_s7, strike_s8
};
int strike_s_time[8] = {270, 167, 186, 250, 252, 255, 250, 238};

const char swing1[] PROGMEM = "SWS1.wav";
const char swing2[] PROGMEM = "SWS2.wav";
const char swing3[] PROGMEM = "SWS3.wav";
const char swing4[] PROGMEM = "SWS4.wav";
const char swing5[] PROGMEM = "SWS5.wav";

const char* const swings[] PROGMEM  = {
  swing1, swing2, swing3, swing4, swing5
};
int swing_time[8] = {389, 372, 360, 366, 337};

const char swingL1[] PROGMEM = "SWL1.wav";
const char swingL2[] PROGMEM = "SWL2.wav";
const char swingL3[] PROGMEM = "SWL3.wav";
const char swingL4[] PROGMEM = "SWL4.wav";

const char* const swings_L[] PROGMEM  = {
  swingL1, swingL2, swingL3, swingL4
};
int swing_time_L[8] = {636, 441, 772, 702};

char BUFFER[10];

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Arrancamos el gyro!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  //setupt motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);

  pinMode(IMU_GND, OUTPUT);
  pinMode(SD_GND, OUTPUT);
  pinMode(BTN_LED, OUTPUT);
  digitalWrite(IMU_GND, 0);
  digitalWrite(SD_GND, 0);
  digitalWrite(BTN_LED, 1);

  randomSeed(analogRead(2));

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  offLed();

  // SD initialization
  tmrpcm.speakerPin = 9;
  tmrpcm.setVolume(5);
  tmrpcm.quality(1);
  if (DEBUG) {
    if (SD.begin(8)){
      Serial.println(F("SD OK"));
    } else {
      Serial.println(F("SD fail"));
    }
  } else {
    SD.begin(8);
  }

  delay(100);
}

void loop() {

  if(mpu.getMotionInterruptStatus()) {
    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    /* Print out the values */
    Serial.print("AccelX:");
    Serial.print(a.acceleration.x);
    Serial.print(",");
    Serial.print("AccelY:");
    Serial.print(a.acceleration.y);
    Serial.print(",");
    Serial.print("AccelZ:");
    Serial.print(a.acceleration.z);
    Serial.print(", ");
    Serial.print("GyroX:");
    Serial.print(g.gyro.x);
    Serial.print(",");
    Serial.print("GyroY:");
    Serial.print(g.gyro.y);
    Serial.print(",");
    Serial.print("GyroZ:");
    Serial.print(g.gyro.z);
    Serial.println("");
  }

  delay(10);
}

void setColor(byte color) {
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

void offLed(){
  analogWrite(RED_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, 0);
}

void fadeLed(int r, int g, int b){
  tmrpcm.play("ON.wav");
  // fade in from min to max in increments of 5 points:
  for (int fadeR = 0, fadeG = 0, fadeB = 0; fadeR <= r || fadeG <= g || fadeB <= b; fadeR += 5, fadeG += 5, fadeB += 5) {
    // sets the value (range from 0 to 255):
    if(fadeR <= r){
      analogWrite(RED_PIN, fadeR);
    }
    
    if(fadeG <= g){
      analogWrite(GREEN_PIN, fadeG);
    }

    if(fadeB <= b){
      analogWrite(BLUE_PIN, fadeB);
    }
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
}

void offLed(int r, int g, int b){
  tmrpcm.play("OFF.wav");
  // fade in from min to max in increments of 5 points:
  for (int fadeR = r, fadeG = g, fadeB = b; fadeR >= 0 || fadeG >= 0 || fadeB >= 0; fadeR -= 5, fadeG -= 5, fadeB -= 5) {
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

void clashLed(int r, int g, int b){
  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);
  delay(100);
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
  delay(100);
  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);
  delay(100);
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}