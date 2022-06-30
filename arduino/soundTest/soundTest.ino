const int pinBuzzer = 3;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  pinMode(A0,INPUT);
  pinMode(pinBuzzer,OUTPUT);

}

void loop() {
  int sound = analogRead(A0);
  Serial.print("Sound: ");
  Serial.println(sound);
  if(sound > 550){
    //tone(pinBuzzer,440,1000);
    playSound();
    delay(1000);
  }
  delay(500);
}

void playSound(){
    switch (random(1,7)) {
        case 1:phrase1(); break;
        case 2:phrase2(); break;
        case 3:phrase1(); phrase2(); break;
        case 4:phrase1(); phrase2(); phrase1();break;
        case 5:phrase1(); phrase2(); phrase1(); phrase2(); phrase1();break;
        case 6:phrase2(); phrase1(); phrase2(); break;
    }
}

void phrase1() {
    
    int k = random(1000,2000);
   
    for (int i = 0; i <=  random(100,2000); i++){
        
        tone(pinBuzzer, k+(-i*2));          
        delay(random(.9,2));             
    } 
    
    for (int i = 0; i <= random(100,1000); i++){
        
        tone(pinBuzzer, k + (i * 10));          
        delay(random(.9,2));             
    }
}
void phrase2() {
    
    int k = random(1000,2000); 
    for (int i = 0; i <= random(100,2000); i++){
        
        tone(pinBuzzer, k+(i*2));          
        delay(random(.9,2));             
    }   
    for (int i = 0; i <= random(100,1000); i++){
        
        tone(pinBuzzer, k + (-i * 10));          
        delay(random(.9,2));             
    } 
}
