

const int LEDPin = 11;
const int LDRPin = A0;
const int threshold = 100;
 
void setup() {
	pinMode(LEDPin, OUTPUT);
	pinMode(LDRPin, INPUT);

  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  digitalWrite(LEDPin, LOW);

  
  //pinMode(2, INPUT);   // digital sensor is on digital pin 2
  establishContact();  // send a byte to establish contact until receiver responds 
}
 
void loop() {
  int input = 0;
                
  input = analogRead(LDRPin);
  int n = map (input, 0,1024, 0,100);
  
  if (50 <= n) {
      		digitalWrite(LEDPin, HIGH);
                Serial.println(n);
  }
  else {
     	        digitalWrite(LEDPin, LOW);
                Serial.println(n);
	}               
  delay(300);
  
}

void establishContact() {
  /*
  while (Serial.available() <= 0) {
    Serial.println("Puerto serie inicializado: " + Serial.available());   // send an initial string
    delay(300);
  }
  */
  Serial.println("Puerto serie inicializado: " + Serial.available());
  delay(1000);
}


