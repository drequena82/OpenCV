#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

/*
 *  Para la posición 0° el pulso es de 0.6ms, para 90° es de 1.5ms y para 180° 2.4ms.
 */
#define POS0_TYPE1 125// ancho de pulso en cuentas para pocicion 0°
#define POS180_TYPE1 600// ancho de pulso en cuentas para la pocicion 180°
#define POS0_TYPE2 150
#define POS180_TYPE2 580
#define SERVO_TYPE1 1
#define SERVO_TYPE2 2

Adafruit_PWMServoDriver servos = Adafruit_PWMServoDriver(0x40);
Adafruit_MPU6050 mpu;

unsigned int pataDelanteraIzq[] = {0,1}; 
unsigned int pataDelanteraDer[] = {2,3};

unsigned int pataTraseraIzq[] = {4,5};
unsigned int pataTraseraDer[] = {6,7};

unsigned int cuello[] = {14,15};
unsigned int cola = 13;

void setup(void) {

  //Inicialización de los servos
  servos.begin();  
  servos.setPWMFreq(60); //Frecuecia PWM de 60Hz o T=16,66ms
  
  Serial.begin(115200);
  while (!Serial){
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }
  
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }

  Serial.println("");

  iniciarMotores();
  
  delay(100);
}

//Actualizar la posicion del servo, en funcion del tipo de servo, tipo 1 o tipo 2 (hay 2 clases de servos)
void setServo(uint8_t n_servo, int anguloFin, int type){
  if(type == SERVO_TYPE1){
    setServo(n_servo,anguloFin,POS0_TYPE1,POS180_TYPE1);
  }else{
    setServo(n_servo,anguloFin,POS0_TYPE2,POS180_TYPE2);
  }
}

//Actualiza la posicion del servo
void setServo(int n_servo, int anguloFin,int pos0,int pos180) {
  int dutyFin;
  dutyFin=map(anguloFin,0,180,pos0, pos180);
  
  servos.setPWM(n_servo, 0, dutyFin);  
}

/*
cola                      <- cola a la izquierda 0 grados, centro 90 grados, derecha 180 grados
pata delantera izquierda  <- posicion inicial 0 grados
pata trasera izquierda    <- posicion inicial 180 grados
pata delantera derecha    <- posicion inicial 180 grados
pata trasera derecha      <- posicion inicial 0 grados
*/


//Inicializa los servos a la posicion 0
void iniciarMotores(){
  setServo(cola,0,SERVO_TYPE2);
  setServo(pataDelanteraIzq[0],0,SERVO_TYPE1);
  setServo(pataDelanteraIzq[1],0,SERVO_TYPE2);
  setServo(pataTraseraIzq[0],180,SERVO_TYPE1);
  setServo(pataTraseraIzq[1],180,SERVO_TYPE2);
  setServo(pataDelanteraDer[0],180,SERVO_TYPE1);
  setServo(pataDelanteraDer[1],180,SERVO_TYPE2);
  setServo(pataTraseraDer[0],0,SERVO_TYPE1);
  setServo(pataTraseraDer[1],0,SERVO_TYPE2);
}

//Sentados
void sentado(){
  setServo(cola,0,SERVO_TYPE2);
  setServo(pataDelanteraIzq[0],135,SERVO_TYPE1);
  setServo(pataDelanteraIzq[1],135,SERVO_TYPE2);
  setServo(pataTraseraIzq[0],45,SERVO_TYPE1);
  setServo(pataTraseraIzq[1],45,SERVO_TYPE2);
  setServo(pataDelanteraDer[0],45,SERVO_TYPE1);
  setServo(pataDelanteraDer[1],45,SERVO_TYPE2);
  setServo(pataTraseraDer[0],135,SERVO_TYPE1);
  setServo(pataTraseraDer[1],135,SERVO_TYPE2);
}

//Movimiento de los servos 
void plantado(){
  setServo(cola,90,SERVO_TYPE2);
  setServo(pataDelanteraIzq[0],90,SERVO_TYPE1);
  setServo(pataDelanteraIzq[1],90,SERVO_TYPE2);
  setServo(pataTraseraIzq[0],90,SERVO_TYPE1);
  setServo(pataTraseraIzq[1],90,SERVO_TYPE2);
  setServo(pataDelanteraDer[0],90,SERVO_TYPE1);
  setServo(pataDelanteraDer[1],90,SERVO_TYPE2);
  setServo(pataTraseraDer[0],90,SERVO_TYPE1);
  setServo(pataTraseraDer[1],90,SERVO_TYPE2);
}


//Para caminar
void marchaIzquierda(){
  setServo(cola,70,SERVO_TYPE2);
  setServo(pataDelanteraIzq[0],60,SERVO_TYPE1);
  setServo(pataDelanteraIzq[1],60,SERVO_TYPE2);
  setServo(pataTraseraIzq[0],120,SERVO_TYPE1);
  setServo(pataTraseraIzq[1],120,SERVO_TYPE2);
  setServo(pataDelanteraDer[0],60,SERVO_TYPE1);
  setServo(pataDelanteraDer[1],60,SERVO_TYPE2);
  setServo(pataTraseraDer[0],120,SERVO_TYPE1);
  setServo(pataTraseraDer[1],120,SERVO_TYPE2);
}

//Para caminar
void marchaDerecha(){
  setServo(cola,110,SERVO_TYPE2);
  setServo(pataDelanteraIzq[0],120,SERVO_TYPE1);
  setServo(pataDelanteraIzq[1],120,SERVO_TYPE2);
  setServo(pataTraseraIzq[0],60,SERVO_TYPE1);
  setServo(pataTraseraIzq[1],60,SERVO_TYPE2);
  setServo(pataDelanteraDer[0],120,SERVO_TYPE1);
  setServo(pataDelanteraDer[1],120,SERVO_TYPE2);
  setServo(pataTraseraDer[0],60,SERVO_TYPE1);
  setServo(pataTraseraDer[1],60,SERVO_TYPE2);
}

//Funcion que recoge el objeto de posiciones del giroscopio
sensors_event_t getRotation(){
   /* Take a new reading */
  mpu.read();

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  /*
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");
  */
  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" deg/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

  Serial.println("");

  return g;
}

void loop() {
  //Recogemos el estado de posicion del giroscopio
  sensors_event_t giro = getRotation();
  
  //sentado();

  delay(1500);
  
  plantado();
  
  delay(1500);
}
