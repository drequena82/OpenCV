 /*  Retro Pac-Man Clock
 Author: @TechKiwiGadgets Date 08/04/2017
  V5 
  - introduction of optional Backlight Dimmer Code
  - fix issues with Scoreboard display
 V6 
  - Introduce TEST button on Alarm screen to enable playing alarm sound
 V7 
 - Fix AM/PM error at midday
 V8
 - Add Ms Pac-Man feature in Setup menu
 V9
 - Fix issue with Ms Pac-Man leaving a trail at corners
 - Randomseed will shuffle the random function using Analog pin 0 
 V10 
 - Finally Retro PacMan Dots added and traditional gameplay- enjoy!!
*/
 
#include <UTFT.h> 
#include <URTouch.h>
#include <EEPROM.h>
#include <Time.h>  
#include <Wire.h>  
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t

// Alarm Variables
boolean alarmstatus = false; // flag where false is off and true is on
boolean soundalarm = false; // Flag to indicate the alarm needs to be initiated
int alarmhour = 0;  // hour of alarm setting
int alarmminute = 0; // Minute of alarm setting
byte ahour; //Byte variable for hour
byte amin; //Byte variable for minute
int actr = 300; // When alarm sounds this is a counter used to reset sound card until screen touched
int act = 0;

boolean mspacman = false;  //  if this is is set to true then play the game as Ms Pac-man

//Dot Array - There are 72 Dots with 4 of them that will turn Ghost Blue!

byte dot[73]; // Where if dot is zero then has been gobbled by Pac-Man



// Initializes RTC time values: 
const int DS1307 = 0x68; // Address of DS1307 see data sheets

// Display Dimmer Variables
int dimscreen = 255; // This variable is used to drive the screen brightness where 255 is max brightness
int LDR = 100; // LDR variable measured directly from Analog 7

//==== Creating Objects

//UTFT myGLCD(ILI9341_16,38,39,40,41); //Parameters should be adjusted to your Display/Schield model
UTFT    myGLCD(SSD1289,38,39,40,41); //Parameters should be adjusted to your Display/Schield model
URTouch  myTouch( 6, 5, 4, 3, 2);


//==== Defining Fonts
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t SevenSeg_XXXL_Num[];

extern unsigned int c_pacman[0x310]; // Ghost Bitmap Straight ahead


extern unsigned int ms_c_pacman_u[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int ms_c_pacman_d[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int ms_c_pacman_l[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int ms_c_pacman_r[0x310]; // Ghost Bitmap Straight ahead

extern unsigned int ms_d_m_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int ms_d_o_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int ms_l_m_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int ms_l_o_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int ms_r_m_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int ms_r_o_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int ms_u_m_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int ms_u_o_pacman[0x310]; // Ghost Bitmap Straight ahead

extern unsigned int d_m_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int d_o_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int l_m_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int l_o_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int r_m_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int r_o_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int u_m_pacman[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int u_o_pacman[0x310]; // Ghost Bitmap Straight ahead



extern unsigned int ru_ghost[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int rd_ghost[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int rl_ghost[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int rr_ghost[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int fruit[0x310]; // Ghost Bitmap Straight ahead
extern unsigned int bluepacman[0x310]; // Ghost Bitmap Straight ahead

// Touch screen coordinates
boolean screenPressed = false;
int xT,yT;
int userT = 4; // flag to indicate directional touch on screen
boolean setupscreen = false; // used to access the setup screen

// Fruit flags
boolean fruitgone = false;
boolean fruitdrawn = false;
boolean fruiteatenpacman = false;

//Pacman & Ghost kill flags
boolean pacmanlost = false;
boolean ghostlost = false;

//Alarm setup variables
boolean xsetup = false; // Flag to determine if existing setup mode

// Scorecard
int pacmanscore = 0;
int ghostscore = 0;

// Animation delay to slow movement down
int dly = 18; // Orignally 30

// Time Refresh counter 
int rfcvalue = 900; // wait this long untiul check time for changes
int rfc = 1;

// Pacman coordinates of top LHS of 28x28 bitmap
int xP = 4;
int yP = 108;
int P = 0;  // Pacman Graphic Flag 0 = Closed, 1 = Medium Open, 2 = Wide Open, 3 = Medium Open
int D = 0;  // Pacman direction 0 = right, 1 = down, 2 = left, 3 = up
int prevD;  // Capture legacy direction to enable adequate blanking of trail
int direct;   //  Random direction variable

// Ghost coordinates of top LHS of 28x28 bitmap
int xG = 288;
int yG = 108;
int GD = 2;  // Ghost direction 0 = right, 1 = down, 2 = left, 3 = up
int prevGD;  // Capture legacy direction to enable adequate blanking of trail
int gdirect;   //  Random direction variable 

// Declare global variables for previous time,  to enable refesh of only digits that have changed
// There are four digits that bneed to be drawn independently to ensure consisitent positioning of time
  int c1 = 20;  // Tens hour digit
  int c2 = 20;  // Ones hour digit
  int c3 = 20;  // Tens minute digit
  int c4 = 20;  // Ones minute digit


void setup() {

// Initialize Dot Array
  for (int dotarray = 1; dotarray < 73; dotarray++) {    
    dot[dotarray] = 1;    
    }
  
//Initialize RTC
    Serial.begin(9600);
  // while (!Serial) ; // wait until Arduino Serial Monitor opens
  delay(200);
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  setSyncInterval(60); // sync the time every 60 seconds (1 minutes)
  if(timeStatus()!= timeSet){ 
     Serial.println("Unable to sync with the RTC");
     RTC.set(1408278800); // set the RTC to Aug 25 2014 9:00 am
     setTime(1408278800);
    }
    else{
     Serial.println("RTC has set the system time");   
    }

// Setup Alarm enable pin to play back sound on the ISD1820 board
   pinMode(8, OUTPUT); // D8 used to toggle sound
   digitalWrite(8,LOW);  // Set to low to turn off sound
   
// Setup Pin 9 of Arduino to LED A control brightness of the backlight. 
// This is done by firstly disconnecting Pin 19 of the TFT screen
// Followed by connecting D9 of Arduino to Pin 19 of TFT via a 47 ohm resistor
   pinMode(9, OUTPUT); // D9 used to PWM voltage to backlight on TFT screen
   analogWrite(9, 255); // Controls brightness 0 is Dark, Ambient room is approx 25 and 70 is direct sunlight
   
// Randomseed will shuffle the random function
randomSeed(analogRead(0));

  // Initiate display
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_LOW);
  
 
  drawscreen(); // Initiate the game
  UpdateDisp(); // update value to clock 


}

void loop() {

// Set Screen Brightness
// Check the ambient light and adjust LED brightness to suit Ambient approx 500 dark is below 100
LDR = analogRead(A7);

/* Test value range of LDR
  myGLCD.setColor(237, 28, 36);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.printNumI(LDR,250,60,3);

if (LDR >=121){
    dimscreen = 255;
   } 
   
if (LDR <=120)   {  
    dimscreen = 20;
   }    
*/

dimscreen = (LDR/4)+5;

if (dimscreen >= 255){
  dimscreen = 255;
  }
analogWrite(9, dimscreen); // Controls brightness 0 is Dark, Ambient room is approx 25 and 70 is direct sunlight 
  
  
//Print scoreboard

if((ghostscore >= 95)||(pacmanscore >= 95)){ // Reset scoreboard if over 95
ghostscore = 0;
pacmanscore = 0;

  for (int dotarray = 1; dotarray < 73; dotarray++) {
    
    dot[dotarray] = 1;
    
    }

// Blank the screen across the digits before redrawing them
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(0, 0, 0);

    myGLCD.fillRect(299, 87, 314, 97); // Blankout ghost score  
    myGLCD.fillRect(7, 87, 22, 97);   // Blankout pacman score

drawscreen(); // Redraw dots  
}
  
  myGLCD.setFont(SmallFont);

// Account for position issue if over or under 10

if (ghostscore >= 10){
  myGLCD.setColor(237, 28, 36);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.printNumI(ghostscore,299,87);
} else {
  myGLCD.setColor(237, 28, 36);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.printNumI(ghostscore,307,87);  // Account for being less than 10
}

if (pacmanscore >= 10){
  myGLCD.setColor(248, 236, 1);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.printNumI(pacmanscore,7,87);  
} else{
  myGLCD.setColor(248, 236, 1);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.printNumI(pacmanscore,15,87);  // Account for being less than 10
} 
  
  // Draw fruit
if ((fruitdrawn == false)&&(fruitgone == false)){ // draw fruit and set flag that fruit present so its not drawn again
    myGLCD.drawBitmap (146, 168, 28, 28, fruit); //   draw fruit 
    fruitdrawn = true;
}  

// Redraw fruit if Ghost eats fruit only if Ghost passesover 172 or 120 on the row 186
if ((fruitdrawn == true)&&(fruitgone == false)&&(xG >= 168)&&(xG <= 170)&&(yG >= 168)&&(yG <= 180)){
      myGLCD.drawBitmap (146, 168, 28, 28, fruit); //   draw fruit 
}

if ((fruitdrawn == true)&&(fruitgone == false)&&(xG == 120)&&(yG == 168)){
      myGLCD.drawBitmap (146, 168, 28, 28, fruit); //   draw fruit 
}

// Award Points if Pacman eats Big Dots
if ((fruitdrawn == true)&&(fruitgone == false)&&(xP == 146)&&(yP == 168)){
  fruitgone = true; // If Pacman eats fruit then fruit disappears  
  pacmanscore = pacmanscore + 5; //Increment pacman score 
}





// Read the current date and time from the RTC and reset board
rfc++;
  if (rfc >= rfcvalue) { // count cycles and print time
    UpdateDisp(); // update value to clock then ...
     fruiteatenpacman =  false; // Turn Ghost red again  
     fruitdrawn = false; // If Pacman eats fruit then fruit disappears
     fruitgone = false;
     // Reset every minute both characters
     pacmanlost = false;
     ghostlost = false;
     dly = 18; // reset delay
     rfc = 0;
     
  }

//=== Check if Alarm needs to be sounded
   if (alarmstatus == true){  
     if ( (alarmhour == hour()) && (alarmminute == minute())) {  // Sound the alarm        
           soundalarm = true;
       }     
   }

//=== Start Alarm Sound - Sound pays for 10 seconds then will restart at 20 second mark

if ((alarmstatus == true)&&(soundalarm==true)){ // Set off a counter and take action to restart sound if screen not touched

    if (act == 0) { // Set off alarm by toggling D8, recorded sound triggered by LOW to HIGH transition
        digitalWrite(8,HIGH); // Set high
        digitalWrite(8,LOW); // Set low
        UpdateDisp(); // update value to clock 
    }
    act = act +1;
   
    if (act == actr) { // Set off alarm by toggling D8, recorded sound triggered by LOW to HIGH transition
        digitalWrite(8,HIGH); // Set high
        digitalWrite(8,LOW); // Set low
        act = 0; // Reset counter hopfully every 20 seconds
    } 

}

// Check if user input to touch screen
// UserT sets direction 0 = right, 1 = down, 2 = left, 3 = up, 4 = no touch input


     myTouch.read();
 if (myTouch.dataAvailable() && !screenPressed) {
    xT = myTouch.getX();
    yT = myTouch.getY();        
    // Capture direction request from user
    if ((xT>=1) && (xT<=80) && (yT>=80) && (yT<=160)) { // Left
        userT = 2; // Request to go left   
    }
    if ((xT>=240) && (xT<=318) && (yT>=80) && (yT<=160)) { // Right
        userT = 0; // Request to go right   
    }
    if ((xT>=110) && (xT<=210) && (yT>=1) && (yT<=80)) { // Up
        userT = 3; // Request to go Up   
    }
     if ((xT>=110) && (xT<=210) && (yT>=160) && (yT<=238)) { // Down
        userT = 1; // Request to go Down   
    }
 
 // **********************************
 // ******* Enter Setup Mode *********
 // **********************************
 
    if (((xT>=120) && (xT<=200) && (yT>=105) && (yT<=140)) &&  (soundalarm !=true)) { // Call Setup Routine if alarm is not sounding
        xsetup = true;  // Toggle flag
        clocksetup(); // Call Clock Setup Routine 
        UpdateDisp(); // update value to clock
        
    } else  // If centre of screen touched while alarm sounding then turn off the sound and reset the alarm to not set 
    
    if (((xT>=120) && (xT<=200) && (yT>=105) && (yT<=140)) && ((alarmstatus == true) && (soundalarm ==true))) {
     
      alarmstatus = false;
      soundalarm = false;
      digitalWrite(8,LOW); // Set low
    } 
    
    
 
    if(pacmanlost == false){ // only apply requested changes if Pacman still alive

       if (userT == 2 && D == 0 ){ // Going Right request to turn Left OK
         D = 2;
         }
       if (userT == 0 && D == 2 ){ // Going Left request to turn Right OK
         D = 0;
         }
       if (userT == 1 && D == 3 ){ // Going Up request to turn Down OK
         D = 1;
         }
       if (userT == 3 && D == 1 ){ // Going Down request to turn Up OK
         D = 3;
         }
      }
         screenPressed = true;
   }
    // Doesn't allow holding the screen / you must tap it
    else if ( !myTouch.dataAvailable() && screenPressed){
      screenPressed = false;
   }

// Pacman Captured
// If pacman captured then pacman dissapears until reset
if ((fruiteatenpacman == false)&&(abs(xG-xP)<=5)&&(abs(yG-yP)<=5)){ 
// firstly blank out Pacman
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(xP, yP, xP+27, yP+27); 

  if (pacmanlost == false){
    ghostscore = ghostscore + 15;  
  }
  pacmanlost = true;
 // Slow down speed of drawing now only one moving charater
  dly = 28;
  }
 
if (pacmanlost == false) { // Only draw pacman if he is still alive


// Draw Pac-Man
drawPacman(xP,yP,P,D,prevD); // Draws Pacman at these coordinates
  

// If Pac-Man is on a dot then print the adjacent dots if they are valid

  myGLCD.setColor(200, 200, 200);
  
// Check Rows

if (yP== 4) {  // if in Row 1 **********************************************************
  if (xP== 4) { // dot 1
     if (dot[2] == 1) {  // Check if dot 2 gobbled already
	  myGLCD.fillCircle(42, 19, 2); // dot 2
     }    
      if (dot[13] == 1) {  // Check if dot 13 gobbled already
  	myGLCD.fillCircle(19, 40, 7); // Big dot 13
     }    

  } else
  if (xP== 28) { // dot 2
     if (dot[1] == 1) {  // Check if dot 1 gobbled already
	  myGLCD.fillCircle(19, 19, 2); // dot 1
     }    
      if (dot[3] == 1) {  // Check if dot 3 gobbled already
  	myGLCD.fillCircle(65, 19, 2); // dot 3
     }    

  } else
  if (xP== 52) { // dot 3
     if (dot[2] == 1) {  // Check if dot 2 gobbled already
	  myGLCD.fillCircle(42, 19, 2); // dot 2
     }    
      if (dot[4] == 1) {  // Check if dot 4 gobbled already
  	myGLCD.fillCircle(88, 19, 2); // dot 4
     } 
      if (dot[14] == 1) {  // Check if dot 14 gobbled already
  	myGLCD.fillCircle(77, 40, 2); // dot 14
     }   
  } else
  if (xP== 74) { // dot 4
     if (dot[3] == 1) {  // Check if dot 3 gobbled already
  	myGLCD.fillCircle(65, 19, 2); // dot 3
     }    
      if (dot[5] == 1) {  // Check if dot 5 gobbled already
  	myGLCD.fillCircle(112, 19, 2); // dot 5
     }   
      if (dot[14] == 1) {  // Check if dot 14 gobbled already
  	myGLCD.fillCircle(77, 40, 2); // dot 14
     }    
  } else
  if (xP== 98) { // dot 5
     if (dot[4] == 1) {  // Check if dot 3 gobbled already
  	myGLCD.fillCircle(88, 19, 2); // dot 4
     }    
      if (dot[6] == 1) {  // Check if dot 5 gobbled already
  	myGLCD.fillCircle(136, 19, 2); // dot 6
     }     
  } else
  if (xP== 120) { // dot 6
     if (dot[5] == 1) {  // Check if dot 5 gobbled already
  	myGLCD.fillCircle(112, 19, 2); // dot 5
     }    
      if (dot[15] == 1) {  // Check if dot 15 gobbled already
  	myGLCD.fillCircle(136, 40, 2); // dot 15
     }     
  } else
 

 if (xP== 168) { // dot 7
      if (dot[16] == 1) {  // Check if dot 16 gobbled already
  	myGLCD.fillCircle(183, 40, 2); // dot 16
     }    
      if (dot[8] == 1) {  // Check if dot 8 gobbled already
  	myGLCD.fillCircle(206, 19, 2); // dot 8
     }     
  } else
  if (xP== 192) { // dot 8
      if (dot[7] == 1) {  // Check if dot 7 gobbled already
  	myGLCD.fillCircle(183, 19, 2); // dot 7
     }    
      if (dot[9] == 1) {  // Check if dot 9 gobbled already
  	myGLCD.fillCircle(229, 19, 2); // dot 9
     }    
  } else
  if (xP== 216) { // dot 9
      if (dot[10] == 1) {  // Check if dot 10 gobbled already
  	myGLCD.fillCircle(252, 19, 2); // dot 10
     }    
      if (dot[8] == 1) {  // Check if dot 8 gobbled already
  	myGLCD.fillCircle(206, 19, 2); // dot 8
     }      
      if (dot[17] == 1) {  // Check if dot 17 gobbled already
  	myGLCD.fillCircle(241, 40, 2); // dot 17
     }   
 } else
  if (xP== 238) { // dot 10
      if (dot[11] == 1) {  // Check if dot 11 gobbled already
  	myGLCD.fillCircle(275, 19, 2); // dot 11
     }    
      if (dot[9] == 1) {  // Check if dot 9 gobbled already
  	myGLCD.fillCircle(229, 19, 2); // dot 9
     }      
      if (dot[17] == 1) {  // Check if dot 17 gobbled already
  	myGLCD.fillCircle(241, 40, 2); // dot 17
     }   
  } else
  if (xP== 262) { // dot 11
      if (dot[10] == 1) {  // Check if dot 10 gobbled already
  	myGLCD.fillCircle(252, 19, 2); // dot 10
     }    
      if (dot[12] == 1) {  // Check if dot 12 gobbled already
  	myGLCD.fillCircle(298, 19, 2); // dot 12
     }    
  } else
  if (xP== 284) { // dot 12
      if (dot[11] == 1) {  // Check if dot 11 gobbled already
  	myGLCD.fillCircle(275, 19, 2); // dot 11
     }    
      if (dot[18] == 1) {  // Check if dot 18 gobbled already
  	myGLCD.fillCircle(298, 40, 7); // dot 18
     }  
  }
} else 
if (yP== 26) {  // if in Row 2  **********************************************************
  if (xP== 4) { // dot 13
     if (dot[1] == 1) {  // Check if dot 1 gobbled already
	  myGLCD.fillCircle(19, 19, 2); // dot 1
     }    
      if (dot[19] == 1) {  // Check if dot 19 gobbled already
  	myGLCD.fillCircle(19, 60, 2); //  dot 19
     }   
  } else
  
    if (xP== 62) { // dot 14
      if (dot[3] == 1) {  // Check if dot 3 gobbled already
  	myGLCD.fillCircle(65, 19, 2); // dot 3
     }   
         if (dot[4] == 1) {  // Check if dot 4 gobbled already
  	myGLCD.fillCircle(88, 19, 2); // dot 4
     } 
         if (dot[21] == 1) {  // Check if dot 21 gobbled already
  	myGLCD.fillCircle(65, 60, 2); // dot 21
     }   
      if (dot[22] == 1) {  // Check if dot 22 gobbled already
  	myGLCD.fillCircle(88, 60, 2); // dot 22
     }    
     
  } else
  
  if (xP== 120) { // dot 15
     if (dot[24] == 1) {  // Check if dot 24 gobbled already
  	myGLCD.fillCircle(136, 60, 2); // dot 24
     }    
      if (dot[6] == 1) {  // Check if dot 6 gobbled already
  	myGLCD.fillCircle(136, 19, 2); // dot 6
     }      
  } else
  if (xP== 168) { // dot 16
      if (dot[7] == 1) {  // Check if dot 7 gobbled already
  	myGLCD.fillCircle(183, 19, 2); // dot 7
     }    
      if (dot[26] == 1) {  // Check if dot 26 gobbled already
  	myGLCD.fillCircle(183, 60, 2); // dot 26
     }          
  } else
    if (xP== 228) { // dot 17
      if (dot[9] == 1) {  // Check if dot 9 gobbled already
  	myGLCD.fillCircle(229, 19, 2); // dot 9
     }      
       if (dot[10] == 1) {  // Check if dot 10 gobbled already
  	myGLCD.fillCircle(252, 19, 2); // dot 10
     }  
      if (dot[28] == 1) {  // Check if dot 28 gobbled already
	  myGLCD.fillCircle(229, 60, 2); // dot 28
     }  
       if (dot[29] == 1) {  // Check if dot 29 gobbled already
  	myGLCD.fillCircle(252, 60, 2); // dot 29
     }     
     
  } else
  if (xP== 284) { // dot 18
      if (dot[31] == 1) {  // Check if dot 31 gobbled already
  	myGLCD.fillCircle(298, 60, 2); // dot 31
     }    
      if (dot[12] == 1) {  // Check if dot 12 gobbled already
  	myGLCD.fillCircle(298, 19, 2); // dot 12
     }  
  }
} else
if (yP== 46) {  // if in Row 3  **********************************************************
  if (xP== 4) { // dot 19
     if (dot[20] == 1) {  // Check if dot 20 gobbled already
	  myGLCD.fillCircle(42, 60, 2); // dot 20
     }    
      if (dot[13] == 1) {  // Check if dot 13 gobbled already
  	myGLCD.fillCircle(19, 40, 7); // Big dot 13
     }  
  } else
  if (xP== 28) { // dot 20
     if (dot[19] == 1) {  // Check if dot 19 gobbled already
  	myGLCD.fillCircle(19, 60, 2); // dot 19
     }    
      if (dot[21] == 1) {  // Check if dot 21 gobbled already
  	myGLCD.fillCircle(65, 60, 2); // dot 21
     }   
      if (dot[32] == 1) {  // Check if dot 32 gobbled already
  	myGLCD.fillCircle(42, 80, 2); // dot 32
     }    
  } else
  if (xP== 52) { // dot 21
     if (dot[20] == 1) {  // Check if dot 20 gobbled already
	  myGLCD.fillCircle(42, 60, 2); // dot 20
     }    
      if (dot[22] == 1) {  // Check if dot 22 gobbled already
  	myGLCD.fillCircle(88, 60, 2); // dot 22
     } 
      if (dot[14] == 1) {  // Check if dot 14 gobbled already
  	myGLCD.fillCircle(77, 40, 2); // dot 14
     }         
  } else
  if (xP== 74) { // dot 22
      if (dot[21] == 1) {  // Check if dot 21 gobbled already
  	myGLCD.fillCircle(65, 60, 2); // dot 21
     }    
      if (dot[23] == 1) {  // Check if dot 23 gobbled already
  	myGLCD.fillCircle(112, 60, 2); // dot 23
     } 
      if (dot[14] == 1) {  // Check if dot 14 gobbled already
  	myGLCD.fillCircle(77, 40, 2); // dot 14
     }    
  } else
  if (xP== 98) { // dot 23
     if (dot[24] == 1) {  // Check if dot 24 gobbled already
	  myGLCD.fillCircle(136, 60, 2); // dot 24
     }    
      if (dot[22] == 1) {  // Check if dot 22 gobbled already
  	myGLCD.fillCircle(88, 60, 2); // dot 22
     }  
    
  } else
  if (xP== 120) { // dot 24
      if (dot[25] == 1) {  // Check if dot 25 gobbled already
  	myGLCD.fillCircle(160, 60, 2); // dot 25
     }    
      if (dot[23] == 1) {  // Check if dot 23 gobbled already
  	myGLCD.fillCircle(112, 60, 2); // dot 23
     }
      if (dot[15] == 1) {  // Check if dot 15 gobbled already
  	myGLCD.fillCircle(136, 40, 2); // dot 15
     }        
  } else
  if (xP== 146) { // dot 25
     if (dot[24] == 1) {  // Check if dot 24 gobbled already
	  myGLCD.fillCircle(136, 60, 2); // dot 24
     }    
      if (dot[26] == 1) {  // Check if dot 26 gobbled already
  	myGLCD.fillCircle(183, 60, 2); // dot 26
     }    
  } else
  if (xP== 168) { // dot 26
      if (dot[25] == 1) {  // Check if dot 25 gobbled already
  	myGLCD.fillCircle(160, 60, 2); // dot 25
     }    
      if (dot[27] == 1) {  // Check if dot 27 gobbled already
  	myGLCD.fillCircle(206, 60, 2); // dot 27
     }
      if (dot[16] == 1) {  // Check if dot 16 gobbled already
  	myGLCD.fillCircle(183, 40, 2); // dot 16
     }    
  } else
  if (xP== 192) { // dot 27
     if (dot[28] == 1) {  // Check if dot 28 gobbled already
	  myGLCD.fillCircle(229, 60, 2); // dot 28
     }    
      if (dot[26] == 1) {  // Check if dot 26 gobbled already
  	myGLCD.fillCircle(183, 60, 2); // dot 26
     }      
  } else
  if (xP== 216) { // dot 28
      if (dot[29] == 1) {  // Check if dot 29 gobbled already
  	myGLCD.fillCircle(252, 60, 2); // dot 29
     }    
      if (dot[27] == 1) {  // Check if dot 27 gobbled already
  	myGLCD.fillCircle(206, 60, 2); // dot 27
     }      
      if (dot[17] == 1) {  // Check if dot 17 gobbled already
  	myGLCD.fillCircle(241, 40, 2); // dot 17
     }   
  } else
  if (xP== 238) { // dot 29
     if (dot[28] == 1) {  // Check if dot 28 gobbled already
	  myGLCD.fillCircle(229, 60, 2); // dot 28
     }    
      if (dot[30] == 1) {  // Check if dot 30 gobbled already
  	myGLCD.fillCircle(275, 60, 2); // dot 30
     }      
      if (dot[17] == 1) {  // Check if dot 17 gobbled already
  	myGLCD.fillCircle(241, 40, 2); // dot 17
     }   
   
  } else
  if (xP== 262) { // dot 30
      if (dot[29] == 1) {  // Check if dot 29 gobbled already
  	myGLCD.fillCircle(252, 60, 2); // dot 29
     }    
      if (dot[33] == 1) {  // Check if dot 33 gobbled already
  	myGLCD.fillCircle(275, 80, 2); // dot 33
     }      
      if (dot[31] == 1) {  // Check if dot 31 gobbled already
  	myGLCD.fillCircle(298, 60, 2); // dot 31
     }  
  
  } else
  if (xP== 284) { // dot 31
   if (dot[18] == 1) {  // Check if dot 18 gobbled already
  	myGLCD.fillCircle(298, 40, 7); // dot 18
   }     
   if (dot[30] == 1) {  // Check if dot 30 gobbled already
  	myGLCD.fillCircle(275, 60, 2); // dot 30
   } 
  }
} else

if (yP== 168) {  // if in Row 4  **********************************************************
  if (xP== 4) { // dot 42
     if (dot[43] == 1) {  // Check if dot 43 gobbled already
	  myGLCD.fillCircle(42, 181, 2); // dot 43
     }     
     if (dot[55] == 1) {  // Check if dot 55 gobbled already
	  myGLCD.fillCircle(19, 201, 7); // dot 55
     }     
  } else
  if (xP== 28) { // dot 43
     if (dot[42] == 1) {  // Check if dot 42 gobbled already
	  myGLCD.fillCircle(19, 181, 2); // dot 42
     }     
     if (dot[44] == 1) {  // Check if dot 44 gobbled already
	  myGLCD.fillCircle(65, 181, 2); // dot 44
     }   
      if (dot[40] == 1) {  // Check if dot 40 gobbled already
  	myGLCD.fillCircle(42, 160, 2); // dot 40
     }       
  } else
  if (xP== 52) { // dot 44
     if (dot[43] == 1) {  // Check if dot 43 gobbled already
	  myGLCD.fillCircle(42, 181, 2); // dot 43
     }     
     if (dot[45] == 1) {  // Check if dot 45 gobbled already
	  myGLCD.fillCircle(88, 181, 2); // dot 45
     } 
     if (dot[56] == 1) {  // Check if dot 56 gobbled already
	  myGLCD.fillCircle(77, 201, 2); // dot 56 
     }    
  } else
  if (xP== 74) { // dot 45
     if (dot[46] == 1) {  // Check if dot 46 gobbled already
	  myGLCD.fillCircle(112, 181, 2); // dot 46
     }     
     if (dot[44] == 1) {  // Check if dot 44 gobbled already
	  myGLCD.fillCircle(65, 181, 2); // dot 44
     } 
     if (dot[56] == 1) {  // Check if dot 56 gobbled already
	  myGLCD.fillCircle(77, 201, 2); // dot 56 
     }    
     
  } else
  if (xP== 98) { // dot 46
     if (dot[47] == 1) {  // Check if dot 47 gobbled already
	  myGLCD.fillCircle(136, 181, 2); // dot 47
     }     
     if (dot[45] == 1) {  // Check if dot 45 gobbled already
	  myGLCD.fillCircle(88, 181, 2); // dot 45
     }  
  } else
  if (xP== 120) { // dot 47
     if (dot[48] == 1) {  // Check if dot 48 gobbled already
	  myGLCD.fillCircle(160, 181, 2); // dot 48
     }     
     if (dot[46] == 1) {  // Check if dot 46 gobbled already
	  myGLCD.fillCircle(112, 181, 2); // dot 46
     } 
     if (dot[57] == 1) {  // Check if dot 57 gobbled already
	  myGLCD.fillCircle(136, 201, 2); // dot 57 
     }      
  } else
  if (xP== 146) { // dot 48
     if (dot[47] == 1) {  // Check if dot 47 gobbled already
	  myGLCD.fillCircle(136, 181, 2); // dot 47
     }     
     if (dot[49] == 1) {  // Check if dot 49 gobbled already
	  myGLCD.fillCircle(183, 181, 2); // dot 49
     }  
  } else

  if (xP== 168) { // dot 49
     if (dot[48] == 1) {  // Check if dot 48 gobbled already
	  myGLCD.fillCircle(160, 181, 2); // dot 48
     }     
     if (dot[50] == 1) {  // Check if dot 50 gobbled already
	  myGLCD.fillCircle(206, 181, 2); // dot 50
     } 
     if (dot[58] == 1) {  // Check if dot 58 gobbled already
	  myGLCD.fillCircle(183, 201, 2); // dot 58
     }        
  } else
  if (xP== 192) { // dot 50
     if (dot[51] == 1) {  // Check if dot 51 gobbled already
	  myGLCD.fillCircle(229, 181, 2); // dot 51
     }     
     if (dot[49] == 1) {  // Check if dot 49 gobbled already
	  myGLCD.fillCircle(183, 181, 2); // dot 49
     }      
  } else
  if (xP== 216) { // dot 51
     if (dot[50] == 1) {  // Check if dot 50 gobbled already
	  myGLCD.fillCircle(206, 181, 2); // dot 50
     }    
     if (dot[52] == 1) {  // Check if dot 52 gobbled already
	  myGLCD.fillCircle(252, 181, 2); // dot 52
     } 
     if (dot[59] == 1) {  // Check if dot 59 gobbled already
	  myGLCD.fillCircle(241, 201, 2); // dot 59
     }     
  } else
  if (xP== 238) { // dot 52
     if (dot[53] == 1) {  // Check if dot 53 gobbled already
	  myGLCD.fillCircle(275, 181, 2); // dot 53
     }    
     if (dot[51] == 1) {  // Check if dot 51 gobbled already
	  myGLCD.fillCircle(229, 181, 2); // dot 51
     }  
     if (dot[59] == 1) {  // Check if dot 59 gobbled already
	  myGLCD.fillCircle(241, 201, 2); // dot 59
     }     
  } else
 




 if (xP== 262) { // dot 53
     if (dot[41] == 1) {  // Check if dot 41 gobbled already
	  myGLCD.fillCircle(275, 160, 2); // dot 41
     }    
     if (dot[52] == 1) {  // Check if dot 52 gobbled already
	  myGLCD.fillCircle(252, 181, 2); // dot 52
     } 
     if (dot[54] == 1) {  // Check if dot 54 gobbled already
	  myGLCD.fillCircle(298, 181, 2); // dot 54
     }     
  } else
  if (xP== 284) { // dot 54
     if (dot[53] == 1) {  // Check if dot 53 gobbled already
	  myGLCD.fillCircle(275, 181, 2); // dot 53
     }    
     if (dot[60] == 1) {  // Check if dot 60 gobbled already
	  myGLCD.fillCircle(298, 201, 7); // Big dot 60
     }      
  } 

} else
if (yP== 188) {  // if in Row 5  **********************************************************
  if (xP== 4) { // dot 55
     if (dot[42] == 1) {  // Check if dot 42 gobbled already
	  myGLCD.fillCircle(19, 181, 2); // dot 42
     } 
     if (dot[61] == 1) {  // Check if dot 61 gobbled already
	  myGLCD.fillCircle(19, 221, 2); // dot 61
     }    
  } else
   if (xP== 62) { // dot 56
     if (dot[44] == 1) {  // Check if dot 44 gobbled already
	  myGLCD.fillCircle(65, 181, 2); // dot 44
     } 
     if (dot[45] == 1) {  // Check if dot 45 gobbled already
	  myGLCD.fillCircle(88, 181, 2); // dot 45
     } 
     if (dot[63] == 1) {  // Check if dot 63 gobbled already
	  myGLCD.fillCircle(65, 221, 2); // dot 63
     }
     if (dot[64] == 1) {  // Check if dot 64 gobbled already
	  myGLCD.fillCircle(88, 221, 2); // dot 64
     }      
     
  } else
  
  if (xP== 120) { // dot 57
     if (dot[47] == 1) {  // Check if dot 47 gobbled already
	  myGLCD.fillCircle(136, 181, 2); // dot 47
     }     
     if (dot[66] == 1) {  // Check if dot 66 gobbled already
	  myGLCD.fillCircle(136, 221, 2); // dot 66
     }    
  } else
  if (xP== 168) { // dot 58
     if (dot[67] == 1) {  // Check if dot 67 gobbled already
	  myGLCD.fillCircle(183, 221, 2); // dot 67
     }     
     if (dot[49] == 1) {  // Check if dot 49 gobbled already
	  myGLCD.fillCircle(183, 181, 2); // dot 49
     }       
  } else
  
  if (xP== 228) { // dot 59
     if (dot[51] == 1) {  // Check if dot 51 gobbled already
	  myGLCD.fillCircle(229, 181, 2); // dot 51
     }
     if (dot[52] == 1) {  // Check if dot 52 gobbled already
	  myGLCD.fillCircle(252, 181, 2); // dot 52
     } 
     if (dot[69] == 1) {  // Check if dot 69 gobbled already
	  myGLCD.fillCircle(229, 221, 2); // dot 69
     } 
     if (dot[70] == 1) {  // Check if dot 70 gobbled already
	  myGLCD.fillCircle(252, 221, 2); // dot 70
     }      
     
  } else
  
  if (xP== 284) { // dot 60
     if (dot[72] == 1) {  // Check if dot 72 gobbled already
	  myGLCD.fillCircle(298, 221, 2); // Big dot 72
     } 
     if (dot[54] == 1) {  // Check if dot 54 gobbled already
	  myGLCD.fillCircle(298, 181, 2); // dot 54
     }    
  } 

} else


if (yP== 208) {  // if in Row 6  **********************************************************
  if (xP== 4) { // dot 61
     if (dot[55] == 1) {  // Check if dot 55 gobbled already
	  myGLCD.fillCircle(19, 201, 7); // dot 55
     } 
     if (dot[62] == 1) {  // Check if dot 62 gobbled already
	  myGLCD.fillCircle(42, 221, 2); // dot 62
     }   
  } else
  if (xP== 28) { // dot 62
     if (dot[61] == 1) {  // Check if dot 61 gobbled already
	  myGLCD.fillCircle(19, 221, 2); // dot 61
     }  
     if (dot[63] == 1) {  // Check if dot 63 gobbled already
	  myGLCD.fillCircle(65, 221, 2); // dot 63
     }      
  } else
  if (xP== 52) { // dot 63
     if (dot[64] == 1) {  // Check if dot 64 gobbled already
	  myGLCD.fillCircle(88, 221, 2); // dot 64
     } 
     if (dot[62] == 1) {  // Check if dot 62 gobbled already
	  myGLCD.fillCircle(42, 221, 2); // dot 62
     }  
     if (dot[56] == 1) {  // Check if dot 56 gobbled already
	  myGLCD.fillCircle(77, 201, 2); // dot 56 
     }      
  } else
  if (xP== 74) { // dot 64
     if (dot[65] == 1) {  // Check if dot 65 gobbled already
	  myGLCD.fillCircle(112, 221, 2); // dot 65
     } 
     if (dot[63] == 1) {  // Check if dot 63 gobbled already
	  myGLCD.fillCircle(65, 221, 2); // dot 63
     }  
     if (dot[56] == 1) {  // Check if dot 56 gobbled already
	  myGLCD.fillCircle(77, 201, 2); // dot 56 
     }     
  } else
  if (xP== 98) { // dot 65
     if (dot[64] == 1) {  // Check if dot 64 gobbled already
	  myGLCD.fillCircle(88, 221, 2); // dot 64
     } 
     if (dot[66] == 1) {  // Check if dot 66 gobbled already
	  myGLCD.fillCircle(136, 221, 2); // dot 66
     }    
  } else
  if (xP== 120) { // dot 66
     if (dot[65] == 1) {  // Check if dot 65 gobbled already
	  myGLCD.fillCircle(112, 221, 2); // dot 65
     } 
     if (dot[57] == 1) {  // Check if dot 57 gobbled already
	  myGLCD.fillCircle(136, 201, 2); // dot 57 
     }    
  } else
  if (xP== 168) { // dot 67
     if (dot[68] == 1) {  // Check if dot 68 gobbled already
	  myGLCD.fillCircle(206, 221, 2); // dot 68
     } 
     if (dot[58] == 1) {  // Check if dot 58 gobbled already
	  myGLCD.fillCircle(183, 201, 2); // dot 58
     }     
  } else
  if (xP== 192) { // dot 68
     if (dot[67] == 1) {  // Check if dot 67 gobbled already
	  myGLCD.fillCircle(183, 221, 2); // dot 67
     } 
     if (dot[69] == 1) {  // Check if dot 69 gobbled already
	  myGLCD.fillCircle(229, 221, 2); // dot 69
     }    
  } else
  if (xP== 216) { // dot 69
     if (dot[68] == 1) {  // Check if dot 68 gobbled already
	  myGLCD.fillCircle(206, 221, 2); // dot 68
     } 
     if (dot[59] == 1) {  // Check if dot 59 gobbled already
	  myGLCD.fillCircle(241, 201, 2); // dot 59
     }
     if (dot[70] == 1) {  // Check if dot 70 gobbled already
	  myGLCD.fillCircle(252, 221, 2); // dot 70
     }    
  } else
  if (xP== 238) { // dot 70
     if (dot[69] == 1) {  // Check if dot 69 gobbled already
	  myGLCD.fillCircle(229, 221, 2); // dot 69
     } 
     if (dot[59] == 1) {  // Check if dot 59 gobbled already
	  myGLCD.fillCircle(241, 201, 2); // dot 59
     }
     if (dot[71] == 1) {  // Check if dot 71 gobbled already
	  myGLCD.fillCircle(275, 221, 2); // dot 71
     }       
  } else
  if (xP== 262) { // dot 71
     if (dot[70] == 1) {  // Check if dot 70 gobbled already
	  myGLCD.fillCircle(252, 221, 2); // dot 70
     }  
     if (dot[72] == 1) {  // Check if dot 72 gobbled already
	  myGLCD.fillCircle(298, 221, 2); // dot 72
     }       
  } else
  if (xP== 284) { // dot 72
     if (dot[71] == 1) {  // Check if dot 71 gobbled already
	  myGLCD.fillCircle(275, 221, 2); // dot 71
     } 
     if (dot[60] == 1) {  // Check if dot 60 gobbled already
	  myGLCD.fillCircle(298, 201, 7); // Big dot 60
     }     
  }
} else



// Check Columns


if (xP== 28) {  // if in Column 2
  if (yP== 66) { // dot 32
     if (dot[20] == 1) {  // Check if dot 20 gobbled already
	  myGLCD.fillCircle(42, 60, 2); // dot 20
     }     
     if (dot[34] == 1) {  // Check if dot 34 gobbled already
	  myGLCD.fillCircle(42, 100, 2); // dot 34
     }        
  } else
  if (yP== 86) { // dot 34
      if (dot[32] == 1) {  // Check if dot 32 gobbled already
  	myGLCD.fillCircle(42, 80, 2); // dot 32
     }  
      if (dot[36] == 1) {  // Check if dot 36 gobbled already
  	myGLCD.fillCircle(42, 120, 2); // dot 36
     }      
  } else
  if (yP== 106) { // dot 36
     if (dot[38] == 1) {  // Check if dot 38 gobbled already
	  myGLCD.fillCircle(42, 140, 2); // dot 38
     }     
     if (dot[34] == 1) {  // Check if dot 34 gobbled already
	  myGLCD.fillCircle(42, 100, 2); // dot 34
     }      
  } else
  if (yP== 126) { // dot 38
      if (dot[40] == 1) {  // Check if dot 40 gobbled already
  	myGLCD.fillCircle(42, 160, 2); // dot 40
     } 
      if (dot[36] == 1) {  // Check if dot 36 gobbled already
  	myGLCD.fillCircle(42, 120, 2); // dot 36
     }       
  } else
  if (yP== 146) { // dot 40
     if (dot[38] == 1) {  // Check if dot 38 gobbled already
	  myGLCD.fillCircle(42, 140, 2); // dot 38
     }     
     if (dot[43] == 1) {  // Check if dot 43 gobbled already
	  myGLCD.fillCircle(42, 181, 2); // dot 43
     }      
  } 

} else
if (xP== 262) {  // if in Column 7

  if (yP== 66) { // dot 33
      if (dot[30] == 1) {  // Check if dot 30 gobbled already
  	myGLCD.fillCircle(275, 60, 2); // dot 30
     }   
      if (dot[35] == 1) {  // Check if dot 35 gobbled already
  	myGLCD.fillCircle(275, 100, 2); // dot 35
     }   
  } else
  if (yP== 86) { // dot 35
      if (dot[33] == 1) {  // Check if dot 33 gobbled already
  	myGLCD.fillCircle(275, 80, 2); // dot 33
     }  
      if (dot[37] == 1) {  // Check if dot 37 gobbled already
  	myGLCD.fillCircle(275, 120, 2); // dot 37
     }     
  } else
  if (yP== 106) { // dot 37
      if (dot[35] == 1) {  // Check if dot 35 gobbled already
  	myGLCD.fillCircle(275, 100, 2); // dot 35
     }  
      if (dot[39] == 1) {  // Check if dot 39 gobbled already
  	myGLCD.fillCircle(275, 140, 2); // dot 39
     }      
  } else
  if (yP== 126) { // dot 39
      if (dot[37] == 1) {  // Check if dot 37 gobbled already
  	myGLCD.fillCircle(275, 120, 2); // dot 37
     }
     if (dot[41] == 1) {  // Check if dot 41 gobbled already
	  myGLCD.fillCircle(275, 160, 2); // dot 41
     }       
  } else
  if (yP== 146) { // dot 41
      if (dot[39] == 1) {  // Check if dot 39 gobbled already
  	myGLCD.fillCircle(275, 140, 2); // dot 39
     } 
     if (dot[53] == 1) {  // Check if dot 53 gobbled already
	  myGLCD.fillCircle(275, 181, 2); // dot 53
     }     
  } 
}



  
// increment Pacman Graphic Flag 0 = Closed, 1 = Medium Open, 2 = Wide Open
P=P+1; 
if(P==4){
  P=0; // Reset counter to closed
}

      
       
// Capture legacy direction to enable adequate blanking of trail
prevD = D;

/* Temp print variables for testing
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(114, 198, 206);

  myGLCD.printNumI(xT,100,140); // Print xP
  myGLCD.printNumI(yT,155,140); // Print yP 
*/


// Check if Dot has been eaten before and incrementing score

// Check Rows

if (yP == 4) {  // if in Row 1 **********************************************************
  if (xP == 4) { // dot 1
     if (dot[1] == 1) {  // Check if dot gobbled already
        dot[1] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 28) { // dot 2
     if (dot[2] == 1) {  // Check if dot gobbled already
        dot[2] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 52) { // dot 3
     if (dot[3] == 1) {  // Check if dot gobbled already
        dot[3] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 74) { // dot 4
     if (dot[4] == 1) {  // Check if dot gobbled already
        dot[4] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 98) { // dot 5
     if (dot[5] == 1) {  // Check if dot gobbled already
        dot[5] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 120) { // dot 6
     if (dot[6] == 1) {  // Check if dot gobbled already
        dot[6] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 168) { // dot 7
     if (dot[7] == 1) {  // Check if dot gobbled already
        dot[7] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 192) { // dot 8
     if (dot[8] == 1) {  // Check if dot gobbled already
        dot[8] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 216) { // dot 9
     if (dot[9] == 1) {  // Check if dot gobbled already
        dot[9] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 238) { // dot 10
     if (dot[10] == 1) {  // Check if dot gobbled already
        dot[10] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 262) { // dot 11
     if (dot[11] == 1) {  // Check if dot gobbled already
        dot[11] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 284) { // dot 12
     if (dot[12] == 1) {  // Check if dot gobbled already
        dot[12] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } 

} else 
if (yP == 26) {  // if in Row 2  **********************************************************
  if (xP == 4) { // dot 13
     if (dot[13] == 1) {  // Check if dot gobbled already
        dot[13] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score 
        // Turn Ghost Blue if Pacman eats Big Dots
        fruiteatenpacman = true; // Turn Ghost blue      
     }     
  } else
  if (xP == 62) { // dot 14
     if (dot[14] == 1) {  // Check if dot gobbled already
        dot[14] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 120) { // dot 15
     if (dot[15] == 1) {  // Check if dot gobbled already
        dot[15] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 168) { // dot 16
     if (dot[16] == 1) {  // Check if dot gobbled already
        dot[16] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 228) { // dot 17
     if (dot[17] == 1) {  // Check if dot gobbled already
        dot[17] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 284) { // dot 18
     if (dot[18] == 1) {  // Check if dot gobbled already
        dot[18] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score
        // Turn Ghost Blue if Pacman eats Big Dots
        fruiteatenpacman = true; // Turn Ghost Blue       
     }     
  } 

} else
if (yP == 46) {  // if in Row 3  **********************************************************
  if (xP == 4) { // dot 19
     if (dot[19] == 1) {  // Check if dot gobbled already
        dot[19] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 28) { // dot 20
     if (dot[20] == 1) {  // Check if dot gobbled already
        dot[20] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 52) { // dot 21
     if (dot[21] == 1) {  // Check if dot gobbled already
        dot[21] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 74) { // dot 22
     if (dot[22] == 1) {  // Check if dot gobbled already
        dot[22] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 98) { // dot 23
     if (dot[23] == 1) {  // Check if dot gobbled already
        dot[23] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 120) { // dot 24
     if (dot[24] == 1) {  // Check if dot gobbled already
        dot[24] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 146) { // dot 25
     if (dot[25] == 1) {  // Check if dot gobbled already
        dot[25] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else

  if (xP == 168) { // dot 26
     if (dot[26] == 1) {  // Check if dot gobbled already
        dot[26] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 192) { // dot 27
     if (dot[27] == 1) {  // Check if dot gobbled already
        dot[27] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 216) { // dot 28
     if (dot[28] == 1) {  // Check if dot gobbled already
        dot[28] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 238) { // dot 29
     if (dot[29] == 1) {  // Check if dot gobbled already
        dot[29] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 262) { // dot 30
     if (dot[30] == 1) {  // Check if dot gobbled already
        dot[30] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 284) { // dot 31
     if (dot[31] == 1) {  // Check if dot gobbled already
        dot[31] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } 

} else
if (yP == 168) {  // if in Row 4  **********************************************************
  if (xP == 4) { // dot 42
     if (dot[42] == 1) {  // Check if dot gobbled already
        dot[42] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 28) { // dot 43
     if (dot[43] == 1) {  // Check if dot gobbled already
        dot[43] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 52) { // dot 44
     if (dot[44] == 1) {  // Check if dot gobbled already
        dot[44] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 74) { // dot 45
     if (dot[45] == 1) {  // Check if dot gobbled already
        dot[45] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 98) { // dot 46
     if (dot[46] == 1) {  // Check if dot gobbled already
        dot[46] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 120) { // dot 47
     if (dot[47] == 1) {  // Check if dot gobbled already
        dot[47] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 146) { // dot 48
     if (dot[48] == 1) {  // Check if dot gobbled already
        dot[48] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else

  if (xP == 168) { // dot 49
     if (dot[49] == 1) {  // Check if dot gobbled already
        dot[49] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 192) { // dot 50
     if (dot[50] == 1) {  // Check if dot gobbled already
        dot[50] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 216) { // dot 51
     if (dot[51] == 1) {  // Check if dot gobbled already
        dot[51] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 238) { // dot 52
     if (dot[52] == 1) {  // Check if dot gobbled already
        dot[52] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 262) { // dot 53
     if (dot[53] == 1) {  // Check if dot gobbled already
        dot[53] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 284) { // dot 54
     if (dot[54] == 1) {  // Check if dot gobbled already
        dot[54] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } 

} else
if (yP == 188) {  // if in Row 5  **********************************************************
  if (xP == 4) { // dot 55
     if (dot[55] == 1) {  // Check if dot gobbled already
        dot[55] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score
         // Turn Ghost Blue if Pacman eats Big Dots
        fruiteatenpacman = true; // Turn Ghost blue         
     }     
  } else
  if (xP == 62) { // dot 56
     if (dot[56] == 1) {  // Check if dot gobbled already
        dot[56] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 120) { // dot 57
     if (dot[57] == 1) {  // Check if dot gobbled already
        dot[57] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 168) { // dot 58
     if (dot[58] == 1) {  // Check if dot gobbled already
        dot[58] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 228) { // dot 59
     if (dot[59] == 1) {  // Check if dot gobbled already
        dot[59] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 284) { // dot 60
     if (dot[60] == 1) {  // Check if dot gobbled already
        dot[60] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score 
          // Turn Ghost Blue if Pacman eats Big Dots
        fruiteatenpacman = true; // Turn Ghost blue        
     }     
  } 

} else
if (yP == 208) {  // if in Row 6  **********************************************************
  if (xP == 4) { // dot 61
     if (dot[61] == 1) {  // Check if dot gobbled already
        dot[61] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 28) { // dot 62
     if (dot[62] == 1) {  // Check if dot gobbled already
        dot[62] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 52) { // dot 63
     if (dot[63] == 1) {  // Check if dot gobbled already
        dot[63] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 74) { // dot 64
     if (dot[64] == 1) {  // Check if dot gobbled already
        dot[64] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 98) { // dot 65
     if (dot[65] == 1) {  // Check if dot gobbled already
        dot[65] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 120) { // dot 66
     if (dot[66] == 1) {  // Check if dot gobbled already
        dot[66] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 168) { // dot 67
     if (dot[67] == 1) {  // Check if dot gobbled already
        dot[67] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 192) { // dot 68
     if (dot[68] == 1) {  // Check if dot gobbled already
        dot[68] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 216) { // dot 69
     if (dot[69] == 1) {  // Check if dot gobbled already
        dot[69] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 238) { // dot 70
     if (dot[70] == 1) {  // Check if dot gobbled already
        dot[70] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 262) { // dot 71
     if (dot[71] == 1) {  // Check if dot gobbled already
        dot[71] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (xP == 284) { // dot 72
     if (dot[72] == 1) {  // Check if dot gobbled already
        dot[72] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } 

}   



// Check Columns


if (xP == 28) {  // if in Column 2
  if (yP == 66) { // dot 32
     if (dot[32] == 1) {  // Check if dot gobbled already
        dot[32] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (yP == 86) { // dot 34
     if (dot[34] == 1) {  // Check if dot gobbled already
        dot[34] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (yP == 108) { // dot 36
     if (dot[36] == 1) {  // Check if dot gobbled already
        dot[36] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (yP == 126) { // dot 38
     if (dot[38] == 1) {  // Check if dot gobbled already
        dot[38] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (yP == 146) { // dot 40
     if (dot[40] == 1) {  // Check if dot gobbled already
        dot[40] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } 

} else
if (xP == 262) {  // if in Column 7
  if (yP == 66) { // dot 33
     if (dot[33] == 1) {  // Check if dot gobbled already
        dot[33] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (yP == 86) { // dot 35
     if (dot[35] == 1) {  // Check if dot gobbled already
        dot[35] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (yP == 106) { // dot 37
     if (dot[37] == 1) {  // Check if dot gobbled already
        dot[37] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (yP == 126) { // dot 39
     if (dot[39] == 1) {  // Check if dot gobbled already
        dot[39] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } else
  if (yP == 146) { // dot 41
     if (dot[41] == 1) {  // Check if dot gobbled already
        dot[41] = 0; // Reset flag to Zero
        pacmanscore++; // Increment pacman score       
     }     
  } 
} 

 

//Pacman wandering Algorithm 
// Note: Keep horizontal and vertical coordinates even numbers only to accomodate increment rate and starting point
// Pacman direction variable D where 0 = right, 1 = down, 2 = left, 3 = up

//****************************************************************************************************************************
//Right hand motion and ***************************************************************************************************
//****************************************************************************************************************************



if(D == 0){
// Increment xP and then test if any decisions required on turning up or down
  xP = xP+2; 

 // There are four horizontal rows that need rules

  // First Horizontal Row
  if (yP == 4) { 

    // Past first block decide to continue or go down
    if (xP == 62) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
    // Past second block only option is down
    if (xP == 120) { 
         D = 1; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
    // Past third block decide to continue or go down
    if (xP == 228) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
    // Past fourth block only option is down
    if (xP == 284) { 
         D = 1; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
  }

  // 2nd Horizontal Row
  if (yP == 46) { 

    // Past upper doorway on left decide to continue or go down
    if (xP == 28) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Past first block decide to continue or go up
    if (xP == 62) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
     // Past Second block decide to continue or go up
    if (xP == 120) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

     // Past Mid Wall decide to continue or go up
    if (xP == 168) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Past third block decide to continue or go up
    if (xP == 228) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Past last clock digit decide to continue or go down
    if (xP == 262) { 
      direct = random(2); // generate random number between 0 and 2
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Past fourth block only option is up
    if (xP == 284) { 
         D = 3; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
  }

  // LHS Door Horizontal Row
  if (yP == 108) { 

    // Past upper doorway on left decide to go up or go down
    if (xP == 28) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 1; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 3;}    
    }
  }

  // 3rd Horizontal Row
  if (yP == 168) { 

    // Past lower doorway on left decide to continue or go up
    if (xP == 28) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Past first block decide to continue or go down
    if (xP == 62) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
     // Past Second block decide to continue or go down
    if (xP == 120) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

     // Past Mid Wall decide to continue or go down
    if (xP == 168) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Past third block decide to continue or go down
    if (xP == 228) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Past last clock digit decide to continue or go up
    if (xP == 262) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Past fourth block only option is down
    if (xP == 284) { 
         D = 1; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
  }
 
  
  // 4th Horizontal Row
  if (yP == 208) { 

    // Past first block decide to continue or go up
    if (xP == 62) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
    // Past second block only option is up
    if (xP == 120) { 
         D = 3; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
    // Past third block decide to continue or go up
    if (xP == 228) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
    // Past fourth block only option is up
    if (xP == 284) { 
         D = 3; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
   }
}

//****************************************************************************************************************************
//Left hand motion **********************************************************************************************************
//****************************************************************************************************************************

else if(D == 2){
// Increment xP and then test if any decisions required on turning up or down
  xP = xP-2; 

/* Temp print variables for testing
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(114, 198, 206);
  myGLCD.printNumI(xP,80,165); // Print xP
  myGLCD.printNumI(yP,110,165); // Print yP
*/

 // There are four horizontal rows that need rules

  // First Horizontal Row  ******************************
  if (yP == 4) { 

     // Past first block only option is down
    if (xP == 4) { 
         D = 1; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
 
    // Past second block decide to continue or go down
    if (xP == 62) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
    // Past third block only option is down
    if (xP == 168) { 
         D = 1; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
    // Past fourth block decide to continue or go down
    if (xP == 228) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
  }

  // 2nd Horizontal Row ******************************
  if (yP == 46) { 

    // Meet LHS wall only option is up
    if (xP == 4) { 
         D = 3; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
    // Meet upper doorway on left decide to continue or go down
    if (xP == 28) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Meet first block decide to continue or go up
    if (xP == 62) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
     // Meet Second block decide to continue or go up
    if (xP == 120) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

     // Meet Mid Wall decide to continue or go up
    if (xP == 168) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Meet third block decide to continue or go up
    if (xP == 228) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Meet last clock digit decide to continue or go down
    if (xP == 262) { 
      direct = random(2); // generate random number between 0 and 3
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

  }
  

  // 3rd Horizontal Row ******************************
  if (yP == 168) { 

    // Meet LHS lower wall only option is down
    if (xP == 4) { 
         D = 1; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }

    // Meet lower doorway on left decide to continue or go up
    if (xP == 28) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Meet first block decide to continue or go down
    if (xP == 62) { 
      direct = random(2); // generate random number between 0 and 3
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
     // Meet Second block decide to continue or go down
    if (xP == 120) { 
      direct = random(2); // generate random number between 0 and 3
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

     // Meet Mid Wall decide to continue or go down
    if (xP == 168) { 
      direct = random(2); // generate random number between 0 and 3
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Meet third block decide to continue or go down
    if (xP == 228) { 
      direct = random(2); // generate random number between 0 and 3
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Meet last clock digit above decide to continue or go up
    if (xP == 262) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    
    }
    
  }
   // 4th Horizontal Row ******************************
  if (yP == 208) { 

    // Meet LHS wall only option is up
    if (xP == 4) { 
         D = 3; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }  
    // Meet first block decide to continue or go up
    if (xP == 62) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
    // Meet bottom divider wall only option is up
    if (xP == 168) { 
         D = 3; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
    // Meet 3rd block decide to continue or go up
    if (xP == 228) { 
      direct = random(4); // generate random number between 0 and 3
      if (direct == 3){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
   
  
  }
}  
  


//****************************************************************************************************************************
//Down motion **********************************************************************************************************
//****************************************************************************************************************************

else if(D == 1){
// Increment yP and then test if any decisions required on turning up or down
  yP = yP+2; 

/* Temp print variables for testing
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(114, 198, 206);
  myGLCD.printNumI(xP,80,165); // Print xP
  myGLCD.printNumI(yP,110,165); // Print yP
*/

 // There are vertical rows that need rules

  // First Vertical Row  ******************************
  if (xP == 4) { 

     // Past first block only option is right
    if (yP == 46) { 
         D = 0; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
 
    // Towards bottom wall only option right
    if (yP == 208) { 
         D = 0; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    } 
  }

  // 2nd Vertical Row ******************************
  if (xP == 28) { 

    // Meet bottom doorway on left decide to go left or go right
    if (yP == 168) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }
  }

  // 3rd Vertical Row ******************************
  if (xP == 62) { 

    // Meet top lh digit decide to go left or go right
    if (yP == 46) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }

    // Meet top lh digit decide to go left or go right
    if (yP == 208) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }
  }

  // 5th Vertical Row ******************************
  if (xP == 120) { 

    // Meet top lh digit decide to go left or go right
    if (yP == 46) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }

    // Meet bottom wall only opgion to go left
    if (yP == 208) { 
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up   
    }
  }

  // 6th Vertical Row ******************************
  if (xP == 168) { 

    // Meet top lh digit decide to go left or go right
    if (yP == 46) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }

    // Meet bottom wall only opgion to go right
    if (yP == 208) { 
         D = 0; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up   
    }
  }

  // 8th Vertical Row ******************************
  if (xP == 228) { 

    // Meet top lh digit decide to go left or go right
    if (yP == 46) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }

    // Meet bottom wall
    if (yP == 208) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }
  }

  // 9th Vertical Row ******************************
  if (xP == 262) { 

    // Meet bottom right doorway  decide to go left or go right
    if (yP == 168) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }
  }

  // 10th Vertical Row  ******************************
  if (xP == 284) { 

     // Past first block only option is left
    if (yP == 46) { 
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
 
    // Towards bottom wall only option right
    if (yP == 208) { 
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    } 
  } 
}  

//****************************************************************************************************************************
//Up motion **********************************************************************************************************
//****************************************************************************************************************************

else if(D == 3){
// Decrement yP and then test if any decisions required on turning up or down
  yP = yP-2; 

/* Temp print variables for testing
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(114, 198, 206);
  myGLCD.printNumI(xP,80,165); // Print xP
  myGLCD.printNumI(yP,110,165); // Print yP
*/


 // There are vertical rows that need rules

  // First Vertical Row  ******************************
  if (xP == 4) { 

     // Past first block only option is right
    if (yP == 4) { 
         D = 0; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
 
    // Towards bottom wall only option right
    if (yP == 168) { 
         D = 0; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    } 
  }

  // 2nd Vertical Row ******************************
  if (xP == 28) { 

    // Meet top doorway on left decide to go left or go right
    if (yP == 46) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }
  }

  // 3rd Vertical Row ******************************
  if (xP == 62) { 

    // Meet top lh digit decide to go left or go right
    if (yP == 4) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }

    // Meet top lh digit decide to go left or go right
    if (yP == 168) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }
  }

  // 5th Vertical Row ******************************
  if (xP == 120) { 

    // Meet bottom lh digit decide to go left or go right
    if (yP == 168) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }

    // Meet top wall only opgion to go left
    if (yP == 4) { 
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up   
    }
  }

  // 6th Vertical Row ******************************
  if (xP == 168) { 

    // Meet bottom lh digit decide to go left or go right
    if (yP == 168) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }

    // Meet top wall only opgion to go right
    if (yP == 4) { 
         D = 0; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up   
    }
  }

  // 8th Vertical Row ******************************
  if (xP == 228) { 

    // Meet bottom lh digit decide to go left or go right
    if (yP == 168) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }

    // Meet top wall go left or right
    if (yP == 4) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }
  }

  // 9th Vertical Row ******************************
  if (xP == 262) { 

    // Meet top right doorway  decide to go left or go right
    if (yP == 46) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { D = 0;}    
    }
  }

  // 10th Vertical Row  ******************************
  if (xP == 284) { 

     // Past first block only option is left
    if (yP == 168) { 
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
 
    // Towards top wall only option right
    if (yP == 4) { 
         D = 2; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    } 
  } 
}  
}

  
//******************************************************************************************************************


//Ghost ; 
// Note: Keep horizontal and verticalcoordinates even numbers only to accomodateincrement rate and starting point
// Ghost direction variable  D where 0 = right, 1 = down, 2 = left, 3 = up

//****************************************************************************************************************************
//Right hand motion **********************************************************************************************************
//****************************************************************************************************************************


// If ghost captured then ghost dissapears until reset
if ((fruiteatenpacman == true)&&(abs(xG-xP)<=5)&&(abs(yG-yP)<=5)){ 
  
  if (ghostlost == false){
    pacmanscore++;
    pacmanscore++;  
  }

  ghostlost = true;

  dly = 28; // slowdown now only drawing one item
  }
  
  
if (ghostlost == false){ // only draw ghost if still alive

drawGhost(xG,yG,GD,prevGD); // Draws Ghost at these coordinates


// If Ghost is on a dot then print the adjacent dots if they are valid

  myGLCD.setColor(200, 200, 200);
  
// Check Rows

if (yG == 4) {  // if in Row 1 **********************************************************
  if (xG == 4) { // dot 1
     if (dot[2] == 1) {  // Check if dot 2 gobbled already
	  myGLCD.fillCircle(42, 19, 2); // dot 2
     }    
      if (dot[13] == 1) {  // Check if dot 13 gobbled already
  	myGLCD.fillCircle(19, 40, 7); // Big dot 13
     }    

  } else
  if (xG == 28) { // dot 2
     if (dot[1] == 1) {  // Check if dot 1 gobbled already
	  myGLCD.fillCircle(19, 19, 2); // dot 1
     }    
      if (dot[3] == 1) {  // Check if dot 3 gobbled already
  	myGLCD.fillCircle(65, 19, 2); // dot 3
     }    

  } else
  if (xG == 52) { // dot 3
     if (dot[2] == 1) {  // Check if dot 2 gobbled already
	  myGLCD.fillCircle(42, 19, 2); // dot 2
     }    
      if (dot[4] == 1) {  // Check if dot 4 gobbled already
  	myGLCD.fillCircle(88, 19, 2); // dot 4
     } 
      if (dot[14] == 1) {  // Check if dot 14 gobbled already
  	myGLCD.fillCircle(77, 40, 2); // dot 14
     }   
  } else
  if (xG == 74) { // dot 4
     if (dot[3] == 1) {  // Check if dot 3 gobbled already
  	myGLCD.fillCircle(65, 19, 2); // dot 3
     }    
      if (dot[5] == 1) {  // Check if dot 5 gobbled already
  	myGLCD.fillCircle(112, 19, 2); // dot 5
     }   
      if (dot[14] == 1) {  // Check if dot 14 gobbled already
  	myGLCD.fillCircle(77, 40, 2); // dot 14
     }    
  } else
  if (xG == 98) { // dot 5
     if (dot[4] == 1) {  // Check if dot 3 gobbled already
  	myGLCD.fillCircle(88, 19, 2); // dot 4
     }    
      if (dot[6] == 1) {  // Check if dot 5 gobbled already
  	myGLCD.fillCircle(136, 19, 2); // dot 6
     }     
  } else
  if (xG == 120) { // dot 6
     if (dot[5] == 1) {  // Check if dot 5 gobbled already
  	myGLCD.fillCircle(136, 19, 2); // dot 5
     }    
      if (dot[15] == 1) {  // Check if dot 15 gobbled already
  	myGLCD.fillCircle(136, 40, 2); // dot 15
     }     
  } else
 

 if (xG == 168) { // dot 7
      if (dot[16] == 1) {  // Check if dot 16 gobbled already
  	myGLCD.fillCircle(183, 40, 2); // dot 16
     }    
      if (dot[8] == 1) {  // Check if dot 8 gobbled already
  	myGLCD.fillCircle(206, 19, 2); // dot 8
     }     
  } else
  if (xG == 192) { // dot 8
      if (dot[7] == 1) {  // Check if dot 7 gobbled already
  	myGLCD.fillCircle(183, 19, 2); // dot 7
     }    
      if (dot[9] == 1) {  // Check if dot 9 gobbled already
  	myGLCD.fillCircle(229, 19, 2); // dot 9
     }    
  } else
  if (xG == 216) { // dot 9
      if (dot[10] == 1) {  // Check if dot 10 gobbled already
  	myGLCD.fillCircle(252, 19, 2); // dot 10
     }    
      if (dot[8] == 1) {  // Check if dot 8 gobbled already
  	myGLCD.fillCircle(206, 19, 2); // dot 8
     }      
      if (dot[17] == 1) {  // Check if dot 17 gobbled already
  	myGLCD.fillCircle(241, 40, 2); // dot 17
     }   
 } else
  if (xG == 238) { // dot 10
      if (dot[11] == 1) {  // Check if dot 11 gobbled already
  	myGLCD.fillCircle(275, 19, 2); // dot 11
     }    
      if (dot[9] == 1) {  // Check if dot 9 gobbled already
  	myGLCD.fillCircle(229, 19, 2); // dot 9
     }      
      if (dot[17] == 1) {  // Check if dot 17 gobbled already
  	myGLCD.fillCircle(241, 40, 2); // dot 17
     }   
  } else
  if (xG == 262) { // dot 11
      if (dot[10] == 1) {  // Check if dot 10 gobbled already
  	myGLCD.fillCircle(252, 19, 2); // dot 10
     }    
      if (dot[12] == 1) {  // Check if dot 12 gobbled already
  	myGLCD.fillCircle(298, 19, 2); // dot 12
     }    
  } else
  if (xG == 284) { // dot 12
      if (dot[11] == 1) {  // Check if dot 11 gobbled already
  	myGLCD.fillCircle(275, 19, 2); // dot 11
     }    
      if (dot[18] == 1) {  // Check if dot 18 gobbled already
  	myGLCD.fillCircle(298, 40, 7); // dot 18
     }  
  }
} else 
if (yG == 26) {  // if in Row 2  **********************************************************
  if (xG == 4) { // dot 13
     if (dot[1] == 1) {  // Check if dot 1 gobbled already
	  myGLCD.fillCircle(19, 19, 2); // dot 1
     }    
      if (dot[19] == 1) {  // Check if dot 19 gobbled already
  	myGLCD.fillCircle(19, 60, 2); //  dot 19
     }   
  } else
  
    if (xG == 62) { // dot 14
      if (dot[3] == 1) {  // Check if dot 3 gobbled already
  	myGLCD.fillCircle(65, 19, 2); // dot 3
     }   
         if (dot[4] == 1) {  // Check if dot 4 gobbled already
  	myGLCD.fillCircle(88, 19, 2); // dot 4
     } 
         if (dot[21] == 1) {  // Check if dot 21 gobbled already
  	myGLCD.fillCircle(65, 60, 2); // dot 21
     }   
      if (dot[22] == 1) {  // Check if dot 22 gobbled already
  	myGLCD.fillCircle(88, 60, 2); // dot 22
     }    
     
  } else
  
  if (xG == 120) { // dot 15
     if (dot[24] == 1) {  // Check if dot 24 gobbled already
  	myGLCD.fillCircle(136, 60, 2); // dot 24
     }    
      if (dot[6] == 1) {  // Check if dot 6 gobbled already
  	myGLCD.fillCircle(136, 19, 2); // dot 6
     }      
  } else
  if (xG == 168) { // dot 16
      if (dot[7] == 1) {  // Check if dot 7 gobbled already
  	myGLCD.fillCircle(183, 19, 2); // dot 7
     }    
      if (dot[26] == 1) {  // Check if dot 26 gobbled already
  	myGLCD.fillCircle(183, 60, 2); // dot 26
     }          
  } else
    if (xG == 228) { // dot 17
      if (dot[9] == 1) {  // Check if dot 9 gobbled already
  	myGLCD.fillCircle(229, 19, 2); // dot 9
     }      
       if (dot[10] == 1) {  // Check if dot 10 gobbled already
  	myGLCD.fillCircle(252, 19, 2); // dot 10
     }  
      if (dot[28] == 1) {  // Check if dot 28 gobbled already
	  myGLCD.fillCircle(229, 60, 2); // dot 28
     }  
       if (dot[29] == 1) {  // Check if dot 29 gobbled already
  	myGLCD.fillCircle(252, 60, 2); // dot 29
     }     
     
  } else
  if (xG == 284) { // dot 18
      if (dot[31] == 1) {  // Check if dot 31 gobbled already
  	myGLCD.fillCircle(298, 60, 2); // dot 31
     }    
      if (dot[12] == 1) {  // Check if dot 12 gobbled already
  	myGLCD.fillCircle(298, 19, 2); // dot 12
     }  
  }
} else
if (yG == 46) {  // if in Row 3  **********************************************************
  if (xG == 4) { // dot 19
     if (dot[20] == 1) {  // Check if dot 20 gobbled already
	  myGLCD.fillCircle(42, 60, 2); // dot 20
     }    
      if (dot[13] == 1) {  // Check if dot 13 gobbled already
  	myGLCD.fillCircle(19, 40, 7); // Big dot 13
     }  
  } else
  if (xG == 28) { // dot 20
     if (dot[19] == 1) {  // Check if dot 19 gobbled already
  	myGLCD.fillCircle(19, 60, 2); // dot 19
     }    
      if (dot[21] == 1) {  // Check if dot 21 gobbled already
  	myGLCD.fillCircle(65, 60, 2); // dot 21
     }   
      if (dot[32] == 1) {  // Check if dot 32 gobbled already
  	myGLCD.fillCircle(42, 80, 2); // dot 32
     }    
  } else
  if (xG == 52) { // dot 21
     if (dot[20] == 1) {  // Check if dot 20 gobbled already
	  myGLCD.fillCircle(42, 60, 2); // dot 20
     }    
      if (dot[22] == 1) {  // Check if dot 22 gobbled already
  	myGLCD.fillCircle(88, 60, 2); // dot 22
     } 
      if (dot[14] == 1) {  // Check if dot 14 gobbled already
  	myGLCD.fillCircle(77, 40, 2); // dot 14
     }         
  } else
  if (xG == 74) { // dot 22
      if (dot[21] == 1) {  // Check if dot 21 gobbled already
  	myGLCD.fillCircle(65, 60, 2); // dot 21
     }    
      if (dot[23] == 1) {  // Check if dot 23 gobbled already
  	myGLCD.fillCircle(112, 60, 2); // dot 23
     } 
      if (dot[14] == 1) {  // Check if dot 14 gobbled already
  	myGLCD.fillCircle(77, 40, 2); // dot 14
     }    
  } else
  if (xG == 98) { // dot 23
     if (dot[24] == 1) {  // Check if dot 24 gobbled already
	  myGLCD.fillCircle(136, 60, 2); // dot 24
     }    
      if (dot[22] == 1) {  // Check if dot 22 gobbled already
  	myGLCD.fillCircle(88, 60, 2); // dot 22
     }  
    
  } else
  if (xG == 120) { // dot 24
      if (dot[25] == 1) {  // Check if dot 25 gobbled already
  	myGLCD.fillCircle(160, 60, 2); // dot 25
     }    
      if (dot[23] == 1) {  // Check if dot 23 gobbled already
  	myGLCD.fillCircle(112, 60, 2); // dot 23
     }
      if (dot[15] == 1) {  // Check if dot 15 gobbled already
  	myGLCD.fillCircle(136, 40, 2); // dot 15
     }        
  } else
  if (xG == 146) { // dot 25
     if (dot[24] == 1) {  // Check if dot 24 gobbled already
	  myGLCD.fillCircle(136, 60, 2); // dot 24
     }    
      if (dot[26] == 1) {  // Check if dot 26 gobbled already
  	myGLCD.fillCircle(183, 60, 2); // dot 26
     }    
  } else
  if (xG == 168) { // dot 26
      if (dot[25] == 1) {  // Check if dot 25 gobbled already
  	myGLCD.fillCircle(160, 60, 2); // dot 25
     }    
      if (dot[27] == 1) {  // Check if dot 27 gobbled already
  	myGLCD.fillCircle(206, 60, 2); // dot 27
     }
      if (dot[16] == 1) {  // Check if dot 16 gobbled already
  	myGLCD.fillCircle(183, 40, 2); // dot 16
     }    
  } else
  if (xG == 192) { // dot 27
     if (dot[28] == 1) {  // Check if dot 28 gobbled already
	  myGLCD.fillCircle(229, 60, 2); // dot 28
     }    
      if (dot[26] == 1) {  // Check if dot 26 gobbled already
  	myGLCD.fillCircle(183, 60, 2); // dot 26
     }      
  } else
  if (xG == 216) { // dot 28
      if (dot[29] == 1) {  // Check if dot 29 gobbled already
  	myGLCD.fillCircle(252, 60, 2); // dot 29
     }    
      if (dot[27] == 1) {  // Check if dot 27 gobbled already
  	myGLCD.fillCircle(206, 60, 2); // dot 27
     }      
      if (dot[17] == 1) {  // Check if dot 17 gobbled already
  	myGLCD.fillCircle(241, 40, 2); // dot 17
     }   
  } else
  if (xG == 238) { // dot 29
     if (dot[28] == 1) {  // Check if dot 28 gobbled already
	  myGLCD.fillCircle(229, 60, 2); // dot 28
     }    
      if (dot[30] == 1) {  // Check if dot 30 gobbled already
  	myGLCD.fillCircle(275, 60, 2); // dot 30
     }      
      if (dot[17] == 1) {  // Check if dot 17 gobbled already
  	myGLCD.fillCircle(241, 40, 2); // dot 17
     }   
   
  } else
  if (xG == 262) { // dot 30
      if (dot[29] == 1) {  // Check if dot 29 gobbled already
  	myGLCD.fillCircle(252, 60, 2); // dot 29
     }    
      if (dot[33] == 1) {  // Check if dot 33 gobbled already
  	myGLCD.fillCircle(275, 80, 2); // dot 33
     }      
      if (dot[31] == 1) {  // Check if dot 31 gobbled already
  	myGLCD.fillCircle(298, 60, 2); // dot 31
     }  
  
  } else
  if (xG == 284) { // dot 31
   if (dot[18] == 1) {  // Check if dot 18 gobbled already
  	myGLCD.fillCircle(298, 40, 7); // dot 18
   }     
   if (dot[30] == 1) {  // Check if dot 30 gobbled already
  	myGLCD.fillCircle(275, 60, 2); // dot 30
   } 
  }
} else

if (yG == 168) {  // if in Row 4  **********************************************************
  if (xG == 4) { // dot 42
     if (dot[43] == 1) {  // Check if dot 43 gobbled already
	  myGLCD.fillCircle(42, 181, 2); // dot 43
     }     
     if (dot[55] == 1) {  // Check if dot 55 gobbled already
	  myGLCD.fillCircle(19, 201, 7); // dot 55
     }     
  } else
  if (xG == 28) { // dot 43
     if (dot[42] == 1) {  // Check if dot 42 gobbled already
	  myGLCD.fillCircle(19, 181, 2); // dot 42
     }     
     if (dot[44] == 1) {  // Check if dot 44 gobbled already
	  myGLCD.fillCircle(65, 181, 2); // dot 44
     }   
      if (dot[40] == 1) {  // Check if dot 40 gobbled already
  	myGLCD.fillCircle(42, 160, 2); // dot 40
     }       
  } else
  if (xG == 52) { // dot 44
     if (dot[43] == 1) {  // Check if dot 43 gobbled already
	  myGLCD.fillCircle(42, 181, 2); // dot 43
     }     
     if (dot[45] == 1) {  // Check if dot 45 gobbled already
	  myGLCD.fillCircle(88, 181, 2); // dot 45
     } 
     if (dot[56] == 1) {  // Check if dot 56 gobbled already
	  myGLCD.fillCircle(77, 201, 2); // dot 56 
     }    
  } else
  if (xG == 74) { // dot 45
     if (dot[46] == 1) {  // Check if dot 46 gobbled already
	  myGLCD.fillCircle(112, 181, 2); // dot 46
     }     
     if (dot[44] == 1) {  // Check if dot 44 gobbled already
	  myGLCD.fillCircle(65, 181, 2); // dot 44
     } 
     if (dot[56] == 1) {  // Check if dot 56 gobbled already
	  myGLCD.fillCircle(77, 201, 2); // dot 56 
     }    
     
  } else
  if (xG == 98) { // dot 46
     if (dot[47] == 1) {  // Check if dot 47 gobbled already
	  myGLCD.fillCircle(136, 181, 2); // dot 47
     }     
     if (dot[45] == 1) {  // Check if dot 45 gobbled already
	  myGLCD.fillCircle(88, 181, 2); // dot 45
     }  
  } else
  if (xG == 120) { // dot 47
     if (dot[48] == 1) {  // Check if dot 48 gobbled already
	  myGLCD.fillCircle(160, 181, 2); // dot 48
     }     
     if (dot[46] == 1) {  // Check if dot 46 gobbled already
	  myGLCD.fillCircle(112, 181, 2); // dot 46
     } 
     if (dot[57] == 1) {  // Check if dot 57 gobbled already
	  myGLCD.fillCircle(136, 201, 2); // dot 57 
     }      
  } else
  if (xG == 146) { // dot 48
     if (dot[47] == 1) {  // Check if dot 47 gobbled already
	  myGLCD.fillCircle(136, 181, 2); // dot 47
     }     
     if (dot[49] == 1) {  // Check if dot 49 gobbled already
	  myGLCD.fillCircle(183, 181, 2); // dot 49
     }  
  } else

  if (xG == 168) { // dot 49
     if (dot[48] == 1) {  // Check if dot 48 gobbled already
	  myGLCD.fillCircle(160, 181, 2); // dot 48
     }     
     if (dot[50] == 1) {  // Check if dot 50 gobbled already
	  myGLCD.fillCircle(206, 181, 2); // dot 50
     } 
     if (dot[58] == 1) {  // Check if dot 58 gobbled already
	  myGLCD.fillCircle(183, 201, 2); // dot 58
     }        
  } else
  if (xG == 192) { // dot 50
     if (dot[51] == 1) {  // Check if dot 51 gobbled already
	  myGLCD.fillCircle(229, 181, 2); // dot 51
     }     
     if (dot[49] == 1) {  // Check if dot 49 gobbled already
	  myGLCD.fillCircle(183, 181, 2); // dot 49
     }      
  } else
  if (xG == 216) { // dot 51
     if (dot[50] == 1) {  // Check if dot 50 gobbled already
	  myGLCD.fillCircle(206, 181, 2); // dot 50
     }    
     if (dot[52] == 1) {  // Check if dot 52 gobbled already
	  myGLCD.fillCircle(252, 181, 2); // dot 52
     } 
     if (dot[59] == 1) {  // Check if dot 59 gobbled already
	  myGLCD.fillCircle(241, 201, 2); // dot 59
     }     
  } else
  if (xG == 238) { // dot 52
     if (dot[53] == 1) {  // Check if dot 53 gobbled already
	  myGLCD.fillCircle(275, 181, 2); // dot 53
     }    
     if (dot[51] == 1) {  // Check if dot 51 gobbled already
	  myGLCD.fillCircle(229, 181, 2); // dot 51
     }  
     if (dot[59] == 1) {  // Check if dot 59 gobbled already
	  myGLCD.fillCircle(241, 201, 2); // dot 59
     }     
  } else
 




 if (xG == 262) { // dot 53
     if (dot[41] == 1) {  // Check if dot 41 gobbled already
	  myGLCD.fillCircle(275, 160, 2); // dot 41
     }    
     if (dot[52] == 1) {  // Check if dot 52 gobbled already
	  myGLCD.fillCircle(252, 181, 2); // dot 52
     } 
     if (dot[54] == 1) {  // Check if dot 54 gobbled already
	  myGLCD.fillCircle(298, 181, 2); // dot 54
     }     
  } else
  if (xG == 284) { // dot 54
     if (dot[53] == 1) {  // Check if dot 53 gobbled already
	  myGLCD.fillCircle(275, 181, 2); // dot 53
     }    
     if (dot[60] == 1) {  // Check if dot 60 gobbled already
	  myGLCD.fillCircle(298, 201, 7); // Big dot 60
     }      
  } 

} else
if (yG == 188) {  // if in Row 5  **********************************************************
  if (xG == 4) { // dot 55
     if (dot[42] == 1) {  // Check if dot 42 gobbled already
	  myGLCD.fillCircle(19, 181, 2); // dot 42
     } 
     if (dot[61] == 1) {  // Check if dot 61 gobbled already
	  myGLCD.fillCircle(19, 221, 2); // dot 61
     }    
  } else
   if (xG == 62) { // dot 56
     if (dot[44] == 1) {  // Check if dot 44 gobbled already
	  myGLCD.fillCircle(65, 181, 2); // dot 44
     } 
     if (dot[45] == 1) {  // Check if dot 45 gobbled already
	  myGLCD.fillCircle(88, 181, 2); // dot 45
     } 
     if (dot[63] == 1) {  // Check if dot 63 gobbled already
	  myGLCD.fillCircle(65, 221, 2); // dot 63
     }
     if (dot[64] == 1) {  // Check if dot 64 gobbled already
	  myGLCD.fillCircle(88, 221, 2); // dot 64
     }      
     
  } else
  
  if (xG == 120) { // dot 57
     if (dot[47] == 1) {  // Check if dot 47 gobbled already
	  myGLCD.fillCircle(136, 181, 2); // dot 47
     }     
     if (dot[66] == 1) {  // Check if dot 66 gobbled already
	  myGLCD.fillCircle(136, 221, 2); // dot 66
     }    
  } else
  if (xG == 168) { // dot 58
     if (dot[67] == 1) {  // Check if dot 67 gobbled already
	  myGLCD.fillCircle(183, 221, 2); // dot 67
     }     
     if (dot[49] == 1) {  // Check if dot 49 gobbled already
	  myGLCD.fillCircle(183, 181, 2); // dot 49
     }       
  } else
  
  if (xG == 228) { // dot 59
     if (dot[51] == 1) {  // Check if dot 51 gobbled already
	  myGLCD.fillCircle(229, 181, 2); // dot 51
     }
     if (dot[52] == 1) {  // Check if dot 52 gobbled already
	  myGLCD.fillCircle(252, 181, 2); // dot 52
     } 
     if (dot[69] == 1) {  // Check if dot 69 gobbled already
	  myGLCD.fillCircle(229, 221, 2); // dot 69
     } 
     if (dot[70] == 1) {  // Check if dot 70 gobbled already
	  myGLCD.fillCircle(252, 221, 2); // dot 70
     }      
     
  } else
  
  if (xG == 284) { // dot 60
     if (dot[72] == 1) {  // Check if dot 72 gobbled already
	  myGLCD.fillCircle(298, 221, 2); //  dot 72
     } 
     if (dot[54] == 1) {  // Check if dot 54 gobbled already
	  myGLCD.fillCircle(298, 181, 2); // dot 54
     }    
  } 

} else


if (yG == 208) {  // if in Row 6  **********************************************************
  if (xG == 4) { // dot 61
     if (dot[55] == 1) {  // Check if dot 55 gobbled already
	  myGLCD.fillCircle(19, 201, 7); // dot 55
     } 
     if (dot[62] == 1) {  // Check if dot 62 gobbled already
	  myGLCD.fillCircle(42, 221, 2); // dot 62
     }   
  } else
  if (xG == 28) { // dot 62
     if (dot[61] == 1) {  // Check if dot 61 gobbled already
	  myGLCD.fillCircle(19, 221, 2); // dot 61
     }  
     if (dot[63] == 1) {  // Check if dot 63 gobbled already
	  myGLCD.fillCircle(65, 221, 2); // dot 63
     }      
  } else
  if (xG == 52) { // dot 63
     if (dot[64] == 1) {  // Check if dot 64 gobbled already
	  myGLCD.fillCircle(88, 221, 2); // dot 64
     } 
     if (dot[62] == 1) {  // Check if dot 62 gobbled already
	  myGLCD.fillCircle(42, 221, 2); // dot 62
     }  
     if (dot[56] == 1) {  // Check if dot 56 gobbled already
	  myGLCD.fillCircle(77, 201, 2); // dot 56 
     }      
  } else
  if (xG == 74) { // dot 64
     if (dot[65] == 1) {  // Check if dot 65 gobbled already
	  myGLCD.fillCircle(112, 221, 2); // dot 65
     } 
     if (dot[63] == 1) {  // Check if dot 63 gobbled already
	  myGLCD.fillCircle(65, 221, 2); // dot 63
     }  
     if (dot[56] == 1) {  // Check if dot 56 gobbled already
	  myGLCD.fillCircle(77, 201, 2); // dot 56 
     }     
  } else
  if (xG == 98) { // dot 65
     if (dot[64] == 1) {  // Check if dot 64 gobbled already
	  myGLCD.fillCircle(88, 221, 2); // dot 64
     } 
     if (dot[66] == 1) {  // Check if dot 66 gobbled already
	  myGLCD.fillCircle(136, 221, 2); // dot 66
     }    
  } else
  if (xG == 120) { // dot 66
     if (dot[65] == 1) {  // Check if dot 65 gobbled already
	  myGLCD.fillCircle(112, 221, 2); // dot 65
     } 
     if (dot[57] == 1) {  // Check if dot 57 gobbled already
	  myGLCD.fillCircle(136, 201, 2); // dot 57 
     }    
  } else
  if (xG == 168) { // dot 67
     if (dot[68] == 1) {  // Check if dot 68 gobbled already
	  myGLCD.fillCircle(206, 221, 2); // dot 68
     } 
     if (dot[58] == 1) {  // Check if dot 58 gobbled already
	  myGLCD.fillCircle(183, 201, 2); // dot 58
     }     
  } else
  if (xG == 192) { // dot 68
     if (dot[67] == 1) {  // Check if dot 67 gobbled already
	  myGLCD.fillCircle(183, 221, 2); // dot 67
     } 
     if (dot[69] == 1) {  // Check if dot 69 gobbled already
	  myGLCD.fillCircle(229, 221, 2); // dot 69
     }    
  } else
  if (xG == 216) { // dot 69
     if (dot[68] == 1) {  // Check if dot 68 gobbled already
	  myGLCD.fillCircle(206, 221, 2); // dot 68
     } 
     if (dot[59] == 1) {  // Check if dot 59 gobbled already
	  myGLCD.fillCircle(241, 201, 2); // dot 59
     }
     if (dot[70] == 1) {  // Check if dot 70 gobbled already
	  myGLCD.fillCircle(252, 221, 2); // dot 70
     }    
  } else
  if (xG == 238) { // dot 70
     if (dot[69] == 1) {  // Check if dot 69 gobbled already
	  myGLCD.fillCircle(229, 221, 2); // dot 69
     } 
     if (dot[59] == 1) {  // Check if dot 59 gobbled already
	  myGLCD.fillCircle(241, 201, 2); // dot 59
     }
     if (dot[71] == 1) {  // Check if dot 71 gobbled already
	  myGLCD.fillCircle(275, 221, 2); // dot 71
     }       
  } else
  if (xG == 262) { // dot 71
     if (dot[70] == 1) {  // Check if dot 70 gobbled already
	  myGLCD.fillCircle(252, 221, 2); // dot 70
     }  
     if (dot[72] == 1) {  // Check if dot 72 gobbled already
	  myGLCD.fillCircle(298, 221, 2); // dot 72
     }       
  } else
  if (xG == 284) { // dot 72
     if (dot[71] == 1) {  // Check if dot 71 gobbled already
	  myGLCD.fillCircle(275, 221, 2); // dot 71
     } 
     if (dot[60] == 1) {  // Check if dot 60 gobbled already
	  myGLCD.fillCircle(298, 201, 7); // Big dot 60
     }     
  }
} else



// Check Columns


if (xG == 28) {  // if in Column 2
  if (yG == 66) { // dot 32
     if (dot[20] == 1) {  // Check if dot 20 gobbled already
	  myGLCD.fillCircle(42, 60, 2); // dot 20
     }     
     if (dot[34] == 1) {  // Check if dot 34 gobbled already
	  myGLCD.fillCircle(42, 100, 2); // dot 34
     }        
  } else
  if (yG == 86) { // dot 34
      if (dot[32] == 1) {  // Check if dot 32 gobbled already
  	myGLCD.fillCircle(42, 80, 2); // dot 32
     }  
      if (dot[36] == 1) {  // Check if dot 36 gobbled already
  	myGLCD.fillCircle(42, 120, 2); // dot 36
     }      
  } else
  if (yG == 106) { // dot 36
     if (dot[38] == 1) {  // Check if dot 38 gobbled already
	  myGLCD.fillCircle(42, 140, 2); // dot 38
     }     
     if (dot[34] == 1) {  // Check if dot 34 gobbled already
	  myGLCD.fillCircle(42, 100, 2); // dot 34
     }      
  } else
  if (yG == 126) { // dot 38
      if (dot[40] == 1) {  // Check if dot 40 gobbled already
  	myGLCD.fillCircle(42, 160, 2); // dot 40
     } 
      if (dot[36] == 1) {  // Check if dot 36 gobbled already
  	myGLCD.fillCircle(42, 120, 2); // dot 36
     }       
  } else
  if (yG == 146) { // dot 40
     if (dot[38] == 1) {  // Check if dot 38 gobbled already
	  myGLCD.fillCircle(42, 140, 2); // dot 38
     }     
     if (dot[43] == 1) {  // Check if dot 43 gobbled already
	  myGLCD.fillCircle(42, 181, 2); // dot 43
     }      
  } 

} else
if (xG == 262) {  // if in Column 7

  if (yG == 66) { // dot 33
      if (dot[30] == 1) {  // Check if dot 30 gobbled already
  	myGLCD.fillCircle(275, 60, 2); // dot 30
     }   
      if (dot[35] == 1) {  // Check if dot 35 gobbled already
  	myGLCD.fillCircle(275, 100, 2); // dot 35
     }   
  } else
  if (yG == 86) { // dot 35
      if (dot[33] == 1) {  // Check if dot 33 gobbled already
  	myGLCD.fillCircle(275, 80, 2); // dot 33
     }  
      if (dot[37] == 1) {  // Check if dot 37 gobbled already
  	myGLCD.fillCircle(275, 120, 2); // dot 37
     }     
  } else
  if (yG == 106) { // dot 37
      if (dot[35] == 1) {  // Check if dot 35 gobbled already
  	myGLCD.fillCircle(275, 100, 2); // dot 35
     }  
      if (dot[39] == 1) {  // Check if dot 39 gobbled already
  	myGLCD.fillCircle(275, 140, 2); // dot 39
     }      
  } else
  if (yG == 126) { // dot 39
      if (dot[37] == 1) {  // Check if dot 37 gobbled already
  	myGLCD.fillCircle(275, 120, 2); // dot 37
     }
     if (dot[41] == 1) {  // Check if dot 41 gobbled already
	  myGLCD.fillCircle(275, 160, 2); // dot 41
     }       
  } else
  if (yG == 146) { // dot 41
      if (dot[39] == 1) {  // Check if dot 39 gobbled already
  	myGLCD.fillCircle(275, 140, 2); // dot 39
     } 
     if (dot[53] == 1) {  // Check if dot 53 gobbled already
	  myGLCD.fillCircle(275, 181, 2); // dot 53
     }     
  } 
}




// Capture legacy direction to enable adequate blanking of trail
prevGD = GD;

if(GD == 0){
// Increment xG and then test if any decisions required on turning up or down
  xG = xG+2; 

/* Temp print variables for testing
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(114, 198, 206);
  myGLCD.printNumI(xG,80,165); // Print xG
  myGLCD.printNumI(yP,110,165); // Print yP
*/



 // There are four horizontal rows that need rules

  // First Horizontal Row
  if (yG== 4) { 

    // Past first block decide to continue or go down
    if (xG == 62) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
    // Past second block only option is down
    if (xG == 120) { 
         GD = 1; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
    // Past third block decide to continue or go down
    if (xG == 228) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
    // Past fourth block only option is down
    if (xG == 284) { 
         GD = 1; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
  }

  // 2nd Horizontal Row
  if (yG == 46) { 

    // Past upper doorway on left decide to continue right or go down
    if (xG == 28) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Past first block decide to continue right or go up
    if (xG == 62) { 
      if (random(2) == 0){
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 3;}
    }
     // Past Second block decide to continue right or go up
    if (xG == 120) { 
      if (random(2) == 0){
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 3;}
    }

     // Past Mid Wall decide to continue right or go up
    if (xG == 168) { 
      if (random(2) == 0){
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 3;}
    }

    // Past third block decide to continue right or go up
    if (xG == 228) { 
      if (random(2) == 0){
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 3;}
    }

    // Past last clock digit decide to continue or go down
    if (xG == 262) { 
      gdirect = random(2); // generate random number between 0 and 2
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Past fourth block only option is up
    if (xG == 284) { 
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
  }

  // 3rd Horizontal Row
  if (yG== 168) { 

    // Past lower doorway on left decide to continue right or go up
    if (xG == 28) { 
      if (random(2) == 0){
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 3;}
    }

    // Past first block decide to continue or go down
    if (xG == 62) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
     // Past Second block decide to continue or go down
    if (xG == 120) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

     // Past Mid Wall decide to continue or go down
    if (xG == 168) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Past third block decide to continue or go down
    if (xG == 228) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Past last clock digit decide to continue right or go up
    if (xG == 262) { 
      if (random(2) == 0){
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 3;}
    }

    // Past fourth block only option is down
    if (xG == 284) { 
         GD = 1; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
  }
 
  
  // 4th Horizontal Row
  if (yG== 208) { 

    // Past first block decide to continue right or go up
    if (xG == 62) { 
      if (random(2) == 0){
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 3;}
    }
    // Past second block only option is up
    if (xG == 120) { 
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
    // Past third block decide to continue right or go up
    if (xG == 228) { 
      if (random(2) == 0){
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 3;}
    }
    // Past fourth block only option is up
    if (xG == 284) { 
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
   }
}

//****************************************************************************************************************************
//Left hand motion **********************************************************************************************************
//****************************************************************************************************************************

else if(GD == 2){
// Increment xG and then test if any decisions required on turning up or down
  xG = xG-2; 

/* Temp print variables for testing
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(114, 198, 206);
  myGLCD.printNumI(xG,80,165); // Print xG
  myGLCD.printNumI(yP,110,165); // Print yP
*/

 // There are four horizontal rows that need rules

  // First Horizontal Row  ******************************
  if (yG== 4) { 

     // Past first block only option is down
    if (xG == 4) { 
         GD = 1; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
 
    // Past second block decide to continue or go down
    if (xG == 62) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
    // Past third block only option is down
    if (xG == 168) { 
         GD = 1; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
    // Past fourth block decide to continue or go down
    if (xG == 228) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
  }

  // 2nd Horizontal Row ******************************
  if (yG== 46) { 

    // Meet LHS wall only option is up
    if (xG == 4) { 
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
    // Meet upper doorway on left decide to continue left or go down
    if (xG == 28) { 
      if (random(2) == 0){
         GD = 1; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 2;}
    }

    // Meet first block decide to continue left or go up
    if (xG == 62) { 
      if (random(2) == 0){
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 2;}
    }
     // Meet Second block decide to continue left or go up
    if (xG == 120) { 
      if (random(2) == 0){
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 2;}
    }

     // Meet Mid Wall decide to continue left or go up
    if (xG == 168) { 
      if (random(2) == 0){
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 2;}
    }

    // Meet third block decide to continue left or go up
    if (xG == 228) { 
      if (random(2) == 0){
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 2;}
    }

    // Meet last clock digit decide to continue or go down
    if (xG == 262) { 
      gdirect = random(2); // generate random number between 0 and 3
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

  }
 
   // RHS Door Horizontal Row
  if (yG == 108) { 

    // Past upper doorway on left decide to go up or go down
    if (xG == 262) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 1; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 3;}    
    }
  } 

  // 3rd Horizontal Row ******************************
  if (yG== 168) { 

    // Meet LHS lower wall only option is down
    if (xG == 4) { 
         GD = 1; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }

    // Meet lower doorway on left decide to continue left or go up
    if (xG == 28) { 
      if (random(2) == 0){
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 2;}
    }

    // Meet first block decide to continue or go down
    if (xG == 62) { 
      gdirect = random(2); // generate random number between 0 and 3
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }
     // Meet Second block decide to continue or go down
    if (xG == 120) { 
      gdirect = random(2); // generate random number between 0 and 3
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

     // Meet Mid Wall decide to continue or go down
    if (xG == 168) { 
      gdirect = random(2); // generate random number between 0 and 3
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Meet third block decide to continue or go down
    if (xG == 228) { 
      gdirect = random(2); // generate random number between 0 and 3
      if (gdirect == 1){
         GD = gdirect; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

    // Meet last clock digit above decide to continue left or go up
    if (xG == 262) { 
      if (random(2) == 0){
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 2;}
    
    }
    
  }
   // 4th Horizontal Row ******************************
  if (yG== 208) { 

    // Meet LHS wall only option is up
    if (xG == 4) { 
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }  
    // Meet first block decide to continue left or go up
    if (xG == 62) { 
      if (random(2) == 0){
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 2;}
    }
    // Meet bottom divider wall only option is up
    if (xG == 168) { 
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
    // Meet 3rd block decide to continue left or go up
    if (xG == 228) { 
      if (random(2) == 0){
         GD = 3; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } else { GD = 2;}
    }
   
  
  }
}  
  


//****************************************************************************************************************************
//Down motion **********************************************************************************************************
//****************************************************************************************************************************

else if(GD == 1){
// Increment yGand then test if any decisions required on turning up or down
  yG= yG+2; 

/* Temp print variables for testing
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(114, 198, 206);
  myGLCD.printNumI(xG,80,165); // Print xG
  myGLCD.printNumI(yP,110,165); // Print yP
*/

 // There are vertical rows that need rules

  // First Vertical Row  ******************************
  if (xG == 4) { 

     // Past first block only option is right
    if (yG== 46) { 
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
 
    // Towards bottom wall only option right
    if (yG== 208) { 
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    } 
  }

  // 2nd Vertical Row ******************************
  if (xG == 28) { 

    // Meet bottom doorway on left decide to go left or go right
    if (yG== 168) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }
  }

  // 3rd Vertical Row ******************************
  if (xG == 62) { 

    // Meet top lh digit decide to go left or go right
    if (yG== 46) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }

    // Meet top lh digit decide to go left or go right
    if (yG== 208) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }
  }

  // 5th Vertical Row ******************************
  if (xG == 120) { 

    // Meet top lh digit decide to go left or go right
    if (yG== 46) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }

    // Meet bottom wall only opgion to go left
    if (yG== 208) { 
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up   
    }
  }

  // 6th Vertical Row ******************************
  if (xG == 168) { 

    // Meet top lh digit decide to go left or go right
    if (yG== 46) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }

    // Meet bottom wall only opgion to go right
    if (yG== 208) { 
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up   
    }
  }

  // 8th Vertical Row ******************************
  if (xG == 228) { 

    // Meet top lh digit decide to go left or go right
    if (yG== 46) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }

    // Meet bottom wall
    if (yG== 208) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }
  }

  // 9th Vertical Row ******************************
  if (xG == 262) { 

    // Meet bottom right doorway  decide to go left or go right
    if (yG== 168) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }
  }

  // 10th Vertical Row  ******************************
  if (xG == 284) { 

     // Past first block only option is left
    if (yG== 46) { 
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
 
    // Towards bottom wall only option right
    if (yG== 208) { 
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    } 
  } 
}  

//****************************************************************************************************************************
//Up motion **********************************************************************************************************
//****************************************************************************************************************************

else if(GD == 3){
// Decrement yGand then test if any decisions required on turning up or down
  yG= yG-2; 

/* Temp print variables for testing
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(114, 198, 206);
  myGLCD.printNumI(xG,80,165); // Print xG
  myGLCD.printNumI(yP,110,165); // Print yP
*/


 // There are vertical rows that need rules

  // First Vertical Row  ******************************
  if (xG == 4) { 

     // Past first block only option is right
    if (yG== 4) { 
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
 
    // Towards bottom wall only option right
    if (yG== 168) { 
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    } 
  }

  // 2nd Vertical Row ******************************
  if (xG == 28) { 

    // Meet top doorway on left decide to go left or go right
    if (yG== 46) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }
  }

  // 3rd Vertical Row ******************************
  if (xG == 62) { 

    // Meet top lh digit decide to go left or go right
    if (yG== 4) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }

    // Meet top lh digit decide to go left or go right
    if (yG== 168) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }
  }

  // 5th Vertical Row ******************************
  if (xG == 120) { 

    // Meet bottom lh digit decide to go left or go right
    if (yG== 168) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }

    // Meet top wall only opgion to go left
    if (yG== 4) { 
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up   
    }
  }

  // 6th Vertical Row ******************************
  if (xG == 168) { 

    // Meet bottom lh digit decide to go left or go right
    if (yG== 168) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }

    // Meet top wall only opgion to go right
    if (yG== 4) { 
         GD = 0; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up   
    }
  }

  // 8th Vertical Row ******************************
  if (xG == 228) { 

    // Meet bottom lh digit decide to go left or go right
    if (yG== 168) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }

    // Meet top wall go left or right
    if (yG== 4) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }
  }

  // 9th Vertical Row ******************************
  if (xG == 262) { 

    // Meet top right doorway  decide to go left or go right
    if (yG== 46) { 
      gdirect = random(2); // generate random number between 0 and 1
      if (gdirect == 1){
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      } 
      else { GD = 0;}    
    }
  }

  // 10th Vertical Row  ******************************
  if (xG == 284) { 

     // Past first block only option is left
    if (yG== 168) { 
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    }
 
    // Towards top wall only option right
    if (yG== 4) { 
         GD = 2; // set Ghost direction variable to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
    } 
  } 
}  

}
  




//******************************************************************************************************************

//******************************************************************************************************************



/*
//temp barriers

if (yP>200) {
  yP=46;
}
if(xP>260){
  xP=4;
}
*/
delay(dly); 
  


}

// ************************************************************************************************************
// ===== Update Digital Clock
// ************************************************************************************************************
 void UpdateDisp(){
 
// Clear the time area
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(0, 0, 0);
//  myGLCD.fillRect(60, 80 ,262, 166);

   
  int h; // Hour value in 24 hour format
  int e; // Minute value in minute format
  int pm = 0; // Flag to detrmine if PM or AM
  
  // There are four digits that need to be drawn independently to ensure consisitent positioning of time
  int d1;  // Tens hour digit
  int d2;  // Ones hour digit
  int d3;  // Tens minute digit
  int d4;  // Ones minute digit
  

  h = hour(); // 24 hour RT clock value
  e = minute();

/* TEST
h = 12;
e = 8;
*/


// Calculate hour digit values for time

if ((h >= 10) && (h <= 12)) {     // AM hours 10,11,12
  d1 = 1; // calculate Tens hour digit
  d2 = h - 10;  // calculate Ones hour digit 0,1,2
  } else  
  if ( (h >= 22) && (h <= 24)) {    // PM hours 10,11,12
  d1 = 1; // calculate Tens hour digit
  d2 = h - 22;  // calculate Ones hour digit 0,1,2    
  } else 
  if ((h <= 9)&&(h >= 1)) {     // AM hours below ten
  d1 = 0; // calculate Tens hour digit
  d2 = h;  // calculate Ones hour digit 0,1,2    
  } else
  if ( (h >= 13) && (h <= 21)) { // PM hours below 10
  d1 = 0; // calculate Tens hour digit
  d2 = h - 12;  // calculate Ones hour digit 0,1,2 
  } else { 
    // If hour is 0
  d1 = 1; // calculate Tens hour digit
  d2 = 2;  // calculate Ones hour digit 0,1,2   
  }
    
    
// Calculate minute digit values for time

if ((e >= 10)) {  
  d3 = e/10 ; // calculate Tens minute digit 1,2,3,4,5
  d4 = e - (d3*10);  // calculate Ones minute digit 0,1,2
  } else {
    // e is less than 10
  d3 = 0;
  d4 = e;
  }  

/* Print test results
myGLCD.setFont(SmallFont);
myGLCD.printNumI(d1,10,200); // Print 0
myGLCD.printNumI(d2,40,200); // Print 0
myGLCD.printNumI(d3,70,200); // Print 0
myGLCD.printNumI(d4,100,200); // Print 0
*/


if (h>=12){ // Set 
//  h = h-12; // Work out value
  pm = 1;  // Set PM flag
} 

// *************************************************************************
// Print each digit if it has changed to reduce screen impact/flicker

// Set digit font colour to white

  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(SevenSeg_XXXL_Num);
  
// First Digit
if(((d1 != c1)||(xsetup == true))&&(d1 != 0)){ // Do not print zero in first digit position
    myGLCD.printNumI(d1,10,70); // Printing thisnumber impacts LFH walls so redraw impacted area   
// ---------------- reprint two left wall pillars
    myGLCD.setColor(1, 73, 240);
    
    myGLCD.drawRoundRect(0, 80, 27, 105); 
    myGLCD.drawRoundRect(2, 85, 25, 100); 

    myGLCD.drawRoundRect(0, 140, 27, 165); 
    myGLCD.drawRoundRect(2, 145, 25, 160); 

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

// Reprint the dots that have not been gobbled
    myGLCD.setColor(200,200,200);
// Row 4
if ( dot[32] == 1) {
  myGLCD.fillCircle(42, 80, 2);
} 

// Row 5

if ( dot[34] == 1) {
  myGLCD.fillCircle(42, 100, 2);
}

// Row 6
if ( dot[36] == 1) {
  myGLCD.fillCircle(42, 120, 2);
}

// Row 7
if ( dot[38] == 1) {
  myGLCD.fillCircle(42, 140, 2);
}

// Row 8
if ( dot[40] == 1) {
  myGLCD.fillCircle(42, 160, 2);
}



  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(SevenSeg_XXXL_Num);
  
// Second Digit
if((d2 != c2)||(xsetup == true)){
  myGLCD.printNumI(d2,70,70); // Print 0
}

// Third Digit
if((d3 != c3)||(xsetup == true)){
  myGLCD.printNumI(d3,143,70); // Was 145    
}

// Fourth Digit
if((d4 != c4)||(xsetup == true)){
  myGLCD.printNumI(d4,204,70); // Was 205  
}

if (xsetup == true){
  xsetup = false; // Reset Flag now leaving setup mode
  } 
 // Print PM or AM
 
      myGLCD.setColor(1, 73, 240);
      myGLCD.setBackColor(0, 0, 0);
      myGLCD.setFont(SmallFont);
  if (pm == 0) {
      myGLCD.print("AM", 299, 147); 
   } else {
      myGLCD.print("PM", 299, 147);  
   }

// ----------- Alarm Set on LHS lower pillar
if (alarmstatus == true) { // Print AS on fron screenleft hand side
      myGLCD.print("AS", 7, 147); 
}


  // Round dots

  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.fillCircle(137, 105, 5);
  myGLCD.fillCircle(137, 135, 5);





//--------------------- copy exising time digits to global variables so that these can be used to test which digits change in future

c1 = d1;
c2 = d2;
c3 = d3;
c4 = d4;

}




// ===== initiateGame - Custom Function
void drawscreen() {

  

  //Draw Background lines

      myGLCD.setColor(1, 73, 240);
 
// ---------------- Outside wall

        myGLCD.drawRoundRect(0, 239, 319, 0); 
        myGLCD.drawRoundRect(2, 237, 317, 2); 

// ---------------- Four top spacers and wall pillar
 
        myGLCD.drawRoundRect(35, 35, 60, 45); 
        myGLCD.drawRoundRect(37, 37, 58, 43);

        myGLCD.drawRoundRect(93, 35, 118, 45); 
        myGLCD.drawRoundRect(95, 37, 116, 43);

        myGLCD.drawRoundRect(201, 35, 226, 45); 
        myGLCD.drawRoundRect(203, 37, 224, 43);
        
        myGLCD.drawRoundRect(258, 35, 283, 45); 
        myGLCD.drawRoundRect(260, 37, 281, 43);       

        myGLCD.drawRoundRect(155, 0, 165, 45); 
        myGLCD.drawRoundRect(157, 2, 163, 43); 
 
 
// ---------------- Four bottom spacers and wall pillar

        myGLCD.drawRoundRect(35, 196, 60, 206); 
        myGLCD.drawRoundRect(37, 198, 58, 204);

        myGLCD.drawRoundRect(93, 196, 118, 206); 
        myGLCD.drawRoundRect(95, 198, 116, 204);

        myGLCD.drawRoundRect(201, 196, 226, 206); 
        myGLCD.drawRoundRect(203, 198, 224, 204);
        
        myGLCD.drawRoundRect(258, 196, 283, 206); 
        myGLCD.drawRoundRect(260, 198, 281, 204);          

        myGLCD.drawRoundRect(155, 196, 165, 239); 
        myGLCD.drawRoundRect(157, 198, 163, 237); 

// ---------- Four Door Pillars 

        myGLCD.drawRoundRect(0, 80, 27, 105); 
        myGLCD.drawRoundRect(2, 85, 25, 100); 

        myGLCD.drawRoundRect(0, 140, 27, 165); 
        myGLCD.drawRoundRect(2, 145, 25, 160); 
        
        myGLCD.drawRoundRect(292, 80, 319, 105); 
        myGLCD.drawRoundRect(294, 85, 317, 100); 

        myGLCD.drawRoundRect(292, 140, 319, 165); 
        myGLCD.drawRoundRect(294, 145, 317, 160);  


// ---------------- Clear lines on Outside wall
        myGLCD.setColor(0,0,0);
        myGLCD.drawRoundRect(1, 238, 318, 1);   

        myGLCD.fillRect(0, 106, 2, 139); 
        myGLCD.fillRect(317, 106, 319, 139); 

// Draw Dots
  myGLCD.setColor(200, 200, 200);
  myGLCD.setBackColor(0, 0, 0);


/*
// Row 4
if ( dot[32] == 1) {
  myGLCD.fillCircle(42, 80, 2);
} 
*/



// Row 1
if ( dot[1] == 1) {
  myGLCD.fillCircle(19, 19, 2); // dot 1
  }
if ( dot[2] == 1) {  
  myGLCD.fillCircle(42, 19, 2); // dot 2
  }
if ( dot[3] == 1) {
  myGLCD.fillCircle(65, 19, 2); // dot 3
  }
if ( dot[4] == 1) {
  myGLCD.fillCircle(88, 19, 2); // dot 4
  }
if ( dot[5] == 1) {
  myGLCD.fillCircle(112, 19, 2); // dot 5
  }
if ( dot[6] == 1) {
  myGLCD.fillCircle(136, 19, 2); // dot 6   
  }  
// 
if ( dot[7] == 1) {
  myGLCD.fillCircle(183, 19, 2); // dot 7
  }
if ( dot[8] == 1) {  
  myGLCD.fillCircle(206, 19, 2);  // dot 8 
  }
if ( dot[9] == 1) {  
  myGLCD.fillCircle(229, 19, 2); // dot 9
  }
if ( dot[10] == 1) {  
  myGLCD.fillCircle(252, 19, 2); // dot 10
  }
if ( dot[11] == 1) {  
  myGLCD.fillCircle(275, 19, 2);  // dot 11
  }
if ( dot[12] == 1) {
  myGLCD.fillCircle(298, 19, 2);  // dot 12
  }
// Row 2
if ( dot[13] == 1) {
  myGLCD.fillCircle(19, 40, 7); // Big dot 13
  }
if ( dot[14] == 1) {
  myGLCD.fillCircle(77, 40, 2);  // dot 14
  }
if ( dot[15] == 1) {
  myGLCD.fillCircle(136, 40, 2);  // dot 15
  }
if ( dot[16] == 1) {
  myGLCD.fillCircle(183, 40, 2);  // dot 16
  }
if ( dot[17] == 1) {
  myGLCD.fillCircle(241, 40, 2);  // dot 17
  }
if ( dot[18] == 1) {
  myGLCD.fillCircle(298, 40, 7); // Big dot 18
  }  

  
// Row 3

if ( dot[19] == 1) {
  myGLCD.fillCircle(19, 60, 2);
}
if ( dot[20] == 1) {
  myGLCD.fillCircle(42, 60, 2);
}
if ( dot[21] == 1) {
  myGLCD.fillCircle(65, 60, 2); 
}
if ( dot[22] == 1) {
  myGLCD.fillCircle(88, 60, 2);
}
if ( dot[23] == 1) {
  myGLCD.fillCircle(112, 60, 2);
}
if ( dot[24] == 1) {
  myGLCD.fillCircle(136, 60, 2); 
}
if ( dot[25] == 1) { 
  myGLCD.fillCircle(160, 60, 2);
}
if ( dot[26] == 1) {
  myGLCD.fillCircle(183, 60, 2);
}
if ( dot[27] == 1) {
  myGLCD.fillCircle(206, 60, 2);  
}
if ( dot[28] == 1) {
  myGLCD.fillCircle(229, 60, 2);
}
if ( dot[29] == 1) {
  myGLCD.fillCircle(252, 60, 2);
}
if ( dot[30] == 1) {
  myGLCD.fillCircle(275, 60, 2); 
}
if ( dot[31] == 1) {
  myGLCD.fillCircle(298, 60, 2);   
}

// Row 4
if ( dot[32] == 1) {
  myGLCD.fillCircle(42, 80, 2);
}
if ( dot[33] == 1) {
  myGLCD.fillCircle(275, 80, 2);   
}
// Row 5
if ( dot[34] == 1) {
  myGLCD.fillCircle(42, 100, 2);
}
if ( dot[35] == 1) {
  myGLCD.fillCircle(275, 100, 2);
}
// Row 6
if ( dot[36] == 1) {
  myGLCD.fillCircle(42, 120, 2);
}
if ( dot[37] == 1) {
  myGLCD.fillCircle(275, 120, 2);
}
// Row 7
if ( dot[38] == 1) {
  myGLCD.fillCircle(42, 140, 2);
}
if ( dot[39] == 1) {
  myGLCD.fillCircle(275, 140, 2);
}
// Row 8
if ( dot[40] == 1) {
  myGLCD.fillCircle(42, 160, 2);
}
if ( dot[41] == 1) {
  myGLCD.fillCircle(275, 160, 2);
}
// Row 9
if ( dot[42] == 1) {
  myGLCD.fillCircle(19, 181, 2);
}
if ( dot[43] == 1) {
  myGLCD.fillCircle(42, 181, 2);
}
if ( dot[44] == 1) {
  myGLCD.fillCircle(65, 181, 2); 
}
if ( dot[45] == 1) {
  myGLCD.fillCircle(88, 181, 2);
}
if ( dot[46] == 1) {
  myGLCD.fillCircle(112, 181, 2);
}
if ( dot[47] == 1) {
  myGLCD.fillCircle(136, 181, 2); 
}
if ( dot[48] == 1) { 
  myGLCD.fillCircle(160, 181, 2);
}
if ( dot[49] == 1) {
  myGLCD.fillCircle(183, 181, 2);
}
if ( dot[50] == 1) {
  myGLCD.fillCircle(206, 181, 2);  
}
if ( dot[51] == 1) {
  myGLCD.fillCircle(229, 181, 2);
}
if ( dot[52] == 1) {
  myGLCD.fillCircle(252, 181, 2);
}
if ( dot[53] == 1) {
  myGLCD.fillCircle(275, 181, 2); 
}
if ( dot[54] == 1) {
  myGLCD.fillCircle(298, 181, 2);   
}
// Row 10
if ( dot[55] == 1) {
  myGLCD.fillCircle(19, 201, 7); // Big dot
}
if ( dot[56] == 1) {
  myGLCD.fillCircle(77, 201, 2);
}
if ( dot[57] == 1) {
  myGLCD.fillCircle(136, 201, 2);
}
if ( dot[58] == 1) {
  myGLCD.fillCircle(183, 201, 2);
}
if ( dot[59] == 1) {
  myGLCD.fillCircle(241, 201, 2);
}
if ( dot[60] == 1) {
  myGLCD.fillCircle(298, 201, 7); // Big dot
}  

  

 
  // Row 11
if ( dot[61] == 1) {
  myGLCD.fillCircle(19, 221, 2);
}
if ( dot[62] == 1) {
  myGLCD.fillCircle(42, 221, 2);
}
if ( dot[63] == 1) {
  myGLCD.fillCircle(65, 221, 2); 
}
if ( dot[64] == 1) { 
  myGLCD.fillCircle(88, 221, 2);
}
if ( dot[65] == 1) {
  myGLCD.fillCircle(112, 221, 2);
}
if ( dot[66] == 1) {
  myGLCD.fillCircle(136, 221, 2);   
}  
//  myGLCD.fillCircle(160, 19, 2);

if ( dot[67] == 1) {
  myGLCD.fillCircle(183, 221, 2);
}
if ( dot[68] == 1) {
  myGLCD.fillCircle(206, 221, 2);  
}
if ( dot[69] == 1) {
  myGLCD.fillCircle(229, 221, 2);
}
if ( dot[70] == 1) {
  myGLCD.fillCircle(252, 221, 2);
}
if ( dot[71] == 1) {
  myGLCD.fillCircle(275, 221, 2); 
}
if ( dot[72] == 1) {
  myGLCD.fillCircle(298, 221, 2); 
}


// TempTest delay

// delay(100000);

 }
 
//***************************************************************************************************** 
//====== Draws the Pacman - bitmap
//*****************************************************************************************************
void drawPacman(int x, int y, int p, int d, int pd) {



  // Draws the Pacman - bitmap
//  // Pacman direction d == 0 = right, 1 = down, 2 = left, 3 = up
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(0, 0, 0);

if ( d == 0){ // Right

if (pd == 0){ // Legacy direction Right
  myGLCD.drawRect(x-1, y, x, y+27); // Clear trail off graphic before printing new position
  }
if (pd == 3){ // Legacy direction Up
 myGLCD.drawRect(x+1, y+29, x+28, y+28); // Clear trail off graphic before printing new position
}
if (pd == 1){ // Legacy direction Down
 myGLCD.drawRect(x, y-1, x+28, y); // Clear trail off graphic before printing new position 
}
if (pd == 2){ // Legacy direction Left
 myGLCD.drawRect(x+29, y, x+28, y+28); // Clear trail off graphic before printing new positi 
}

if (p == 0) { 


   if (mspacman == false){
    myGLCD.drawBitmap (x, y, 28, 28, c_pacman); //   Closed Pacman  
   } else {
    myGLCD.drawBitmap (x, y, 28, 28, ms_c_pacman_r); //   Closed Pacman        
   }


 } else if( p == 1) {

   if (mspacman == false){
   myGLCD.drawBitmap (x, y, 28, 28, r_m_pacman); //  Medium open Pacman 
   } else {
   myGLCD.drawBitmap (x, y, 28, 28, ms_r_m_pacman); //  Medium open Pacman       
   }
   
 } else if( p == 2) {

   if (mspacman == false){
   myGLCD.drawBitmap (x, y, 28, 28, r_o_pacman); //  Open Mouth Pacman  
   } else {
   myGLCD.drawBitmap (x, y, 28, 28, ms_r_o_pacman); //  Open Mouth Pacman       
   }
 }
} else  if ( d == 1){ // Down

if (pd == 0){ // Legacy direction Right
  myGLCD.drawRect(x-1, y+6, x, y+22); // Clear trail off graphic before printing new position
  }
if (pd == 3){ // Legacy direction Up
 myGLCD.drawRect(x+6, y+29, x+22, y+28); // Clear trail off graphic before printing new position
}
if (pd == 1){ // Legacy direction Down
 myGLCD.drawRect(x+6, y-1, x+22, y); // Clear trail off graphic before printing new position 
}
if (pd == 2){ // Legacy direction Left
 myGLCD.drawRect(x+29, y+6, x+28, y+22); // Clear trail off graphic before printing new positi 
}

 if (p == 0) { 
   
   if (mspacman == false){
    myGLCD.drawBitmap (x, y, 28, 28, c_pacman); //   Closed Pacman  
   } else {
    myGLCD.drawBitmap (x, y, 28, 28, ms_c_pacman_d); //   Closed Pacman        
   }
    
    
 } else if( p == 1) {

   if (mspacman == false){
   myGLCD.drawBitmap (x, y, 28, 28, d_m_pacman); //  Medium open Pacman   
   } else {
   myGLCD.drawBitmap (x, y, 28, 28, ms_d_m_pacman); //  Medium open Pacman     
   }

 } else if( p == 2) {

   if (mspacman == false){
     myGLCD.drawBitmap (x, y, 28, 28, d_o_pacman); //  Open Mouth Pacman  
   } else {
     myGLCD.drawBitmap (x, y, 28, 28, ms_d_o_pacman); //  Open Mouth Pacman     
   }

 }
} else  if ( d == 2){ // Left

if (pd == 0){ // Legacy direction Right
  myGLCD.drawRect(x-1, y+6, x, y+22); // Clear trail off graphic before printing new position
  }
if (pd == 3){ // Legacy direction Up
 myGLCD.drawRect(x+6, y+29, x+22, y+28); // Clear trail off graphic before printing new position
}
if (pd == 1){ // Legacy direction Down
 myGLCD.drawRect(x+6, y-1, x+22, y); // Clear trail off graphic before printing new position 
}
if (pd == 2){ // Legacy direction Left
 myGLCD.drawRect(x+29, y+6, x+28, y+22); // Clear trail off graphic before printing new positi 
}

 if (p == 0) { 

   if (mspacman == false){
    myGLCD.drawBitmap (x, y, 28, 28, c_pacman); //   Closed Pacman  
   } else {
    myGLCD.drawBitmap (x, y, 28, 28, ms_c_pacman_l); //   Closed Pacman        
   }


 } else if( p == 1) {

   if (mspacman == false){
     myGLCD.drawBitmap (x, y, 28, 28, l_m_pacman); //  Medium open Pacman   
   } else {
     myGLCD.drawBitmap (x, y, 28, 28, ms_l_m_pacman); //  Medium open Pacman   
   }
   
 } else if( p == 2) {
 
   if (mspacman == false){
     myGLCD.drawBitmap (x, y, 28, 28, l_o_pacman); //  Open Mouth Pacman   
   } else {
     myGLCD.drawBitmap (x, y, 28, 28, ms_l_o_pacman); //  Open Mouth Pacman  
   }

 }
} else  if ( d == 3){ // Up

if (pd == 0){ // Legacy direction Right
  myGLCD.drawRect(x-1, y+6, x, y+22); // Clear trail off graphic before printing new position
  }
if (pd == 3){ // Legacy direction Up
 myGLCD.drawRect(x+6, y+29, x+22, y+28); // Clear trail off graphic before printing new position
}
if (pd == 1){ // Legacy direction Down
 myGLCD.drawRect(x+6, y-1, x+22, y); // Clear trail off graphic before printing new position 
}
if (pd == 2){ // Legacy direction Left
 myGLCD.drawRect(x+29, y+6, x+28, y+22); // Clear trail off graphic before printing new positi 
}

 if (p == 0) { 

   if (mspacman == false){
    myGLCD.drawBitmap (x, y, 28, 28, c_pacman); //   Closed Pacman  
   } else {
    myGLCD.drawBitmap (x, y, 28, 28, ms_c_pacman_u); //   Closed Pacman        
   }


 } else if( p == 1) {

   if (mspacman == false){
     myGLCD.drawBitmap (x, y, 28, 28, u_m_pacman); //  Medium open Pacman    
   } else {
     myGLCD.drawBitmap (x, y, 28, 28, ms_u_m_pacman); //  Medium open Pacman   
   }
   

 } else if( p == 2) {

   if (mspacman == false){
     myGLCD.drawBitmap (x, y, 28, 28, u_o_pacman); //  Open Mouth Pacman    
   } else {
     myGLCD.drawBitmap (x, y, 28, 28, ms_u_o_pacman); //  Open Mouth Pacman  
   }
   
 }

}
  
}

//**********************************************************************************************************
//====== Draws the Ghost - bitmap
void drawGhost(int x, int y, int d, int pd) {


  // Draws the Ghost - bitmap
//  // Ghost direction d == 0 = right, 1 = down, 2 = left, 3 = up
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(0, 0, 0);

if ( d == 0){ // Right

if (pd == 0){ // Legacy direction Right
  myGLCD.drawRect(x-1, y+6, x, y+26); // Clear trail off graphic before printing new position
  }
if (pd == 3){ // Legacy direction Up
 myGLCD.drawRect(x, y+29, x+28, y+28); // Clear trail off graphic before printing new position
}
if (pd == 1){ // Legacy direction Down
 myGLCD.drawRect(x+6, y-1, x+22, y); // Clear trail off graphic before printing new position 
}
if (pd == 2){ // Legacy direction Left
 myGLCD.drawRect(x+29, y+6, x+28, y+27); // Clear trail off graphic before printing new positi 
}

  if (fruiteatenpacman == true){ 
    myGLCD.drawBitmap (x, y, 28, 28, bluepacman); //   Closed Ghost 
  } else {
    myGLCD.drawBitmap (x, y, 28, 28, rr_ghost); //   Closed Ghost 
  }
  
} else  if ( d == 1){ // Down

if (pd == 0){ // Legacy direction Right
  myGLCD.drawRect(x-1, y+6, x, y+26); // Clear trail off graphic before printing new position
  }
if (pd == 3){ // Legacy direction Up
 myGLCD.drawRect(x, y+29, x+28, y+28); // Clear trail off graphic before printing new position
}
if (pd == 1){ // Legacy direction Down
 myGLCD.drawRect(x+6, y-1, x+22, y); // Clear trail off graphic before printing new position 
}
if (pd == 2){ // Legacy direction Left
 myGLCD.drawRect(x+29, y+6, x+28, y+27); // Clear trail off graphic before printing new positi 
}

  if (fruiteatenpacman == true){ 
    myGLCD.drawBitmap (x, y, 28, 28, bluepacman); //   Closed Ghost 
  } else {
    myGLCD.drawBitmap (x, y, 28, 28, rd_ghost); //   Closed Ghost 
  }

} else  if ( d == 2){ // Left

if (pd == 0){ // Legacy direction Right
  myGLCD.drawRect(x-1, y+6, x, y+26); // Clear trail off graphic before printing new position
  }
if (pd == 3){ // Legacy direction Up
 myGLCD.drawRect(x, y+29, x+28, y+28); // Clear trail off graphic before printing new position
}
if (pd == 1){ // Legacy direction Down
 myGLCD.drawRect(x+6, y-1, x+22, y); // Clear trail off graphic before printing new position 
}
if (pd == 2){ // Legacy direction Left
 myGLCD.drawRect(x+29, y+6, x+28, y+27); // Clear trail off graphic before printing new positi 
}

  if (fruiteatenpacman == true){ 
    myGLCD.drawBitmap (x, y, 28, 28, bluepacman); //   Closed Ghost 
  } else {
    myGLCD.drawBitmap (x, y, 28, 28, rl_ghost); //   Closed Ghost 
  }

} else  if ( d == 3){ // Up

if (pd == 0){ // Legacy direction Right
  myGLCD.drawRect(x-1, y+6, x, y+26); // Clear trail off graphic before printing new position
  }
if (pd == 3){ // Legacy direction Up
 myGLCD.drawRect(x, y+29, x+28, y+28); // Clear trail off graphic before printing new position
}
if (pd == 1){ // Legacy direction Down
 myGLCD.drawRect(x+6, y-1, x+22, y); // Clear trail off graphic before printing new position 
}
if (pd == 2){ // Legacy direction Left
 myGLCD.drawRect(x+29, y+6, x+28, y+27); // Clear trail off graphic before printing new positi 
}

  if (fruiteatenpacman == true){ 
    myGLCD.drawBitmap (x, y, 28, 28, bluepacman); //   Closed Ghost 
  } else {
    myGLCD.drawBitmap (x, y, 28, 28, ru_ghost); //   Closed Ghost 
  }

}
  
}

//**********************************************************************************************************


 // **********************************
 // ******* Enter Setup Mode *********
 // **********************************
 // Use up down arrows to change time and alrm settings

 void clocksetup(){
 
int timehour = hour();
int timeminute = minute();

// Read Alarm Set Time from Eeprom

  // read a byte from the current address of the EEPROM
  ahour = EEPROM.read(100);
  alarmhour = (int)ahour;
  if (alarmhour >24 ) {
    alarmhour = 0;
  }

  amin = EEPROM.read(101);
  alarmminute = (int)amin;
  if (alarmminute >60 ) {
    alarmminute = 0;
  }


boolean savetimealarm = false; // If save button pushed save the time and alarm

 // Setup Screen
   myGLCD.clrScr();
// ---------------- Outside wall

      myGLCD.setColor(255, 255, 0);
      myGLCD.setBackColor(0, 0, 0);

   myGLCD.drawRoundRect(0, 239, 319, 0); 
   myGLCD.drawRoundRect(2, 237, 317, 2); 
   
//Reset screenpressed flag
screenPressed = false;

// Read in current clock time and Alarm time



  // Setup buttons
    myGLCD.setFont(BigFont);

    // Time Set buttons
    myGLCD.print("+  +", 135, 38); 
    myGLCD.print("-  -", 135, 82);
    myGLCD.drawRoundRect(132, 35, 152, 55); // time hour +
    myGLCD.drawRoundRect(180, 35, 200, 55); // time minute +
    
    myGLCD.drawRoundRect(132, 80, 152, 100); // time hour -
    myGLCD.drawRoundRect(180, 80, 200, 100); // time minute -   

    // Alarm Set buttons
    myGLCD.print("+  +", 135, 138); 
    myGLCD.print("-  -", 135, 182);
    myGLCD.drawRoundRect(132, 135, 152, 155); // alarm hour +
    myGLCD.drawRoundRect(180, 135, 200, 155); // alarm minute +

    myGLCD.drawRoundRect(132, 180, 152, 200);  // alarm hour -
    myGLCD.drawRoundRect(180, 180, 200, 200); // alarm minute -  



    
    myGLCD.print("SAVE", 13, 213);
    myGLCD.print("EXIT", 245, 213);    
    myGLCD.drawRoundRect(10, 210, 80, 230);
    myGLCD.drawRoundRect(243, 210, 310, 230);  

// Get your Ghost on
    myGLCD.drawBitmap (5, 100, 28, 28, rr_ghost); //   Closed Ghost 
    myGLCD.drawBitmap (285, 100, 28, 28, bluepacman); //   Closed Ghost 


// Display MS Pacman or Pacman in menu - push to change
if (mspacman == false) {
    myGLCD.drawBitmap (154, 208, 28, 28, r_o_pacman); //   Closed Ghost 
} else {
    myGLCD.drawBitmap (154, 208, 28, 28, ms_r_o_pacman); //   Closed Ghost  
}
// Begin Loop here

while (xsetup == true){


   if (alarmstatus == true){ // flag where false is off and true is on
    myGLCD.print("SET", 220, 160);
 } else {
    myGLCD.print("OFF", 220, 160);
 }   
    myGLCD.drawRoundRect(218, 157, 268, 177);

// Draw Sound Button

    myGLCD.print("TEST", 50, 110);  // Triggers alarm sound
    myGLCD.drawRoundRect(48, 108, 116, 128);    
    
// Display Current Time
   
    myGLCD.print("Time", 40, 60);    


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
   
    myGLCD.print("Alarm", 40, 160);    


//    myGLCD.printNumI(alarmhour, 130, 160); 
 if(alarmhour>=10){ // Annoyingly if number less than 10 doesnt print on RHS and misses zero so need to compensate
      myGLCD.printNumI(alarmhour, 130, 160);   // If >= 10 just print minute
      } else {
      myGLCD.print("0", 130, 160);
      myGLCD.printNumI(alarmhour, 146, 160);      
      } 



    myGLCD.print(":", 160, 160);       

 if(alarmminute>=10){ // Annoyingly if number less than 10 doesnt print on RHS and misses zero so need to compensate
      myGLCD.printNumI(alarmminute, 175, 160);   // If >= 10 just print minute
      } else {
      myGLCD.print("0", 175, 160);
      myGLCD.printNumI(alarmminute, 193, 160);      
      }    

// Read input to determine if buttons pressed
     myTouch.read();
 if (myTouch.dataAvailable()) {
    xT = myTouch.getX();
    yT = myTouch.getY();        

    // Capture input command from user
    if ((xT>=230) && (xT<=319) && (yT>=200) && (yT<=239)) { // (243, 210, 310, 230)  Exit Button
        xsetup = false; // Exit setupmode   
    } 
    
    else if ((xT>=0) && (xT<=90) && (yT>=200) && (yT<=239)) { // (243, 210, 310, 230)  Save Alarm and Time Button
        savetimealarm = true; // Exit and save time and alarm
        xsetup = false; // Exit setupmode    
      }  
    
    
    else if ((xT>=130) && (xT<=154) && (yT>=12) && (yT<=57)) { // Time Hour +  (132, 35, 152, 55)
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
    
    else if ((xT>=178) && (xT<=202) && (yT>=12) && (yT<=57)) { // Time Minute +  (180, 35, 200, 55)
        timeminute = timeminute + 1; // Increment Hour
        if (timeminute == 60) {  // reset minute to 0 minutes if 60
           timeminute = 0 ;
        }
      } 

    else if ((xT>=178) && (xT<=202) && (yT>=78) && (yT<=102)) { // (180, 80, 200, 100); // time minute - 
        timeminute = timeminute - 1; // Decrement Hour
        if (timeminute == -1) {  // reset minute to 0 minutes if 60
           timeminute = 59 ;
        }
      }       
 
     else if ((xT>=130) && (xT<=154) && (yT>=113) && (yT<=157)) { // (132, 135, 152, 155); // alarm hour +
        alarmhour = alarmhour + 1; // Increment Hour
        if (alarmhour == 24) {  // reset hour to 0 hours if 24
           alarmhour = 0 ;
       
      } 
    } 

    else if ((xT>=130) && (xT<=154) && (yT>=178) && (yT<=202)) { // (132, 180, 152, 200);  // alarm hour -
        alarmhour = alarmhour - 1; // Decrement Hour
        if (alarmhour == -1) {  // reset hour to 23 hours if < 0
           alarmhour = 23 ;
       
      } 
    }
    
    else if ((xT>=178) && (xT<=202) && (yT>=113) && (yT<=157)) { // (180, 135, 200, 155); // alarm minute +
        alarmminute = alarmminute + 1; // Increment Hour
        if (alarmminute == 60) {  // reset minute to 0 minutes if 60
           alarmminute = 0 ;
        }
      } 

    else if ((xT>=178) && (xT<=202) && (yT>=178) && (yT<=202)) { // (180, 180, 200, 200); // alarm minute -
        alarmminute = alarmminute - 1; // Increment Hour
        if (alarmminute == -1) {  // reset minute to 0 minutes if 60
           alarmminute = 59 ;
        }
      }      


    else if ((xT>=154) && (xT<=184) && (yT>=208) && (yT<=238)) { // toggle Pacman code 
         mspacman = !mspacman; // toggle the value
      } 

    else if ((xT>=216) && (xT<=270) && (yT>=155) && (yT<=179)) { // (218, 157, 268, 177); // alarm set button pushed
        if (alarmstatus == true) {  
             alarmstatus = false; // Turn off Alarm
        } else {
            alarmstatus = true; // Turn on Alarm
        }
      }
     else if ((xT>=46) && (xT<=118) && (yT>=106) && (yT<=130)) { // ((48, 108, 116, 128); // alarm test button pushed
        // Set off alarm by toggling D8, recorded sound triggered by LOW to HIGH transition
        digitalWrite(8,HIGH); // Set high
        digitalWrite(8,LOW); // Set low
     }
       
      // Should mean changes should scroll if held down
        delay(250);      

    }  

// Display MS Pacman or Pacman in menu - push to change
if (mspacman == false) {
    myGLCD.drawBitmap (154, 208, 28, 28, r_o_pacman); //   Closed Ghost 
} else {
    myGLCD.drawBitmap (154, 208, 28, 28, ms_r_o_pacman); //   Closed Ghost  
}

}   




if ( savetimealarm == true) {
  // The following codes transmits the data to the RTC
  Wire.beginTransmission(DS1307);
  Wire.write(byte(0));
  Wire.write(decToBcd(0));
  Wire.write(decToBcd(timeminute));
  Wire.write(decToBcd(timehour));
  Wire.write(decToBcd(0));
  Wire.write(decToBcd(0));
  Wire.write(decToBcd(0));
  Wire.write(decToBcd(0));
  Wire.write(byte(0));
  Wire.endTransmission();
  // Ends transmission of data
  
  // Write the Alarm Time to EEPROM so it can be stored when powered off
 
     //alarmhour = (int)ahour;
     ahour = (byte)alarmhour;
     amin = (byte)alarmminute;
     EEPROM.write(100, ahour);
     EEPROM.write(101, amin);   
    
  // Now time and alarm data saved reset flag
  savetimealarm = false;
}


     //* Clear Screen
      myGLCD.setColor(0, 0, 0); 
      myGLCD.setBackColor(0, 0, 0);
      myGLCD.fillRect(0,239,319,0);
     xsetup = true; // Set Flag now leaving setup mode in order to draw Clock Digits 
     setSyncProvider(RTC.get);   // the function to get the time from the RTC
     setSyncInterval(60); // sync the time every 60 seconds (1 minutes)
     drawscreen(); // Initiate the screen
     UpdateDisp(); // update value to clock
 
 }
 
 // ================= Decimal to BCD converter

byte decToBcd(byte val) {
  return ((val/10*16) + (val%10));
} 
