int option;
int led = 13;
 
void setup(){
  Serial.begin(9600);
  pinMode(led, OUTPUT); 
}
 
void loop(){
  //si existe información pendiente
  if (Serial.available()>0){
    //leeemos la opcion
    char option = Serial.read();
    Serial.println(option);
    //si la opcion esta entre '1' y '9'
    if (option >= '1' && option <= '9')
    {
      //restamos el valor '0' ascii (posicion 80) para obtener el numero enviado
      option -= '0';
      //El cast a int se realiza con la posicin del codigo ASCII
      for(int i=0;i<option;i++){
         digitalWrite(led, HIGH);
         delay(100);
         digitalWrite(led, LOW);
         delay(200);
      }
    }
  }
}
