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

sensors_event_t giro;

const int trigger = 2;   //Pin digital 2 para el Trigger del sensor
const int echo = 3;   //Pin digital 3 para el Echo del sensor

void setup(void) {

  //Incializacion del sensor de ultrasonido
  pinMode(trigger, OUTPUT); //pin como salida
  pinMode(echo, INPUT);  //pin como entrada
  digitalWrite(trigger, LOW);//Inicializamos el pin con 0
  
  //Inicialización de los servos
  servos.begin();  
  servos.setPWMFreq(60); //Frecuecia PWM de 60Hz o T=16,66ms
  
  Serial.begin(115200);
  while (!Serial){
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }
  
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip.");
    while (1) {
      delay(10);
    }
  }
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  //Posiciones iniciales de los servos (patas estiradas totalmente).
  iniciarMotores();
  
  delay(1000);

  sentado();

  delay(1000);
}

//Actualizar la posicion del servo, en funcion del tipo de servo, tipo 1 o tipo 2 (hay 2 clases de servos)
void setServo(uint8_t n_servo, unsigned int anguloFin,unsigned int type){
  if(type == SERVO_TYPE1){
    setServo(n_servo,anguloFin,POS0_TYPE1,POS180_TYPE1);
  }else{
    setServo(n_servo,anguloFin,POS0_TYPE2,POS180_TYPE2);
  }
}

//Actualiza la posicion del servo
void setServo(unsigned int n_servo,unsigned int anguloFin,unsigned int pos0,unsigned int pos180) {
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
  giro = getRotation();
  
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
  giro = getRotation();
  
  setServo(cola,0,SERVO_TYPE2);
  setServo(pataDelanteraIzq[0],135,SERVO_TYPE1);
  setServo(pataDelanteraIzq[1],0,SERVO_TYPE2);
  
  setServo(pataTraseraIzq[0],90,SERVO_TYPE1);
  setServo(pataTraseraIzq[1],45,SERVO_TYPE2);
  
  setServo(pataDelanteraDer[0],45,SERVO_TYPE1);
  setServo(pataDelanteraDer[1],180,SERVO_TYPE2);
  
  setServo(pataTraseraDer[0],90,SERVO_TYPE1);
  setServo(pataTraseraDer[1],135,SERVO_TYPE2);
}

//Movimiento de los servos 
void plantado(){
  giro = getRotation();
  
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
  giro = getRotation();
  
  setServo(cola,70,SERVO_TYPE2);
  setServo(pataDelanteraIzq[0],60,SERVO_TYPE1);
  setServo(pataDelanteraIzq[1],60,SERVO_TYPE2);
  setServo(pataTraseraIzq[0],60,SERVO_TYPE1);
  setServo(pataTraseraIzq[1],60,SERVO_TYPE2);

  delay(100);
  
  setServo(pataDelanteraDer[0],60,SERVO_TYPE1);
  setServo(pataDelanteraDer[1],60,SERVO_TYPE2);
  setServo(pataTraseraDer[0],60,SERVO_TYPE1);
  setServo(pataTraseraDer[1],60,SERVO_TYPE2);
}

//Para caminar
void marchaDerecha(){
  giro = getRotation();
  
  setServo(cola,110,SERVO_TYPE2);
  setServo(pataDelanteraIzq[0],120,SERVO_TYPE1);
  setServo(pataDelanteraIzq[1],120,SERVO_TYPE2);
  setServo(pataTraseraIzq[0],120,SERVO_TYPE1);
  setServo(pataTraseraIzq[1],120,SERVO_TYPE2);

  delay(100);
  
  setServo(pataDelanteraDer[0],120,SERVO_TYPE1);
  setServo(pataDelanteraDer[1],120,SERVO_TYPE2);
  setServo(pataTraseraDer[0],120,SERVO_TYPE1);
  setServo(pataTraseraDer[1],120,SERVO_TYPE2);
}

//Funcion que recoge el objeto de posiciones del giroscopio
sensors_event_t getRotation(){
   /* Take a new reading */
  mpu.read();

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");
  
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

long getDistancia(){
  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(trigger, LOW);
  
  t = pulseIn(echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm
  
  Serial.print("Distancia: ");
  Serial.print(d);      //Enviamos serialmente el valor de la distancia
  Serial.print("cm");
  Serial.println();
  delay(100);          //Hacemos una pausa de 100ms
}

void loop() {
  //Va a caminar hasta que llegue a 15 cm de un objeto.
  for(int i=0;i<10;i++){
    if(getDistancia() > 15){
      marchaIzquierda();
      delay(500);
      marchaDerecha();
      delay(500);
    }else{
      plantado();
    }
  }

  plantado();
  
  delay(2000);

}
