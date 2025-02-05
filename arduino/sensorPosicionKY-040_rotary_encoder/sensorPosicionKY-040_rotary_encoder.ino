/*
  Capitulo 22 de Arduino desde cero en Espa�ol.
  Programa que demuestra el funcionamiento del codificador rotatorio KY-040
        utilizando interrupciones y permitiendo incrementar o decrementar un valor
        inicial dependiendo del sentido de giro del mismo y estableciendo un limite
        minimo y maximo que puede asumir la variable POSICION.

  Autor: bitwiseAr  

*/

int A = 2;      //variable A a pin digital 2 (DT en modulo)
int B = 4;        //variable B a pin digital 4 (CLK en modulo)

int ANTERIOR = 50;    // almacena valor anterior de la variable POSICION

volatile int POSICION = 50; // variable POSICION con valor inicial de 50 y definida
        // como global al ser usada en loop e ISR (encoder)
void setup() {
  pinMode(A, INPUT);    // A como entrada
  pinMode(B, INPUT);    // B como entrada

  Serial.begin(9600);   // incializacion de comunicacion serie a 9600 bps

  attachInterrupt(digitalPinToInterrupt(A), encoder, LOW);// interrupcion sobre pin A con
                // funcion ISR encoder y modo LOW
  Serial.println("Listo");  // imprime en monitor serial Listo
}


void loop() {
  if (POSICION != ANTERIOR) { // si el valor de POSICION es distinto de ANTERIOR
    Serial.println(POSICION); // imprime valor de POSICION
    ANTERIOR = POSICION ; // asigna a ANTERIOR el valor actualizado de POSICION
  }
}

void encoder()  {
  static unsigned long ultimaInterrupcion = 0;  // variable static con ultimo valor de
            // tiempo de interrupcion
  unsigned long tiempoInterrupcion = millis();  // variable almacena valor de func. millis

  if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima
              // pulsos menores a 5 mseg.
    if (digitalRead(B) == HIGH)     // si B es HIGH, sentido horario
    {
      POSICION++ ;        // incrementa POSICION en 1
    }
    else {          // si B es LOW, senti anti horario
      POSICION-- ;        // decrementa POSICION en 1
    }

    POSICION = min(100, max(0, POSICION));  // establece limite inferior de 0 y
            // superior de 100 para POSICION
    ultimaInterrupcion = tiempoInterrupcion;  // guarda valor actualizado del tiempo
  }           // de la interrupcion en variable static
}
