#include "SSD1306Wire.h"
#include <Adafruit_Fingerprint.h>

SoftwareSerial mySerial(D3, D4);

// Use the corresponding display class:

// Initialize the OLED display using SPI
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
// SSD1306Spi        display(D0, D2, D8);
// or
// SH1106Spi         display(D0, D2);

// Initialize the OLED display using brzo_i2c
// D3 -> SDA
// D5 -> SCL
// SSD1306Brzo display(0x3c, D3, D5);
// or
// SH1106Brzo  display(0x3c, D3, D5);

// Initialize the OLED display using Wire library
SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t modePin = 0;
uint8_t id;
uint8_t pressTime = 0;

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  delay(100);
  display.init();
  display.setColor(WHITE);
  display.setContrast(255);
  display.setLogBuffer(5, 30);
  
  printlnDisplay("Finger detect");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    printlnDisplay("Sensor detectado!");
  } else {
    printlnDisplay("Error :(");
    while (1) { delay(1); }
  }

  printlnDisplay(F("Params"));
  finger.getParameters();
  printDisplay(F("Status: 0x")); printlnDisplay( String(finger.status_reg,HEX));
  printDisplay(F("Sys ID: 0x")); printlnDisplay( String(finger.system_id,HEX));
  printDisplay(F("Capacidad: ")); printlnDisplay( String(finger.capacity));
  printDisplay(F("Nivel seg.: ")); printlnDisplay( String(finger.security_level));
  printDisplay(F("Dirección: ")); printlnDisplay( String(finger.device_addr,HEX));
  printDisplay(F("Tamaño paq.: ")); printlnDisplay( String(finger.packet_len));
  printDisplay(F("Ratio tras.: ")); printlnDisplay( String(finger.baud_rate));

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    printDisplay("No hay huellas reg.");
  }
  else {
    printDisplay("Se han registrado "); 
    printDisplay(String(finger.templateCount)); 
    printlnDisplay(" huellas.");
    printlnDisplay("Esperando lectura...");
  }

  pinMode(D5,INPUT);
}

void loop()                     // run over and over again
{
  
  if(digitalRead(D5) == HIGH){
    pressTime = pressTime > 0 ? pressTime : millis();
    
    if((millis() - pressTime) > 3000){
        modePin = modePin == 0 ? 1 : 0;
        pressTime = 0;
    }
  }else{
    pressTime = 0;
  }
  
  if(modePin == 0){
    getFingerprintID();
    delay(50);            //don't ned to run this at full speed.
  }else{
    printlnDisplay("Esperando huella!");
    printlnDisplay("Ingresando el ID...");
    delay(500);
    id = finger.templateCount + 1;
    printDisplay("Guardando ID #");
    printlnDisplay(String(id));
  
    if(!getFingerprintEnroll()){
      modePin = 0;
      pressTime = 0;
      delay(1000);
    }
  }
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      printlnDisplay("Huella tomada!");
      break;
    case FINGERPRINT_NOFINGER:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      printlnDisplay("Error com.!");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      printlnDisplay("Error img!");
      return p;
    default:
      printlnDisplay("Error desc.!");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      printlnDisplay("Huella tomada!");
      break;
    case FINGERPRINT_IMAGEMESS:
      printlnDisplay("Huella movida!");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      printlnDisplay("Error com.");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      printlnDisplay("Tipo huella no encontrada.!");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      printlnDisplay("Huella inválida!");
      return p;
    default:
      printlnDisplay("Error desc.");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    printlnDisplay("Huella encontrada!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    printlnDisplay("Error com.");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    printlnDisplay("Huella no registrada");
    return p;
  } else {
    printlnDisplay("Error desconocido!");
    return p;
  }

  // found a match!
  printDisplay("Encontrado ID #"); 
  printDisplay( String(finger.fingerID));
  printDisplay(" con coincidencia en "); 
  printlnDisplay( String(finger.confidence));

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  printDisplay("Encontrado ID #"); 
  printDisplay( String(finger.fingerID));
  printDisplay(" con coincidencia en "); 
  printlnDisplay( String(finger.confidence));
  return finger.fingerID;
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  printDisplay("Esperando a una lectura de huella válida como #"); 
  printlnDisplay( String(id));
  
  while (p != FINGERPRINT_OK) {;
    if(digitalRead(D5) == HIGH){
    pressTime = pressTime > 0 ? pressTime : millis();
    
    if((millis() - pressTime) > 3000){
        printlnDisplay("Salimos!!");
        pressTime = 0;
        return false;
    }
  }

    
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      printlnDisplay("Huella tomada!!");
      break;
    case FINGERPRINT_NOFINGER:
      printDisplay(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      printlnDisplay("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      printlnDisplay("Imaging error");
      break;
    default:
      printlnDisplay("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      printlnDisplay("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      printlnDisplay("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      printlnDisplay("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      printlnDisplay("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      printlnDisplay("Could not find fingerprint features");
      return p;
    default:
      printlnDisplay("Unknown error");
      return p;
  }

  printlnDisplay("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  printDisplay("ID "); 
  printlnDisplay( String(id));
  p = -1;
  printlnDisplay("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      printlnDisplay("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      printDisplay(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      printlnDisplay("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      printlnDisplay("Imaging error");
      break;
    default:
      printlnDisplay("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      printlnDisplay("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      printlnDisplay("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      printlnDisplay("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      printlnDisplay("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      printlnDisplay("Could not find fingerprint features");
      return p;
    default:
      printlnDisplay("Unknown error");
      return p;
  }

  // OK converted!
  printDisplay("Creating model for #");  
  printlnDisplay( String(id));

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    printlnDisplay("Huella encontrada!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    printlnDisplay("Error en la comunicación");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    printlnDisplay("No hay concidencias encontradas");
    return p;
  } else {
    printlnDisplay("Error desconocido");
    return p;
  }

  printDisplay("ID "); 
  printlnDisplay( String(id));
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    printlnDisplay("Huella guardada!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    printlnDisplay("Error de comunicación");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    printlnDisplay("No se ha podido almacenar la huella");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    printlnDisplay("Error al escribir en memoria");
    return p;
  } else {
    printlnDisplay("Error desconocido");
    return p;
  }

  return true;
}


void printDisplay(String text){
  printDisplay(text,true);
}

void printDisplay(String text,boolean clearDisplay){
  //Escribimos en el display
  if(clearDisplay){
    display.clear();
  }
  display.print(text);
  display.drawLogBuffer(0, 0);
  Serial.print(text);
  display.display();
}

void printlnDisplay(String text){
  printlnDisplay(text,true);
}

void printlnDisplay(String text,boolean clearDisplay){
  //Escribimos en el display
  if(clearDisplay){
    display.clear();
  }
  display.println(text);
  display.drawLogBuffer(0, 0);
  Serial.println(text);
  display.display();
}
