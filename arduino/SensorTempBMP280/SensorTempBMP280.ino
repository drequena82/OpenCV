/*################## modulo BMP280 ########################
* Filename: BMP280_Ej1.ino
* Descripción: Modulo Presion y Temperatura
* Autor: Jose Mª Morales
* Revisión: 3-03-2017
* Probado: ARDUINO UNO r3 - IDE 1.8.2 (Windows7)
* Web: www.playbyte.es/electronica/
* Licencia: Creative Commons Share-Alike 3.0
* http://creativecommons.org/licenses/by-sa/3.0/deed.es_ES
* ##############################################################
*/

#include <Adafruit_Sensor.h>
#include "Adafruit_BMP280.h"


Adafruit_BMP280 bmp; // I2C


float presion; // Almacena la presion atmosferica (Pa)
float temperatura; // Almacena la temperatura (oC)
int altitud; // Almacena la altitud (m) (se puede usar variable float)


void setup() {

 bmp.begin(); // Inicia el sensor
 Serial.begin(9600); // Inicia comunicacion serie
 Serial.println("BMP280 Sensor de Presion y Temperatura");
}


void loop() {
 
 // Lee valores del sensor:
 presion = bmp.readPressure()/100;
 temperatura = bmp.readTemperature();
 altitud = bmp.readAltitude (1015); // Ajustar con el valor local
 
 // Imprime valores por el serial monitor:
 Serial.print(F("Presion: "));
 Serial.print(presion);
 Serial.print(" hPa");
 Serial.print("\t");
 Serial.print(("Temp: "));
 Serial.print(temperatura);
 Serial.print(" *C");
 Serial.print("\t");
 Serial.print("Altitud (aprox): ");
 Serial.print(altitud); 
 Serial.println(" m");
 delay(1000);
}
