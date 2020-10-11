/*  Retro Arcade Clock - TechKiwigadgets 
V1 - First Production Release
*/ 
 
#include <UTFT.h> 
#include <URTouch.h> 
#include <Wire.h>  
#include <RTClib.h>

unsigned long timeRefresh = 0;
int p = 0;  // Animation position E.G Pacman Graphic Flag 0 = Closed, 1 = Medium Open, 2 = Wide Open, 3 = Medium Open
int m = 0;  // Animation position mario 3 postions

// Graphics X,Y coordinates

//    myGLCD.drawBitmap (30, 14, 40, 40, rd_ghost); //   Closed Ghost
int ghostX = 15;
int ghostY = 14;
int ghostD = 0; //  direction d == 0 = right, 1 = down, 2 = left, 3 = up
 
//    myGLCD.drawBitmap (140, 14, 40, 40, MarioR3); //   Closed Ghost
int MarioX = 141;
int MarioY = 14;
int MarioD = 0; //  direction d == 0 = right, 1 = down, 2 = left, 3 = up
 
//    myGLCD.drawBitmap (240, 14, 40, 40, Monkey2); //   Closed Ghost
int MonkeyX = 261;
int MonkeyY = 14;
int MonkeyD = 0; //  direction d == 0 = right, 1 = down, 2 = left, 3 = up

    
//    myGLCD.drawBitmap (30, 180, 40, 40, pacman); //   Closed Ghost
int pacmanX = 15;
int pacmanY = 185;
int pacmanD = 2; //  direction d == 0 = right, 1 = down, 2 = left, 3 = up


//    myGLCD.drawBitmap (140, 180, 40, 40, Alien); //   Closed Ghost
int AlienX = 141;
int AlienY = 185;
int AlienD = 2; //  direction d == 0 = right, 1 = down, 2 = left, 3 = up


//    myGLCD.drawBitmap (240, 180, 40, 40, Cannon); //   Closed Ghost   
int CannonX = 270;
int CannonY = 185;
int CannonD = 2; //  direction d == 0 = right, 1 = down, 2 = left, 3 = up

// Display Dimmer Variables
int dimscreen = 255; // This variable is used to drive the screen brightness where 255 is max brightness
int LDR = 100; // LDR variable measured directly from Analog 7



//==== Creating Objects
UTFT    myGLCD(ILI9341_16,38,39,40,41); //Parameters should be adjusted to your Display/Schield model
URTouch  myTouch( 6, 5, 4, 3, 2);
RTC_DS3231 rtc;

//==== Defining Fonts
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t SevenSeg_XXXL_Num[];

// Define bitmaps

extern unsigned int Alien1[0x640]; // Alien 1 graphic
extern unsigned int Alien2[0x640]; // Alien 2 graphic
extern unsigned int Cannon[0x640]; // Space invaders cannon

extern unsigned int MarioL1[0x310]; // M Left 1
extern unsigned int MarioL2[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int MarioL3[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int MarioR1[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int MarioR2[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int MarioR3[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int MarioStop[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int MarioU1[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int MarioU2[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int MarioU3[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int MarioU4[0x310]; // Ghost Bitmap Straight ahead

extern unsigned int rd_ghost[784]; // Ghost Bitmap Straight ahead
extern unsigned int ru_ghost[784]; // Ghost Bitmap Straight ahead
extern unsigned int rl_ghost[784]; // Ghost Bitmap Straight ahead
extern unsigned int rr_ghost[784]; // Ghost Bitmap Straight ahead


extern unsigned int r_o_pacman[784]; // Ghost Bitmap Straight ahead
extern unsigned int r_m_pacman[784]; // Ghost Bitmap Straight ahead
extern unsigned int l_o_pacman[784]; // Ghost Bitmap Straight ahead
extern unsigned int l_m_pacman[784]; // Ghost Bitmap Straight ahead
extern unsigned int u_m_pacman[784]; // Ghost Bitmap Straight ahead
extern unsigned int u_o_pacman[784]; // Ghost Bitmap Straight ahead
extern unsigned int d_m_pacman[784]; // Ghost Bitmap Straight ahead
extern unsigned int d_o_pacman[784]; // Ghost Bitmap Straight ahead
extern unsigned int c_pacman[784]; // Ghost Bitmap Straight ahead


extern unsigned int Monkey2[0x640]; // Ghost Bitmap Straight ahead
extern unsigned int Monkey3[0x640]; // Ghost Bitmap Straight ahead

String daysOfTheWeek[7] = { "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" };
String monthsNames[12] = { "Ene.", "Feb.", "Mar.", "Abr.", "May.",  "Jun.", "Jul.","Ago.","Sept.","Oct.","Nov.","Dic." };
 
int xT,yT;
int userT = 4; // flag to indicate directional touch on screen
boolean setupscreen = false; // used to access the setup screen

//Alarm setup variables
boolean xsetup = false; // Flag to determine if existing setup mode


// Animation delay to slow movement down
int dly = 30; // Orignally 30


// Declare global variables for previous time,  to enable refesh of only digits that have changed
// There are four digits that bneed to be drawn independently to ensure consisitent positioning of time
  int c1 = 20;  // Tens hour digit
  int c2 = 20;  // Ones hour digit
  int c3 = 20;  // Tens minute digit
  int c4 = 20;  // Ones minute digit
  int c5 = 20;  // Tens minute digit
  int c6 = 20;  // Ones minute digit

void setup() {

  //Initialize RTC
  Serial.begin(9600);

  timeRefresh = millis();
  
  #ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));nd

  // Initiate display
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_HI);
  
 
  drawscreen(); // Initiate the game
  UpdateDisp(); // update value to clock 


}

void loop() {

//Refresca la pantalla cada segundo
Serial.print("refresco: ");
Serial.println(timeRefresh);

if((timeRefresh + 1000) > millis()){
  UpdateDisp();
  timeRefresh = millis();
}

// increment Pacman Graphic Flag 0 = Closed, 1 = Medium Open, 2 = Wide Open
p=p+1; 
if(p==4){
  p=0; // Reset counter to closed
}

// increment Pacman Graphic Flag 0 = Closed, 1 = Medium Open, 2 = Wide Open
m=m+1; 
if(m==3){
  m=0; // Reset counter to closed
}

// Set Screen Brightness
// Check the ambient light and adjust LED brightness to suit Ambient approx 500 dark is below 100
//TODO: Variable de la resistencia fotosensible, ajusta el brillo de la pantalla a la resistencia!!
LDR = analogRead(A7);

/* Test value range of LDR
  myGLCD.setColor(237, 28, 36);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.printNumI(LDR,250,60);
*/

if (LDR >=121){
    dimscreen = 255;
   } 
   
if (LDR <=120)   {  
    dimscreen = 45;
   }    
   
analogWrite(9, dimscreen); // Controls brightness 0 is Dark, Ambient room is approx 25 and 70 is direct sunlight 

// Check if user input to touch screen
// UserT sets direction 0 = right, 1 = down, 2 = left, 3 = up, 4 = no touch input


     myTouch.read();
 if (myTouch.dataAvailable()) {
    xT = myTouch.getX();
    yT = myTouch.getY();        
 
 // **********************************
 // ******* Enter Setup Mode *********
 // **********************************
 
    if (((xT>=120) && (xT<=200) && (yT>=105) && (yT<=140))) { // Call Setup Routine if alarm is not sounding
        xsetup = true;  // Toggle flag
        clocksetup(); // Call Clock Setup Routine 
        //UpdateDisp(); // update value to clock
        timeRefresh = millis();
    }
 }


drawghost(ghostX, ghostY, ghostD, p);  // Increment position and Draw image

// ghost Direction //
//  direction d == 0 = right, 1 = down, 2 = left, 3 = up

if(ghostD == 0){ // Right
  // Increment Counter and test results
  ghostX = ghostX + 3;
  if (ghostX == 270){
    myGLCD.fillRect(ghostX-3, ghostY, ghostX, ghostY+28); // Clear trail off graphic before changing position
    ghostD = 1; // Change direction down 
  } 
} else if(ghostD == 1) { // Down
  // Increment Counter and test results
  ghostY = ghostY + 3;
  if (ghostY == 185){
    myGLCD.fillRect(ghostX+3, ghostY-3, ghostX+36, ghostY); // Clear trail off graphic before changing position 
    ghostD = 2; // Change direction down 
  }  
} else if(ghostD == 2) { // Left
  // Increment Counter and test results
  ghostX = ghostX - 3;
  if (ghostX == 12){
   myGLCD.fillRect(ghostX+28, ghostY, ghostX+31, ghostY+28); // Clear trail off graphic before printing new positi 
    ghostD = 3; // Change direction down 
  }  
} else if(ghostD == 3) { // Up
  // Increment Counter and test results
  ghostY = ghostY - 3;
  if (ghostY == 14){
    myGLCD.fillRect(ghostX, ghostY+29, ghostX+28, ghostY+28); // Clear trail off graphic before printing new position
    ghostD = 0; // Change direction down 
  }
}


drawMonkey(MonkeyX, MonkeyY, MonkeyD, p);  // Increment position and Draw image

// Monkey Direction //
//  direction d == 0 = right, 1 = down, 2 = left, 3 = up

if(MonkeyD == 0){ // Right
  // Increment Counter and test results
  MonkeyX = MonkeyX + 3;
  if (MonkeyX == 270){
    myGLCD.fillRect(MonkeyX-3, MonkeyY, MonkeyX, MonkeyY+40); // Clear trail off graphic before changing direction
    MonkeyD = 1; // Change direction down 
  } 
} else if(MonkeyD == 1) { // Down
  // Increment Counter and test results
  MonkeyY = MonkeyY + 3;
  if (MonkeyY == 185){
   myGLCD.fillRect(MonkeyX+3, MonkeyY-3, MonkeyX+38, MonkeyY); // Clear trail off graphic before printing new position 
    MonkeyD = 2; // Change direction down 
  }  
} else if(MonkeyD == 2) { // Left
  // Increment Counter and test results
  MonkeyX = MonkeyX - 3;
  if (MonkeyX == 12){
   myGLCD.fillRect(MonkeyX+41, MonkeyY+1, MonkeyX+40, MonkeyY+38); // Clear trail off graphic before printing new positi 
    MonkeyD = 3; // Change direction down 
  }  
} else if(MonkeyD == 3) { // Up
  // Increment Counter and test results
  MonkeyY = MonkeyY - 3;
  if (MonkeyY == 14){
    myGLCD.fillRect(MonkeyX, MonkeyY+38, MonkeyX+40, MonkeyY+43); // Clear trail off graphic before printing new position
    MonkeyD = 0; // Change direction down 
  }
}


drawCannon(CannonX, CannonY, CannonD, p);  // Increment position and Draw image

// Cannon Direction //
//  direction d == 0 = right, 1 = down, 2 = left, 3 = up

if(CannonD == 0){ // Right
  // Increment Counter and test results
  CannonX = CannonX + 3;
  if (CannonX == 270){
    myGLCD.fillRect(CannonX-3, CannonY+3, CannonX, CannonY+36); // Clear trail off graphic before changing direction
    CannonD = 1; // Change direction down 
  } 
} else if(CannonD == 1) { // Down
  // Increment Counter and test results
  CannonY = CannonY + 3;
  if (CannonY == 185){
    CannonD = 2; // Change direction down 
  }  
} else if(CannonD == 2) { // Left
  // Increment Counter and test results
  CannonX = CannonX - 3;
  if (CannonX == 12){
    myGLCD.fillRect(CannonX+41, CannonY+3, CannonX+40, CannonY+36); // Clear trail off graphic before printing new positi 
    CannonD = 3; // Change direction down 
  }  
} else if(CannonD == 3) { // Up
  // Increment Counter and test results
  CannonY = CannonY - 3;
  if (CannonY == 14){
    CannonD = 0; // Change direction down 
  }
}


drawpacman(pacmanX, pacmanY, pacmanD, p);  // Increment position and Draw image

// pacman Direction //
//  direction d == 0 = right, 1 = down, 2 = left, 3 = up

if(pacmanD == 0){ // Right
  // Increment Counter and test results
  pacmanX = pacmanX + 3;
  if (pacmanX == 270){
    myGLCD.fillRect(pacmanX-3, pacmanY+3, pacmanX, pacmanY+36); // Clear trail off graphic before changing direction
    pacmanD = 1; // Change direction down 
  } 
} else if(pacmanD == 1) { // Down
  // Increment Counter and test results
  pacmanY = pacmanY + 3;
  if (pacmanY == 185){
    myGLCD.fillRect(pacmanX+3, pacmanY-3, pacmanX+36, pacmanY); // Clear trail off graphic before changing position 
    pacmanD = 2; // Change direction down 
  }  
} else if(pacmanD == 2) { // Left
  // Increment Counter and test results
  pacmanX = pacmanX - 3;
  if (pacmanX == 12){
    myGLCD.fillRect(pacmanX+28, pacmanY, pacmanX+31, pacmanY+28); // Clear trail off graphic before printing new positi 
    pacmanD = 3; // Change direction down 
  }  
} else if(pacmanD == 3) { // Up
  // Increment Counter and test results
  pacmanY = pacmanY - 3;
  if (pacmanY == 14){
    myGLCD.fillRect(pacmanX, pacmanY+29, pacmanX+28, pacmanY+28); // Clear trail off graphic before printing new position
    pacmanD = 0; // Change direction down 
  }
}


drawAlien(AlienX, AlienY, AlienD, p);  // Increment position and Draw image

// Alien Direction //
//  direction d == 0 = right, 1 = down, 2 = left, 3 = up

if(AlienD == 0){ // Right
  // Increment Counter and test results
  AlienX = AlienX + 3;
  if (AlienX == 270){
    myGLCD.fillRect(AlienX-3, AlienY+3, AlienX, AlienY+36); // Clear trail off graphic before changing direction
    AlienD = 1; // Change direction down 
  } 
} else if(AlienD == 1) { // Down
  // Increment Counter and test results
  AlienY = AlienY + 3;
  if (AlienY == 185){
    AlienD = 2; // Change direction down 
  }  
} else if(AlienD == 2) { // Left
  // Increment Counter and test results
  AlienX = AlienX - 3;
  if (AlienX == 12){
    myGLCD.fillRect(AlienX+41, AlienY+3, AlienX+40, AlienY+36); // Clear trail off graphic before printing new positi 
    AlienD = 3; // Change direction down 
  }  
} else if(AlienD == 3) { // Up
  // Increment Counter and test results
  AlienY = AlienY - 3;
  if (AlienY == 14){
    AlienD = 0; // Change direction down 
  }
}


drawMario(MarioX, MarioY, MarioD, p);  // Increment position and Draw image

// Mario Direction //
//  direction d == 0 = right, 1 = down, 2 = left, 3 = up

if(MarioD == 0){ // Right
  // Increment Counter and test results
  MarioX = MarioX + 3;
  if (MarioX == 270){
    myGLCD.fillRect(MarioX-3, MarioY+3, MarioX, MarioY+36); // Clear trail off graphic before changing direction
    MarioD = 1; // Change direction down 
  } 
} else if(MarioD == 1) { // Down
  // Increment Counter and test results
  MarioY = MarioY + 3;
  if (MarioY == 185){
   myGLCD.fillRect(MarioX+3, MarioY-3, MarioX+36, MarioY); // Clear trail off graphic before printing new position 
    MarioD = 2; // Change direction down 
  }  
} else if(MarioD == 2) { // Left
  // Increment Counter and test results
  MarioX = MarioX - 3;
  if (MarioX == 12){
    MarioD = 3; // Change direction down 
  }  
} else if(MarioD == 3) { // Up
  // Increment Counter and test results
  MarioY = MarioY - 3;
  if (MarioY == 14){
    myGLCD.fillRect(MarioX, MarioY+30, MarioX+28, MarioY+28); // Clear trail off graphic before printing new position
    MarioD = 0; // Change direction down 
  }
}
delay(dly); 
}

// ************************************************************************************************************
// ===== Update Digital Clock
// ************************************************************************************************************
 void UpdateDisp(){

  //Objeto que tiene el calendario del ds3231
  DateTime now = rtc.now();
  
// Clear the time area
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(0, 0, 0);

   
  int h; // Hour value in 24 hour format
  int e; // Minute value in minute format
  int s;
  int yearRTC;
  int monthRTC;
  int dayRTC;
  int dayOfWeekRTC;
  
  // There are four digits that need to be drawn independently to ensure consisitent positioning of time
  int d1;  // Tens hour digit
  int d2;  // Ones hour digit
  int d3;  // Tens minute digit
  int d4;  // Ones minute digit
  int d5;
  int d6;

  h =now.hour(); // 24 hour RT clock value
  e = now.minute();
  s = now.second();

  yearRTC = now.year();
  monthRTC = now.month();
  dayRTC = now.day();
  dayOfWeekRTC = now.dayOfTheWeek();

Serial.print("segundos: ");
Serial.println(s);
  
// Calculate minute digit values for time
  d1 = h/10;
  d2 = h % 10;
  
  if ((e >= 10)) {  
    d3 = e/10 ; // calculate Tens minute digit 1,2,3,4,5
    d4 = e - (d3*10);  // calculate Ones minute digit 0,1,2
  } else {
    // e is less than 10
    d3 = 0;
    d4 = e;
  }

  d5 = s/10;
  d6 = s % 10;

// *************************************************************************
// Print each digit if it has changed to reduce screen impact/flicker

// Set digit font colour to white

  myGLCD.setColor(0, 255, 0);
 
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(SevenSegNumFont);
  
// First Digit
if(((d1 != c1)||xsetup)&&(d1 != 0)){ // Do not print zero in first digit position
    myGLCD.printNumI(d1,53,70); // Printing thisnumber impacts LFH walls so redraw impacted area   

// ---------------- Clear lines on Outside wall
    myGLCD.setColor(0,0,0);
    myGLCD.drawRoundRect(1, 238, 318, 1); 

}

//If prevous time 12:59 or 00:59 and change in time then blank First Digit

if((c1 == 1) && (c2 == 2) && (c3 == 5) && (c4 == 9) && (d2 != c2) ){ // Clear the previouis First Digit and redraw wall

    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(50, 70, 70, 165);


}

if((c1 == 0) && (c2 == 0) && (c3 == 5) && (c4 == 9) && (d2 != c2) ){ // Clear the previouis First Digit and redraw wall

    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(50, 70, 70, 165);


}

  myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(SevenSegNumFont);
  
// Second Digit
if((d2 != c2)||xsetup){
  myGLCD.printNumI(d2,90,70); // Print 0
}

//Dia de la semana
//myGLCD.print(daysOfTheWeek[dayOfWeek - 1],90,90);


// Third Digit
if((d3 != c3)||xsetup){
  myGLCD.printNumI(d3,150,70); // Was 145    
}

// Fourth Digit
if((d4 != c4)||xsetup){
  myGLCD.printNumI(d4,180,70); // Was 205  
}

//Segundos
myGLCD.setFont(BigFont);
if((d5 != c5)||xsetup){
  myGLCD.printNumI(d5,215,105); // Was 205  
}

if((d6 != c6)||xsetup){
  myGLCD.printNumI(d6,230,105); // Was 205  
}

//Fecha
myGLCD.printNumI(dayRTC,55,150);
//myGLCD.print(daysOfTheWeek[dayOfWeek()],100,100);

myGLCD.print(monthsNames[monthRTC - 1],98,150);
myGLCD.printNumI(yearRTC,180,150);

if (xsetup){
  xsetup = false; // Reset Flag now leaving setup mode
  } 

  // Round dots
  myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.fillCircle(137, 87, 2);
  myGLCD.fillCircle(137, 97, 2);





//--------------------- copy exising time digits to global variables so that these can be used to test which digits change in future

c1 = d1;
c2 = d2;
c3 = d3;
c4 = d4;
c5 = d5;
c6 = d6;
}




// ===== initiateGame - Custom Function
void drawscreen() {

     myGLCD.setColor(1, 73, 240);
// ---------------- Outside wall

        myGLCD.drawRoundRect(0, 239, 319, 0); 
        myGLCD.drawRoundRect(2, 237, 317, 2); 


    myGLCD.drawBitmap (ghostX, ghostY, 28, 28, rd_ghost); //   Closed Ghost
    myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioR3); //   Closed Ghost
    myGLCD.drawBitmap (MonkeyX, MonkeyY, 40, 40, Monkey2); //   Closed Ghost
    
    myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, r_o_pacman); //   Closed Ghost
    myGLCD.drawBitmap (AlienX, AlienY, 40, 40, Alien1); //   Closed Ghost
    myGLCD.drawBitmap (CannonX, CannonY, 40, 40, Cannon); //   Closed Ghost    
 }
 


 // **********************************
 // ******* Enter Setup Mode *********
 // **********************************
 // Use up down arrows to change time and alrm settings

String addZero(unsigned int number){
  String result = "";

  if(number < 10){
    result = "0" + String(number);
  }else{
    result = String(number);
  }
  return result;
}

 void clocksetup(){
  DateTime now = rtc.now();
  int timehour = now.hour();  
  int timeminute = now.minute();
  int tyear = now.year();
  int timemonth = now.month();
  int timeday = now.day();

  String sDay = addZero(timeday);
  String sMonth = addZero(timemonth); 
  String sYear = String(tyear);

  
 // Setup Screen
   myGLCD.clrScr();
// ---------------- Outside wall

   myGLCD.setColor(255, 255, 0);
   myGLCD.setBackColor(0, 0, 0);

   myGLCD.drawRoundRect(0, 239, 319, 0); 
   myGLCD.drawRoundRect(2, 237, 317, 2); 

  // Setup buttons
    myGLCD.setFont(BigFont);

    // Time Set buttons
    myGLCD.print("+  +", 135, 38); 
    myGLCD.print("-  -", 135, 82);
    
    myGLCD.drawRoundRect(132, 35, 152, 55); // time hour +
    myGLCD.drawRoundRect(180, 35, 200, 55); // time minute +
    
    myGLCD.drawRoundRect(132, 80, 152, 100); // time hour -
    myGLCD.drawRoundRect(180, 80, 200, 100); // time minute -   

    // Botones para la fecha
    myGLCD.print("+  +  +", 117, 138); 
    myGLCD.print("-  -  -", 117, 182);
    
    //x y esquina superior izq x y esquina inferior derecha
    myGLCD.drawRoundRect(115, 135, 135, 155); // date day +
    myGLCD.drawRoundRect(163, 135, 183, 155); // date month +
    myGLCD.drawRoundRect(211, 135, 231, 155); // date year +
    
    myGLCD.drawRoundRect(115, 180, 135, 200);  // date day -
    myGLCD.drawRoundRect(163, 180, 183, 200); // date month -  
    myGLCD.drawRoundRect(211, 180, 231, 200); // date year -  
    
    myGLCD.print("SAVE", 13, 213);
    myGLCD.print("EXIT", 245, 213);    
    myGLCD.drawRoundRect(10, 210, 80, 230);
    myGLCD.drawRoundRect(243, 210, 310, 230);  

// Get your Ghost on
    myGLCD.drawBitmap (50, 20, 28, 28, rd_ghost); //   Closed Ghost 
    myGLCD.drawBitmap (240, 100, 28, 28, r_o_pacman); //   Closed Ghost 
    myGLCD.drawBitmap (240, 20, 40, 40, Alien1); //   Closed Ghost 

// Begin Loop here

while (xsetup){

// Display Current Time
   
    myGLCD.print("Hora", 5, 60);    

//    myGLCD.printNumI(timehour, 130, 60); 
 if(timehour>=10){ // Annoyingly if number less than 10 doesnt print on RHS and misses zero so need to compensate
      myGLCD.printNumI(timehour, 130, 60);   // If >= 10 just print minute
      } else {
      myGLCD.print("0", 130, 60);
      myGLCD.printNumI(timehour, 146, 60);      
      } 

    myGLCD.print(":", 160, 60);       

 if(timeminute>=10){ // Annoyingly if number less than 10 doesnt print on RHS and misses zero so need to compensate
      myGLCD.printNumI(timeminute, 175, 60);   // If >= 10 just print minute
      } else {
      myGLCD.print("0", 175, 60);
      myGLCD.printNumI(timeminute, 193, 60);      
      } 

//Display Current Alarm Setting
   
    myGLCD.print("Fecha", 5, 160);    


//Ajuste de la fecha 
 
    myGLCD.print(sDay + "-" + sMonth + "-" + sYear, 90, 160);   
    /*
    myGLCD.print("/", 120, 160);
    myGLCD.print(sMonth, 140, 160);
    myGLCD.print("/", 170, 160);       
    myGLCD.printNumI(tyear, 193, 160);      
    */  
      
// Read input to determine if buttons pressed
     myTouch.read();
 if (myTouch.dataAvailable()) {
    xT = myTouch.getX();
    yT = myTouch.getY();        

    // Capture input command from user

    //Guardado de la fecha
    if ((xT>=1) && (xT<=75) && (yT>=200) && (yT<=239)) { // (243, 210, 310, 230)  Exit Button
        rtc.adjust(DateTime(tyear, timemonth, timeday, timehour, timeminute, 0));
        xsetup = false;   
    } 
    else if ((xT>=230) && (xT<=319) && (yT>=200) && (yT<=239)) { // (243, 210, 310, 230)  Exit Button
        xsetup = false; // Exit setupmode   
    } 
    else if ((xT>=130) && (xT<=154) && (yT>=32) && (yT<=57)) { // Time Hour +  (132, 35, 152, 55)
        timehour = timehour + 1; // Increment Hour
        if (timehour == 24) {  // reset hour to 0 hours if 24
           timehour = 0 ;
       
      } 
    } 
    else if ((xT>=130) && (xT<=154) && (yT>=78) && (yT<=102)) { // (132, 80, 152, 100); // time hour -
        timehour = timehour - 1; // Increment Hour
        if (timehour == -1) {  // reset hour to 23 hours if < 0
           timehour = 23 ;
       
      } 
    }
    else if ((xT>=178) && (xT<=202) && (yT>=32) && (yT<=57)) { // Time Minute +  (180, 35, 200, 55)
        timeminute = timeminute + 1; // Increment Hour
        if (timeminute == 60) {  // reset minute to 0 minutes if 60
           timeminute = 0 ;
        }
      } 

    else if ((xT>=178) && (xT<=202) && (yT>=78) && (yT<=102)) { // (180, 80, 200, 100); // time minute - 
        timeminute = timeminute - 1; // Increment Hour
        if (timeminute == -1) {  // reset minute to 0 minutes if 60
           timeminute = 59 ;
        }
      }      

    //Configuración de la fecha
    else if ((xT>=122) && (xT<=146) && (yT>=133) && (yT<=157)) { // (132, 135, 152, 155); // day +
        timeday = timeday + 1; // Increment Hour

        //año bisiesto
        if(tyear % 4 == 0 && (tyear % 400 == 0 || tyear % 100 > 0)){
          if(timemonth == 2 && timeday > 29){
            timeday = 1 ;
          }
        }else{
          if(timemonth == 2 && timeday > 28){
            timeday = 1 ;
          }
        }
        
        if (timeday > diasMes(timemonth)) {
           timeday = 1 ;
        } 

        sDay = addZero(timeday);
    } 
    else if ((xT>=122) && (xT<=146) && (yT>=178) && (yT<=202)) { // (132, 180, 152, 200);  // day -
        timeday = timeday - 1; // Increment Hour

        //año bisiesto
        if(tyear % 4 == 0 && (tyear % 400 == 0 || tyear % 100 > 0)){
          if(timemonth == 2 && timeday < 0){
            timeday = 29 ;
          }
        }else{
          if(timemonth == 2 && timeday < 0){
            timeday = 28 ;
          }
        }

        sDay = addZero(timeday);
    }
    else if ((xT>=163) && (xT<=187) && (yT>=133) && (yT<=157)) { // (180, 135, 200, 155); // month +
        timemonth = timemonth + 1; // Increment Hour
        if (timemonth == 13) {  // reset minute to 0 minutes if 60
           timemonth = 1 ;
        }

        if(timemonth == 2){
          timeday = 1;  
        }
        
        sMonth = addZero(timemonth);
      } 
    else if ((xT>=163) && (xT<=187) && (yT>=178) && (yT<=202)) { // (180, 180, 200, 200); // month -
        timemonth = timemonth - 1; // Increment Hour
        if (timemonth == -1) {  // reset minute to 0 minutes if 60
           timemonth = 12 ;
        }

        if(timemonth == 2){
          timeday = 1;  
        }
        
        sMonth = addZero(timemonth);
      }   
    else if ((xT>=210) && (xT<=234) && (yT>=133) && (yT<=157)) { // (180, 135, 200, 155); // year +
        tyear = tyear + 1; // Increment Hour
        if(tyear > 2050){
          tyear = 1970;
        }
        
        sYear = String(tyear);
        
      } 
    else if ((xT>=210) && (xT<=234) && (yT>=178) && (yT<=202)) { // (180, 180, 200, 200); // year -
        tyear = tyear + -1;

        if(tyear < 1970){
          tyear = 2050;  
        }
        sYear = String(tyear);
      }   
    
      // Should mean changes should scroll if held down
        delay(250);
    }    
    
}   


     //* Clear Screen
      myGLCD.setColor(0, 0, 0); 
      myGLCD.setBackColor(0, 0, 0);
      myGLCD.fillRect(0,239,319,0);
     xsetup = true; // Set Flag now leaving setup mode in order to draw Clock Digits 
     drawscreen(); // Initiate the screen
     UpdateDisp(); // update value to clock
 
 }


 int diasMes(int mes){
  if(mes == 1 || mes == 3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12){
    return 31; 
  }else{
    return 30;
  }
 }
 // ================= Decimal to BCD converter

byte decToBcd(byte val) {
  return ((val/10*16) + (val%10));
} 


/*
 * Método que pinta un cuadrado con texto dentro
*/
void printSquare(String text,int posX,int posY){
   myGLCD.setFont(BigFont);
   myGLCD.print(text, posX, posY); 
   myGLCD.drawRoundRect(posX - 2, posY - 2, posX + 2 + text.length(), posY + 22);
}

/*
 * Método que detecta la pulsación del botón
*/
int touchSquare(String text,int posX,int posY){
   int pressed = 0;
   myTouch.read();
   if (myTouch.dataAvailable()) {
     xT = myTouch.getX();
     yT = myTouch.getY();

     if((xT>=posX) && (xT<=posX + text.length()) && (yT>=posY) && (yT<=posY + 22)){
       pressed = 1;
     }

     delay(200);
   }
   return pressed;
}

//**********************************************************************************************************
//====== Draws the rd_ghost - bitmap
void drawghost(int x, int y, int d, int p) {

  // Draws the ghost - bitmap
  // knotting direction d == 0 = right, 1 = down, 2 = left, 3 = up
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(0, 0, 0);

  if ( d == 0){ // Right

    myGLCD.fillRect(x-3, y, x, y+28); // Clear trail off graphic before printing new position

    // draw image
    myGLCD.drawBitmap (ghostX, ghostY, 28, 28, rr_ghost); 
    
    } else  if ( d == 1){ // Down

   myGLCD.fillRect(x+3, y-3, x+36, y); // Clear trail off graphic before printing new position 

    // draw image
    myGLCD.drawBitmap (ghostX, ghostY, 28, 28, rd_ghost); 
    

   } else  if ( d == 2){ // Left

   myGLCD.fillRect(x+28, y, x+31, y+28); // Clear trail off graphic before printing new positi 

    // draw image
    myGLCD.drawBitmap (ghostX, ghostY, 28, 28, rl_ghost); 
    
   // draw image

   } else  if ( d == 3){ // Up

   myGLCD.fillRect(x, y+29, x+28, y+28); // Clear trail off graphic before printing new position

    // draw image
    myGLCD.drawBitmap (ghostX, ghostY, 28, 28, ru_ghost); 
    

  }

}


//**********************************************************************************************************
//====== Draws the Mario - bitmap
void drawMario(int x, int y, int d, int p) {

  // Draws the Mario - bitmap
  // Mario direction d == 0 = right, 1 = down, 2 = left, 3 = up
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(0, 0, 0);

  if ( d == 0){ // Right

    myGLCD.fillRect(x-3, y+3, x, y+36); // Clear trail off graphic before printing new position

      // draw image
      if (p==0) {    
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioR1); 
         } else if (p==1) {
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioR3);           
        } else if (p==2) {
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioR2);           
        } else if (p==3) {
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioR3);  
        }
 
    } else  if ( d == 1){ // Down

   myGLCD.fillRect(x+3, y-3, x+36, y); // Clear trail off graphic before printing new position 

       // draw image
      if (p==0) {    
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioU1); 
         } else if (p==1) {
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioU2);           
        } else if (p==2) {
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioU1);           
        } else if (p==3) {
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioU2);  
        }
      
   } else  if ( d == 2){ // Left

   myGLCD.fillRect(x+28, y, x+31, y+28); // Clear trail off graphic before printing new positi 

      // draw image
      if (p==0) {    
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioL1); 
         } else if (p==1) {
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioL3);           
        } else if (p==2) {
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioL2);           
        } else if (p==3) {
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioL3);  
        }

   } else  if ( d == 3){ // Up

   myGLCD.fillRect(x, y+30, x+28, y+28); // Clear trail off graphic before printing new position

       // draw image
      if (p==0) {    
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioU1); 
         } else if (p==1) {
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioU2);           
        } else if (p==2) {
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioU1);           
        } else if (p==3) {
        myGLCD.drawBitmap (MarioX, MarioY, 28, 28, MarioU2);
        }
  }
//  myGLCD.drawBitmap (MarioX, MarioY, 40, 40, MarioR3); 
}


//**********************************************************************************************************
//====== Draws the Cannon - bitmap
void drawCannon(int x, int y, int d, int p) {

  // Draws the Cannon - bitmap
  // Cannon direction d == 0 = right, 1 = down, 2 = left, 3 = up
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(0, 0, 0);



  if ( d == 0){ // Right

    myGLCD.fillRect(x-3, y+3, x, y+36); // Clear trail off graphic before printing new position

    // draw image
 
    } else  if ( d == 1){ // Down

   myGLCD.fillRect(x+3, y-3, x+36, y); // Clear trail off graphic before printing new position 

   // draw image

   } else  if ( d == 2){ // Left

   myGLCD.fillRect(x+41, y+3, x+40, y+36); // Clear trail off graphic before printing new positi 

   // draw image

   } else  if ( d == 3){ // Up

   myGLCD.fillRect(x, y+41, x+40, y+40); // Clear trail off graphic before printing new position

   // draw image

  }
  
 
  myGLCD.drawBitmap (CannonX, CannonY, 40, 40, Cannon); 
}


//**********************************************************************************************************
//====== Draws the Monkey - bitmap
void drawMonkey(int x, int y, int d, int p) {

  // Draws the Monkey - bitmap
  // Monkey direction d == 0 = right, 1 = down, 2 = left, 3 = up
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(0, 0, 0);


  if ( d == 0){ // Right

    myGLCD.fillRect(x-3, y, x, y+40); // Clear trail off graphic before printing new position

 
 
      // draw image
      if (p==0) {    
   myGLCD.drawBitmap (MonkeyX, MonkeyY, 40, 40, Monkey2); 
         } else if (p==1) {
   myGLCD.drawBitmap (MonkeyX, MonkeyY, 40, 40, Monkey2);           
        } else if (p==2) {
   myGLCD.drawBitmap (MonkeyX, MonkeyY, 40, 40, Monkey3);            
        }else {
   myGLCD.drawBitmap (MonkeyX, MonkeyY, 40, 40, Monkey3);      
        }   
   
 
    } else  if ( d == 1){ // Down

   myGLCD.fillRect(x, y-3, x+40, y); // Clear trail off graphic before printing new position 

   // draw image
   myGLCD.drawBitmap (MonkeyX, MonkeyY, 40, 40, Monkey3); 

   } else  if ( d == 2){ // Left

   myGLCD.fillRect(x+41, y, x+40, y+40); // Clear trail off graphic before printing new positi 

      // draw image
      if (p==0) {    
   myGLCD.drawBitmap (MonkeyX, MonkeyY, 40, 40, Monkey2); 
         } else if (p==1) {
   myGLCD.drawBitmap (MonkeyX, MonkeyY, 40, 40, Monkey2);           
        } else if (p==2) {
   myGLCD.drawBitmap (MonkeyX, MonkeyY, 40, 40, Monkey3);            
        }else {
   myGLCD.drawBitmap (MonkeyX, MonkeyY, 40, 40, Monkey3);      
        } 


   } else  if ( d == 3){ // Up

   myGLCD.fillRect(x, y+43, x+40, y+40); // Clear trail off graphic before printing new position

   // draw image
   myGLCD.drawBitmap (MonkeyX, MonkeyY, 40, 40, Monkey3); 

  }
  
  
  

}


//**********************************************************************************************************
//====== Draws the pacman - bitmap
void drawpacman(int x, int y, int d, int p) {

  // Draws the pacman - bitmap
  // pacman direction d == 0 = right, 1 = down, 2 = left, 3 = up
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(0, 0, 0);
  


  if ( d == 0){ // Right

    myGLCD.fillRect(x-3, y+3, x, y+36); // Clear trail off graphic before printing new position

      // draw image
      if (p==0) {    
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, c_pacman); 
         } else if (p==1) {
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, r_m_pacman);           
        } else if (p==2) {
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, r_o_pacman);           
        }else {
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, r_m_pacman);           
        }
        
        
   } else  if ( d == 1){ // Down

   myGLCD.fillRect(x+3, y-3, x+36, y); // Clear trail off graphic before printing new position 

      // draw image
      if (p==0) {    
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, c_pacman); 
         } else if (p==1) {
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, d_m_pacman);           
        } else if (p==2) {
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, d_o_pacman);           
        }else {
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, d_m_pacman);           
        }

   }   else     if ( d == 2){ // Left

   myGLCD.fillRect(x+28, y, x+31, y+28); // Clear trail off graphic before printing new positi 

      // draw image
      if (p==0) {    
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, c_pacman); 
         } else if (p==1) {
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, l_m_pacman);           
        } else if (p==2) {
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, l_o_pacman);           
        }else {
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, l_m_pacman);           
        }

   }   else  if ( d == 3){ // Up

   myGLCD.fillRect(x, y+29, x+28, y+28); // Clear trail off graphic before printing new position

      // draw image
      if (p==0) {    
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, c_pacman); 
         } else if (p==1) {
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, u_m_pacman);           
        } else if (p==2) {
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, u_o_pacman);           
        }else {
        myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, u_m_pacman);           
        }
  }


//  myGLCD.drawBitmap (pacmanX, pacmanY, 28, 28, r_o_pacman); 
}


//**********************************************************************************************************
//====== Draws the Alien - bitmap
void drawAlien(int x, int y, int d, int p) {

  // Draws the Alien - bitmap
  // Alien direction d == 0 = right, 1 = down, 2 = left, 3 = up
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(0, 0, 0);


  if ( d == 0){ // Right

    myGLCD.fillRect(x-3, y+3, x, y+36); // Clear trail off graphic before printing new position

    // draw image
    if((p == 1)||(p == 3)){    
      myGLCD.drawBitmap (AlienX, AlienY, 40, 40, Alien1); 
      } else {
      myGLCD.drawBitmap (AlienX, AlienY, 40, 40, Alien2);      
      }
 
    } else  if ( d == 1){ // Down

   myGLCD.fillRect(x+3, y-3, x+36, y); // Clear trail off graphic before printing new position 

    // draw image
    if((p == 1)||(p == 3)){    
      myGLCD.drawBitmap (AlienX, AlienY, 40, 40, Alien1); 
      } else {
      myGLCD.drawBitmap (AlienX, AlienY, 40, 40, Alien2);      
      }

   } else  if ( d == 2){ // Left

   myGLCD.fillRect(x+41, y+3, x+40, y+36); // Clear trail off graphic before printing new positi 

    // draw image
    if((p == 1)||(p == 3)){    
      myGLCD.drawBitmap (AlienX, AlienY, 40, 40, Alien1); 
      } else {
      myGLCD.drawBitmap (AlienX, AlienY, 40, 40, Alien2);      
      }

   } else  if ( d == 3){ // Up

   myGLCD.fillRect(x, y+41, x+40, y+40); // Clear trail off graphic before printing new position

    // draw image
    if((p == 1)||(p == 3)){    
      myGLCD.drawBitmap (AlienX, AlienY, 40, 40, Alien1); 
      } else {
      myGLCD.drawBitmap (AlienX, AlienY, 40, 40, Alien2);      
      }

  }
}
