
#define MAX 1023

const unsigned int pinCentro = 10;
const unsigned int anillo[] = {6};
int unsigned valorAnillo[] = {0};

void setup() {
  Serial.begin(115200);
  
  for(int index=0;index < 5;index++){
    pinMode(OUTPUT,anillo[index]);
    analogWrite(anillo[index],0);
  }

  pinMode(OUTPUT,pinCentro);
  analogWrite(pinCentro,LOW);
}

void loop() {
  if(valorAnillo[0] >= MAX){
    analogWrite(pinCentro,MAX);
  }
    for(int index=0;index < sizeof(anillo);index++){
      if(valorAnillo[index] < MAX){
        valorAnillo[index] = valorAnillo[index] + 8;
        analogWrite(anillo[index],getLevel(valorAnillo[index]));
        delay(50);
      }
    }

    if(millis()%36598 == 0){
      analogWrite(pinCentro,0);
      for(int index=0;index < sizeof(anillo);index++){
        valorAnillo[index] = 0;
      }

      delay(1000 );
    }
}

int getLevel(unsigned int level){
  return map(level,0,MAX,0,255);
}
