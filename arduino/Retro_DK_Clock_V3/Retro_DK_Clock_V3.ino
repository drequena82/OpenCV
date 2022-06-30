  /*  Arduino DK Clock Project
V3 - Bug fixed where when reachs 95 mX value not reset to start. Reset Jump issue resolved.
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


// Initializes RTC time values: 
const int DS1307 = 0x68; // Address of DS1307 see data sheets

// Display Dimmer Variables
int dimscreen = 255; // This variable is used to drive the screen brightness where 255 is max brightness
int LDR = 100; // LDR variable measured directly from Analog 7

// JumpVariables
int jptr = 0; // Points to value in array
int jumpval[16] = {0, -7, -9, -11, -12, -13, -14, -15, -15, -14, -13, -11, -9, -7, -4, 0}; // Value used to subtract from mY during a jump
boolean jumptrigger = false; // Flag indicates if jump initiated
int jumpY = 0; // Temp storage of mY during jump
int jumpA = 0; // Initial jumping coodrinate before adjustment

// Mario Variables

int mX = 28; 
int mY = 212;
int mimage = 1; // Used to determine which image should be used
int mdirection = 1; // Where Down = 0, Right = 1, Left = 2
int mprevdirection = 1; // Where Down = 0, Right = 1, Left = 2
boolean mtrigger = true; // switch used to trigger release of a barrel 


//Barrel Variables
// Three Barrels are able to be sent down at a time
// Each require their own set of variables and a desicion tree to follow when released
// Monkey waives arms then rolls a barrell down which sets the tempo for all barrels released
int Barreldelay = 5;  // Number of main loop cycles before the Monkey graphic changes and the barrel drop is triggered
byte bd = 0; // Temp variable to make turning decisions
  
int b1X = 42; 
int b1Y = 74;
int b1image = 1; // Used to determine which image should be used
int b1direction = 0; // Where Down = 0, Right = 1, Left = 2
int prevb1direction = 0; // Where Down = 0, Right = 1, Left = 2
boolean triggerbarrel1 = false; // switch used to trigger release of a barrel 


int b2X = 42;
int b2Y = 74;
int b2image = 1; // Used to determine which image should be used
int b2direction = 0; // Where Down = 0, Right = 1, Left = 2
int prevb2direction = 0; // Where Down = 0, Right = 1, Left = 2
boolean triggerbarrel2 = false; // switch used to trigger release of a barrel 

int b3X = 42;
int b3Y = 74;
int b3image = 1; // Used to determine which image should be used
int b3direction = 0; // Where Down = 0, Right = 1, Left = 2
int prevb3direction = 0; // Where Down = 0, Right = 1, Left = 2
boolean triggerbarrel3 = false; // switch used to trigger release of a barrel 

int b4X = 42;
int b4Y = 74;
int b4image = 1; // Used to determine which image should be used
int b4direction = 0; // Where Down = 0, Right = 1, Left = 2
int prevb4direction = 0; // Where Down = 0, Right = 1, Left = 2
boolean triggerbarrel4 = false; // switch used to trigger release of a barrel 

// scoreboard

int mscore = 0; // current mario score
int bscore = 0; // current DK score
int prevmscore = 0; // previous mario score
int prevbscore = 0; // previous DK score


//==== Creating Objects
UTFT    myGLCD(ILI9341_16,38,39,40,41); //Parameters should be adjusted to your Display/Schield model
URTouch  myTouch( 6, 5, 4, 3, 2);

//==== Defining Fonts
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t SevenSeg_XXXL_Num[];

// Define bitmaps


extern unsigned int Barrelr1[0x78]; // Barrel Bitmap 
extern unsigned int Barrelr2[0x78]; // Barrel Bitmap 
extern unsigned int Barrelr3[0x78]; // Barrel Bitmap 
extern unsigned int Barrelr4[0x78]; // Barrel Bitmap 
extern unsigned int Barrelh1[0x96]; // Barrel Bitmap 
extern unsigned int Barrelh2[0x96]; // Barrel Bitmap 


extern unsigned int DK_scaffold[0xB8]; // Scaffold Bitmap 
extern unsigned int Monkey1[0x5A0]; // Monkey1 Bitmap 
extern unsigned int Monkey2[0x5A0]; // Monkey2 Bitmap 
extern unsigned int Monkey3[0x5A0]; // Monkey3 Bitmap 
extern unsigned int Monkeyleft[0x5A0]; // Monkey3 Bitmap 
extern unsigned int MonkeyBarrel[0x5A0]; // MonkeyBarrel Bitmap 
extern unsigned int Barrel1[0xA0]; // Barrel Bitmap 
extern unsigned int Oil1[0x180]; // Oil1 Bitmap 
extern unsigned int Oil2[0x180]; // Oil2 Bitmap 
extern unsigned int Girl1[0x13B]; // Girl1 Bitmap 
extern unsigned int Ladder1[0x96]; // Ladder1 Bitmap 

extern unsigned int MarioL1[0x100]; // 16x16 
extern unsigned int MarioL2[0x100]; // 16x16 
extern unsigned int MarioL3[0x100]; // 16x16 Jump Left
extern unsigned int MarioR1[0x100]; // 16x16 
extern unsigned int MarioR2[0x100]; // 16x16 
extern unsigned int MarioR3[0x100]; // 16x16 Jump Right
extern unsigned int MarioU1[0x100]; // 16x16 Climb Ladder 1
extern unsigned int MarioU2[0x100]; // 16x16 Climb Ladder 2
extern unsigned int MarioU3[0x100]; // 16x16 Climb Ladder 3
extern unsigned int MarioU4[0x100]; // 16x16 Climeb Ladder 4
extern unsigned int MarioStop[0x100]; // 16x16 Mario Stand Still

// Touch screen coordinates
boolean screenPressed = false;
int xT,yT;
int userT = 4; // flag to indicate directional touch on screen
boolean setupscreen = false; // used to access the setup screen

//Alarm setup variables
boolean xsetup = false; // Flag to determine if existing setup mode

// Monkey Character Graphic number

int monkeygraphic = 0;// Used to track the change in graphics

// Animation delay to slow movement down
int dly = 15; // Orignally 30

// Time Refresh counter 
int rfcvalue = 900; // wait this long untiul check time for changes
int rfc = 1;


// Declare global variables for previous time,  to enable refesh of only digits that have changed
// There are four digits that bneed to be drawn independently to ensure consisitent positioning of time
  int c1 = 20;  // Tens hour digit
  int c2 = 20;  // Ones hour digit
  int c3 = 20;  // Tens minute digit
  int c4 = 20;  // Ones minute digit


void setup() {

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

// Randomseed will shuffle the random function
randomSeed(analogRead(0));

// Setup Alarm enable pin to play back sound on the ISD1820 board
   pinMode(8, OUTPUT); // D8 used to toggle sound
   digitalWrite(8,LOW);  // Set to low to turn off sound

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
  myGLCD.printNumI(LDR,250,60);
*/

if (LDR >=121){
    dimscreen = 255;
   } 
   
if (LDR <=120)   {  
    dimscreen = 35;
   }    
   
analogWrite(9, dimscreen); // Controls brightness 0 is Dark, Ambient room is approx 25 and 70 is direct sunlight 
  
// Print Score
if ((prevbscore != bscore) || (prevmscore != mscore)) {

  if ((bscore > 95) || ( mscore > 95)){
    myGLCD.setColor(237, 28, 36);
    myGLCD.setBackColor(0, 0, 0);

    myGLCD.print("DK",5,133); //  
    myGLCD.printNumI(bscore,20,133,3); //  

    myGLCD.print("M",285,168); //  
    myGLCD.printNumI(mscore,296,168,3); // 

    delay(3000);  // Delay then reset the game
    bscore = 0;
    mscore = 0;
    myGLCD.fillRect(mX-1, mY+jumpA-1, mX+16, mY+jumpA+16); // Clear trail off graphic before printing new position  16x16

              mX = 28;
              mY = 212;
              jptr = 0;
              jumpY = 0;
              mdirection = 1;
              mprevdirection = 1;
              jumptrigger = false;
  
              b1X = 42; 
              b1Y = 74;
              b1image = 1; // Used to determine which image should be used
              b1direction = 0; // Where Down = 0, Right = 1, Left = 2
              prevb1direction = 0; // Where Down = 0, Right = 1, Left = 2
              triggerbarrel1 = false; // switch used to trigger release of a barrel 

              b2X = 42; 
              b2Y = 74;
              b2image = 1; // Used to determine which image should be used
              b2direction = 0; // Where Down = 0, Right = 1, Left = 2
              prevb2direction = 0; // Where Down = 0, Right = 1, Left = 2
              triggerbarrel2 = false; // switch used to trigger release of a barrel 

           
              b3X = 42; 
              b3Y = 74;
              b3image = 1; // Used to determine which image should be used
              b3direction = 0; // Where Down = 0, Right = 1, Left = 2
              prevb3direction = 0; // Where Down = 0, Right = 1, Left = 2
              triggerbarrel3 = false; // switch used to trigger release of a barrel               


              b4X = 42; 
              b4Y = 74;
              b4image = 1; // Used to determine which image should be used
              b4direction = 0; // Where Down = 0, Right = 1, Left = 2
              prevb4direction = 0; // Where Down = 0, Right = 1, Left = 2
              triggerbarrel4 = false; // switch used to trigger release of a barrel 
  
              myGLCD.clrScr();
 
              c1 = 20;
              c2 = 20;
              c3 = 20;
              c4 = 20;
 
              drawscreen(); // Initiate the game
              UpdateDisp(); // update value to clock 
 
    } 

    myGLCD.setColor(237, 28, 36);
    myGLCD.setBackColor(0, 0, 0);

    myGLCD.print("DK",5,133); //  
    myGLCD.printNumI(bscore,20,133,3); //  

    myGLCD.print("M",285,168); //  
    myGLCD.printNumI(mscore,296,168,3); // 
}
  
// Read the current date and time from the RTC and reset board
rfc++;
  if (rfc >= rfcvalue) { // count cycles and print time
    UpdateDisp(); // update value to clock then ...
     dly = 15; // reset delay
     rfc = 0;
     
  }

//***********************************************************************************************
// Mario Logic
// **********************************************************************************************
/*
int mX = 30; 
int mY = 230;
int mimage = 1; // Used to determine which image should be used
int mdirection = 0; // Where Down = 0, Right = 1, Left = 2
int mprevdirection = 0; // Where Down = 0, Right = 1, Left = 2
boolean mtrigger = tru; // switch used to trigger release of a mario

**************************************************************************************************

Mario Movements
1 - Right as expected left & right of touch screen
2 - Left as expected left of touch screen
3 - Jump straight Up = Top of touch screen
4 - Right Jump = Top RHS of touch screem
5 - Left Jump = Top LHS of touch screen 
6 - Climb Down = When at top of ladder Bottom of touch screen
7 - Climb Up = When at bottom of ladder Top of touch screen
8 - Stop = Stand upright when touch Bottom of touch screen

Use Image Number to Toggle the correct image based on Direction
Approach
Divide the screen into rows and columns. Build a decision based on each row and then 

*/


// Assess Jump Position

/*
// JumpVariables
int jptr = 0; // Points to value in array
int jumpval[14]

*/
if (jumptrigger == true){ 

  jumpA = jumpval[jptr]; // Current position of graphic to be erased
  jptr = jptr + 1;
  jumpY = jumpval[jptr]; //future position of graphic 


  myGLCD.setColor(0, 0, 0); 
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.fillRect(mX-1, mY+jumpA-1, mX+16, mY+jumpA+16); // Clear trail off graphic before printing new position  16x16

  if (jumpval[jptr] == 0){ // End of Jump
    // Reset flags
    jumptrigger = false;
    jptr = 0;
    jumpY = 0;
    // Clear up damage to background
    redraw(mX, mY, mdirection);
    
  }
}

// Capture previous score before changing to avoid priting scoreboard each cycle
prevmscore = mscore;
prevbscore = bscore;

// ************************************************************************************************
// Check for collision and reset Mario if so
// ************************************************************************************************

if ((b1X - mX >= 4) && (b1X - mX <= 12) && (b1Y - (mY+jumpA) <= 10) && (b1Y - (mY+jumpA) >= 0)){
  myGLCD.setColor(0, 0, 0); 
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.fillRect(mX-1, mY+jumpA-1, mX+16, mY+jumpA+16); // clear mario 
  // incremebnt monkey score
  bscore = bscore + 1;  
  // Clear up damage to background
   redraw(mX, mY, mdirection);
  // reset coorinates
  mX = 28;
  mY = 212;
  jptr = 0;
  jumpY = 0;
  mdirection = 1;
  mprevdirection = 1;
  jumptrigger = false;
}

if ((b2X - mX >= 4) && (b2X - mX <= 12) && (b2Y - (mY+jumpA) <= 10)&& (b2Y - (mY+jumpA) >= 0)){
  myGLCD.setColor(0, 0, 0); 
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.fillRect(mX-1, mY+jumpA-1, mX+16, mY+jumpA+16); // clear mario 
  // incremebnt monkey score
  bscore = bscore + 1;  
  // Clear up damage to background
   redraw(mX, mY, mdirection);
  // reset coorinates
  mX = 28;
  mY = 212;
  jptr = 0;
  jumpY = 0;
  mdirection = 1;
  mprevdirection = 1;
  jumptrigger = false;  
}
if ((b3X - mX >= 4) && (b3X - mX <= 12) && (b3Y - (mY+jumpA) <= 10) && (b3Y - (mY+jumpA) >= 0)){
  myGLCD.setColor(0, 0, 0); 
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.fillRect(mX-1, mY+jumpA-1, mX+16, mY+jumpA+16); // clear mario 

  // incremebnt monkey score
  bscore = bscore + 1;  
  // Clear up damage to background
   redraw(mX, mY, mdirection);
  // reset coorinates
  mX = 28;
  mY = 212;
  jptr = 0;
  jumpY = 0;
  mdirection = 1;
  mprevdirection = 1;
  jumptrigger = false;  
}
if ((b4X - mX >= 4) && (b4X - mX <= 12) && (b4Y - (mY+jumpA) <= 10) && (b4Y - (mY+jumpA) >= 0)){
  myGLCD.setColor(0, 0, 0); 
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.fillRect(mX-1, mY+jumpA-1, mX+16, mY+jumpA+16); // clear mario 
  // incremebnt monkey score
  bscore = bscore + 1;  
  // Clear up damage to background
   redraw(mX, mY, mdirection);
  // reset coorinates
  mX = 28;
  mY = 212;
  jptr = 0;
  jumpY = 0;
  mdirection = 1;
  mprevdirection = 1;
  jumptrigger = false;
}

// ************************************************************************************************
// Mario Scoring for jumping Barrell

if (jumptrigger == true){ //If during  a Jump over a Barrel if there is not a collision then increment the Mario Points counter by 5

  if ((b1X - mX >= 4) && (b1X - mX <= 8) && (b1Y - (mY+jumpA) <= 40) && (b1Y - (mY+jumpA) >= 0)){
    mscore = mscore + 5;
  }
  if ((b2X - mX >= 4) && (b2X - mX <= 8) && (b2Y - (mY+jumpA) <= 40)&& (b2Y - (mY+jumpA) >= 0)){
    mscore = mscore + 5;
  }
  if ((b3X - mX >= 4) && (b3X - mX <= 8) && (b3Y - (mY+jumpA) <= 40) && (b3Y - (mY+jumpA) >= 0)){
    mscore = mscore + 5; 
  }
  if ((b4X - mX >= 4) && (b4X - mX <= 8) && (b4Y - (mY+jumpA) <= 40) && (b4Y - (mY+jumpA) >= 0)){
    mscore = mscore + 5;
  }

}

// ************************************************************************************************



// Capture previous direction information
mprevdirection = mdirection;

if ((mtrigger == true)) {// Only draw if Mario in play
  
  drawMario(mX, mY+jumpY, mimage, mdirection, mprevdirection);  // Draw mario1`

/*
//Test print of Mario X/Y coordinates
  myGLCD.setColor(1, 73, 240);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(SmallFont);
  myGLCD.printNumI(mX, 150, 200,3);
  myGLCD.printNumI(mY, 150, 220,3);
*/


  // Right Mario ##################################################################################
  if ( mdirection == 1) { 

    mX = mX + 2 ;  // First move mario down 2 pixels

    // Centralise Y function before decision logic 

       if ((mX >=160)&&(mY >= 205)) { // Validate actually on RHS half of level 1 then apply formulae for gradient
           mY = (((298 - mX)/23)+206);  // Level 1 mY algorithm for RHS of platform
       } else     
       if ((mX <= 159)&&(mY >= 205)) { // Validate actually on LHS half of level 1 
           mY = 212;  // Level 1 mY algorithm for LHS of platform 
       } else
       if ((mY >= 173) && (mY <= 185)){
           mY = (((mX)/23)+173); // Level 2 mY Algorithm 
       } else
       if ((mY >= 138) && (mY <= 150)){
           mY = (((319 - mX)/23)+138);  // Level 3 mY algorithm            
       } else       
       if ((mY >= 105) && (mY <= 120)){
           mY = (((mX)/23)+105);  // Level 4 mY Algorithm
       }        

   // For Right movement use these images sequentialy
  if (mprevdirection != mdirection){  // If change in direction reset image sequence
    mimage = 1;
    } else { 
      mimage++; 
    }
  if ( mimage >= 4) {  // Max 3 images 
      mimage = 1;
    } 
    
    // **************************************************    
    //Level 1 - One Ladder, One end stop
    // **************************************************
       if ((mX >=160)&&(mY >= 205)) { // Validate actually on RHS half of level 1 then apply formulae for gradient
//         mY = (((298 - mX)/23)+206);  // Level 1 mY algorithm for RHS of platform

         // RHS wall Level 1
          if (mX >= 300){   // Make a decision on direction       
            mdirection = 2; // Left             
           }  else
          if (mX == 278){   // Ladder 6   Level 1  
            bd = random(2);
            if ( bd == 1){ // Randomise selection
              mdirection = 1; // Right
            } else { 
              if (jumptrigger == false){
                mdirection = 7; // Up
              } else { 
                mdirection = 1; // Right
              }
            }
          }           
       } else 
    
        
    // **************************************************
    //Level 2 - Three ladders and one end stop
    // **************************************************
    if ((mY >= 173) && (mY <= 185)){

//        mY = (((mX)/23)+173); // Level 2 mY Algorithm 
      
      // Ladder 4 Level 2
      if (mX == 30){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 1; // Right
          } else { 
              if (jumptrigger == false){
                mdirection = 7; // Up
              } else { 
                mdirection = 1; // Right
              }
          }        
      } else      
    // Ladder 5 Level 2
    if (mX == 132){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 1; // Right
          } else { 
              if (jumptrigger == false){
                mdirection = 7; // Up
              } else { 
                mdirection = 1; // Right
              }
          }        
      } else      
    // Top of Ladder 6 Level 1
      if (mX == 278){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 1; // Right
          } else { 
              if (jumptrigger == false){
                mdirection = 6; // Down
              } else { 
                mdirection = 1; // Right
              }
            }        
      } else      
     // RHS end of scaffold Level 2
       if (mX == 282){   // Make a decision on direction       
          mdirection = 2; // Left  
         }    
                 
      
    } else
    // **************************************************
    //Level 3 - Four Ladders and one end stop
    // **************************************************
    if ((mY >= 138) && (mY <= 150)){

 //       mY = (((319 - mX)/23)+138);    // Level 3 mY algorithm
      
      // Top Ladder 4 Level 3
      if (mX == 30){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 1; // Right
          } else { 
              if (jumptrigger == false){
                mdirection = 6; // Down
              } else { 
                mdirection = 1; // Right
              }
            }        
      } else      
    // Top Ladder 5 Level 3
    if (mX == 132){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 1; // Right
          } else { 
              if (jumptrigger == false){
                mdirection = 6; // Down
              } else { 
                mdirection = 1; // Right
              }
          }        
      } else      
    // Bottom Ladder 2 Level 3
      if (mX == 148){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 1; // Right
          } else { 
              if (jumptrigger == false){
                mdirection = 7; // Up
              } else { 
                mdirection = 1; // Right
              }
          }        
      } else      
    // Bottom Ladder 3 Level 3
      if (mX == 278){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 1; // Right
          } else { 
              if (jumptrigger == false){
                mdirection = 7; // Up
              } else { 
                mdirection = 1; // Right
              }
          }        
      } else   
     // RHS end of scaffold Level 3
       if (mX == 302){   // Make a decision on direction       
          mdirection = 2; // Left  
         }    
      

    } else   
    // **************************************************
    //Level 4 - Three ladders and one end stop
    // **************************************************
    if ((mY >= 105) && (mY <= 120)){

//        mY = (((mX)/23)+106);  // Level 4 mY Algorithm      
      
      // Bottom Ladder 1 Level 4
      if (mX == 12){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 1; // Right
          } else { 
              if (jumptrigger == false){
                mdirection = 7; // Up
              } else { 
                mdirection = 1; // Right
              }
          }        
      } else      
    // Top of Ladder 2 Level 4
    if (mX == 148){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 1; // Right
          } else { 
              if (jumptrigger == false){
                mdirection = 6; // Down
              } else { 
                mdirection = 1; // Right
              }
          }        
      } else      
    // Top Ladder 3 Level 4
      if (mX == 278){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 1; // Right
          } else { 
              if (jumptrigger == false){
                mdirection = 6; // Down
              } else { 
                mdirection = 1; // Right
              }
          }        
      } else      
     // RHS end of scaffold Level 4
       if (mX >= 282){   // Make a decision on direction       
          mdirection = 2; // Left  
         }    
    }  

  } else
  // Left Mario  ##################################################################################
  if ( mdirection == 2) {  

  mX = mX - 2 ;  // First move mario left 2 pixels

    // Centralise Y function before decision logic 

       if ((mX >=160)&&(mY >= 205)) { // Validate actually on RHS half of level 1 then apply formulae for gradient
           mY = (((298 - mX)/23)+206);  // Level 1 mY algorithm for RHS of platform
       } else     
       if ((mX <= 159)&&(mY >= 205)) { // Validate actually on LHS half of level 1 
           mY = 212;  // Level 1 mY algorithm for LHS of platform 
       } else
       if ((mY >= 173) && (mY <= 185)){
           mY = (((mX)/23)+173); // Level 2 mY Algorithm 
       } else
       if ((mY >= 138) && (mY <= 150)){
           mY = (((319 - mX)/23)+138);  // Level 3 mY algorithm            
       } else       
       if ((mY >= 105) && (mY <= 120)){
           mY = (((mX)/23)+105);  // Level 4 mY Algorithm
       } 


   // For Left movement use these images sequentialy
  if (mprevdirection != mdirection){  // If change in direction reset image sequence
    mimage = 1;
    } else { 
      mimage++; 
    }
  if ( mimage >= 4) {  // Max 3 images 
      mimage = 1;
    } 
   
   
    
     // **************************************************    
    //Level 1 - One Ladder, One end stop
    // **************************************************

       if ((mX >=160)&&(mY >= 205)) { // Validate actually on RHS half of level 1 then apply formulae for gradient
//         mY = (((298 - mX)/23)+206);  // Level 1 mY algorithm for RHS of platform
 
          if (mX == 278){   // Bottom of Ladder 6   Level 1  
            bd = random(2);
            if ( bd == 1){ // Randomise selection
              mdirection = 2; // Left
            } else { 
              if (jumptrigger == false){
                mdirection = 7; // Up
              } else { 
                mdirection = 2; // Left
              }
           } 
          }
       } else   
       if ((mX <= 159)&&(mY >= 205)) { // Validate actually on LHS half of level 1 
//         mY = 212;  // Level 1 mY algorithm for LHS of platform     
         // RHS wall Level 1
         if (mX <= 22){   // Make a decision on direction       
            mdirection = 1; // Right             
         }        
      }    
    // **************************************************
    //Level 2 - Three ladders and one end stop
    // **************************************************
    if ((mY >= 173) && (mY <= 185)){

//       mY = (((mX)/23)+173); // Level 2 mY Algorithm  
      
      // Ladder 4 Level 2
      if (mX == 31){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 2; // Left
          } else { 
              if (jumptrigger == false){
                mdirection = 7; // Up
              } else { 
                mdirection = 2; // Left
              }
          }        
      } else      
    // Ladder 5 Level 2
    if (mX == 132){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 2; // Left
          } else { 
              if (jumptrigger == false){
                mdirection = 7; // Up
              } else { 
                mdirection = 2; // Left
              }
          }        
      } else      
    // Top of Ladder 6 Level 1
      if (mX == 278){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 2; // Left
          } else { 
              if (jumptrigger == false){
                mdirection = 6; // Down
              } else { 
                mdirection = 2; // Left
              }
          }        
      } else      
     // LHS end of scaffold Level 2
       if (mX == 2){   // Make a decision on direction       
          mdirection = 1; // Right  
         }    
                 
      
    } else
    // **************************************************
    //Level 3 - Four Ladders and one end stop
    // **************************************************
    if ((mY >= 138) && (mY <= 150)){
      

//        mY = (((319 - mX)/23)+138);    // Level 3 mY algorithm

      // Top Ladder 4 Level 3
      if (mX == 30){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 2; // Left
          } else { 
              if (jumptrigger == false){
                mdirection = 6; // Down
              } else { 
                mdirection = 2; // Left
              }
          }        
      } else      
    // Top Ladder 5 Level 3
    if (mX == 132){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 2; // Left
          } else { 
              if (jumptrigger == false){
                mdirection = 6; // Down
              } else { 
                mdirection = 2; // Left
              }
          }        
      } else      
    // Bottom Ladder 2 Level 3
      if (mX == 148){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 2; // Left
          } else { 
              if (jumptrigger == false){
                mdirection = 7; // Up
              } else { 
                mdirection = 2; // Left
              }
          }        
      } else      
    // Bottom Ladder 3 Level 3
      if (mX == 278){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 2; // Left
          } else { 
              if (jumptrigger == false){
                mdirection = 7; // Up
              } else { 
                mdirection = 2; // Left
              }
          }        
      } else   
     // LHS end of scaffold Level 3
       if (mX == 22){   // Make a decision on direction       
          mdirection = 1; // Right  
         }    
      

    } else   
    // **************************************************
    //Level 4 - Three ladders and one end stop
    // **************************************************
    if ((mY >= 105) && (mY <= 116)){

//        mY = (((mX)/23)+106);  // Level 4 mY Algorithm           
      
      // Bottom Ladder 1 Level 4
      if (mX == 12){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 2; // Left
          } else { 
              if (jumptrigger == false){
                mdirection = 7; // Up
              } else { 
                mdirection = 2; // Left
              }
          }        
      } else      
    // Top of Ladder 2 Level 4
    if (mX == 148){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 2; // Left
          } else { 
              if (jumptrigger == false){
                mdirection = 6; // Down
              } else { 
                mdirection = 2; // Left
              }
          }        
      } else      
    // Top Ladder 3 Level 4
      if (mX == 278){   // Make a decision on direction     
        bd = random(2);
        if ( bd == 1){ // Randomise selection
            mdirection = 2; // Left
          } else { 
              if (jumptrigger == false){
                mdirection = 6; // Down
              } else { 
                mdirection = 2; // Left
              }
            }        
      } else      
     // LHS end of scaffold Level 4
       if (mX == 2){   // Make a decision on direction       
          mdirection = 1; // Right  
         }    
    }  
 
 
    
  } else
  // Down Mario ##################################################################################
  if ( mdirection == 6) { 

  mY = mY + 2 ;  // First move mario down 2 pixels
 
   // For Down movement use these images sequentialy
  if (mprevdirection != mdirection){  // If change in direction reset image sequence
    mimage = 1;
    } else { 
      mimage++; 
    }
  if ( mimage >= 5) {  // Max 4 images 
      mimage = 1;
    } 

  // Increment mario image needs to be included in the directional logic due to complexity

     if (mX == 12){ // Ladder 1
        if (mY >= 106){ // mario hits L3
          bd = random(2);
          if ( bd == 1){ // Randomise selection
            mdirection = 1; 
          } else { 
            mdirection = 2;
          }
        }
     } else

     if (mX == 148){ // Ladder 2
        if (mY >= 145){ // mario hits L3
          bd = random(2);
          if ( bd == 1){ // Randomise selection
            mdirection = 1; 
          } else { 
            mdirection = 2;
          }
        }
     } else
        if ((mX == 278)&&(mY <= 158)){ // Ladder 3
          if (mY >= 138){ // mario hits L3
            mdirection = 2; // Only option here is left
          }
    } else 

        if (mX <= 30){ // Ladder 4
          if (mY >= 173){ // mario hits L2
            mdirection = 1; // Only option here is Right
          }
    } else 
        if (mX <= 132){ // Ladder 5
          if (mY >= 179){ // mario hits L2
            mdirection = 1; // Only option here is Right
          }
    } else 
        if ((mX == 278)&&(mY >= 160)){ // Ladder 6
          if (mY >= 209){ // mario hits L1
            mdirection = 2; // Only option here is left
          }  
        }

  } else
  // Up Mario  ##################################################################################
  if ( mdirection == 7) { 
    
   mY = mY - 2 ;  // First move mario down 2 pixels
 
   // For Up movement use these images sequentialy
  if (mprevdirection != mdirection){  // If change in direction reset image sequence
    mimage = 1;
    } else { 
      mimage++; 
    }
  if ( mimage >= 5) {  // Max 4 images 
      mimage = 1;
    } 
    
  // Increment mario image needs to be included in the directional logic due to complexity

     if (mX == 12){ // Ladder 1
        if (mY <= 84){ // mario hits L3
  //          mdirection = 6;  // Down    
  // Reset Mario to beginning of game and award 30 points

              // Quick Pause
              delay(1000);
              // Clear Mario
               myGLCD.setColor(0, 0, 0); 
               myGLCD.setBackColor(0, 0, 0);
               myGLCD.fillRect(mX-1, mY+jumpA-1, mX+16, mY+jumpA+16); // clear mario 

              // Redraw Ladder
                  myGLCD.drawBitmap (15, 84, 10, 15, Ladder1); 
                  myGLCD.drawBitmap (15, 96, 10, 15, Ladder1); 
                  myGLCD.drawBitmap (22, 76, 23, 8, DK_scaffold); //   Scaffold
                  myGLCD.drawBitmap (0, 76, 23, 8, DK_scaffold); //   Scaffold


              // reset coorinates
              mX = 28;
              mY = 212;
              mdirection = 1;
              mprevdirection = 1;
              jumptrigger = false;
              mscore = mscore + 30;
  
        }
     } else

     if (mX == 148){ // Ladder 2
        if (mY <= 110){ // mario hits L3
          bd = random(2);
          if ( bd == 1){ // Randomise selection
            mdirection = 1; 
          } else { 
            mdirection = 2;
          }
        }
     } else
        if ((mX == 278)&&(mY <= 158)){ // Ladder 3
          if (mY <= 115){ // mario hits L3
            bd = random(2);
            if ( bd == 1){ // Randomise selection
              mdirection = 1; 
            } else { 
              mdirection = 2;
            }
          }
    } else 

        if (mX <= 30){ // Ladder 4
          if (mY <= 150){ // mario hits L2
            mdirection = 1; // Only option here is Right
          }
    } else 
        if (mX <= 132){ // Ladder 5
          if (mY <= 144){ // mario hits L2
            bd = random(2);
            if ( bd == 1){ // Randomise selection
              mdirection = 1; 
            } else { 
              mdirection = 2;
            }
          }
    } else 
        if ((mX == 278)&&(mY >= 160)){ // Ladder 6
          if (mY <= 184){ // mario hits L1
            mdirection = 2; // Only option here is left
          }  
        }
  }
}

// **********************************************************************************************


// **********************************************************************************************
// Barrel 1 Logic
// **********************************************************************************************

/*
// Test print out

      myGLCD.setColor(1, 73, 240);
      myGLCD.setBackColor(0, 0, 0);
      myGLCD.setFont(SmallFont);

      myGLCD.printNumI(b1X, 150, 200,3);
      myGLCD.printNumI(b1Y, 150, 220,3);
*/

if ((triggerbarrel1 == true)) {// Only draw if Barrel in play
  
  drawbarrel(b1X, b1Y, b1image, b1direction, prevb1direction);  // Draw Barrel1

  // Increment barrel image 
  b1image++;
  if (b1image == 5) {
    b1image = 1;
  }

  // Capture previous direction to enable trail blanking
  prevb1direction = b1direction; 

  // Direction and Movement Logic 

  /*
    // Past first block decide to continue or go down
    if (xP == 62) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

  */
  if (b1direction == 0){ // Barrel Down direction   ***************************************************
  b1Y = b1Y + 2 ;  // First move Barrel down 2 pixels

     if ((b1X == 42)&&(b1Y<=120)){ // Monkey drop L5 (additional test for level) #1
        if (b1Y >= 112){ // Barrel hits L4
          b1direction = 1; // Only option here is right
        }
     } else
        if (b1X == 148){ // Middle Ladder L4 #2
          if (b1Y >= 151){ // Barrel hits L3
            b1direction = 2; // Only option here is left
          }
     } else
        if ((b1X == 278)&&(b1Y <= 160)){ // RHS Ladder L4 #3
          if (b1Y >= 146){ // Barrel hits L3
            b1direction = 2; // Only option here is left
          }
    } else 
        if ((b1X == 298)&&(b1Y<=160)){ // RHS  L4 drop #4
          if (b1Y >= 145){ // Barrel hits L3
            b1direction = 2; // Only option here is left
          }
    } else 

        if ((b1X == 30)&&(b1Y>=123)){ // LHS Ladder L3 #5
          if (b1Y >= 181){ // Barrel hits L2
            b1direction = 1; // Only option here is Right
          }
    } else 

        if (b1X == 4){ // LHS Ladder L3 #6
          if (b1Y >= 179){ // Barrel hits L2
            b1direction = 1; // Only option here is Right
          }

    } else 
        if (b1X <= 132){ // Middle Ladder L3 #7
          if (b1Y >= 184){ // Barrel hits L2
            b1direction = 1; // Only option here is Right
          }
    } else 
        if ((b1X == 278)&&(b1Y >= 160)){ // RHS Ladder L2 #8
          if (b1Y >= 215){ // Barrel hits L1
            b1direction = 2; // Only option here is left
          }
    } else
        if ((b1X == 298)&&(b1Y>=160)){ // RHS drop from L2 #9
          if (b1Y >= 212){ // Barrel hits L1
            b1direction = 2; // Only option here is left
          }    
        }
   } else
  if (b1direction == 1){ // Barrel Right direction  ***************************************************
    b1X = b1X + 2 ;  // First move Barrel down 2 pixels

   // Apply drop relative to position on levels 4 then 2
   // Level 4
   if ((b1Y <= 140)) { // Validate actually on level 4 then apply formulae for gradient
      b1Y = (((b1X - 30)/23)+112); // Apply drop

      // Now decide on direction at three points on level 4
      if (b1X == 148) { // Level 4 middle ladder decision
        b1direction = random(2);
      } else
      if (b1X == 278) { // Level 4 RHS ladder decision
        b1direction = random(2);
      } else
       if (b1X == 298) { // Level 4 end of scaffold
        b1direction = 0; // Only option down
      }     
      
      
   }else    // Level 2
   if (((b1Y >= 170)&&(b1Y <= 205))) { // Validate actually on level 2 then apply formulae for gradient
      b1Y = (((b1X - 30)/23)+179); // Apply drop
 
       // Now decide on direction at two points on level 2
      if (b1X == 278) { // Level 4 RHS ladder decision
        b1direction = random(2);
      } else
       if (b1X == 298) { // Level 4 end of scaffold
        b1direction = 0; // Only option down
      }
 
   }     
   
 
  } else
  if (b1direction == 2){ // Barrel Left direction **********************************************************
    b1X = b1X - 2 ;  // First move Barrel left 2 pixels


   // Level 3
   if (((b1Y <= 170)&&(b1Y >= 140))) { // Validate actually on level 3 then apply formulae for gradient
      b1Y = (((298 - b1X)/23)+144);
 
       // Now decide on direction at three points on level 3
      if (b1X == 132) { // Level 3 middle ladder decision
        bd = random(2);
        if ( bd == 1){ // Randomise selection
          b1direction = 0; 
          } else { 
          b1direction = 2;
        }   
      } else
      if (b1X == 30) { // Level 3 LHS ladder decision
        bd = random(2);
        if ( bd == 1){ // Randomise selection
          b1direction = 0; 
          } else { 
          b1direction = 2;
          }
        } else
       if (b1X == 4) { // Level 3 end of scaffold
        b1direction = 0; // Only option down
      }     
     
   } else   // Level 1 
   if (((b1X >=160)&&(b1X <= 298))&&(b1Y >= 205)) { // Validate actually on RHS half of level 1 then apply formulae for gradient
       b1Y = (((298 - b1X)/23)+212);
       } // else    
   if (((b1X >=20)&&(b1X <= 159))&&(b1Y >= 205)) { // Validate actually on LHS half of level 1 
       b1Y = 218;  // Flat finish for Barrell
   
       if (b1X <= 22) { // Turn off the Barrell and blank graphic
           triggerbarrel1 = false;
           b1image = 1; // Used to determine which image should be used
           b1direction = 0; // Where Down = 0, Right = 1, Left = 2
           prevb1direction = 0;
           myGLCD.fillRect(b1X, b1Y, b1X+14, b1Y+10); // Clear Barrel       
           b1X = 42;
           b1Y = 74;
           }
       }

  }
}





// **********************************************************************************************
// Barrel 2 Logic
// **********************************************************************************************

/*
// Test print out

      myGLCD.setColor(1, 73, 240);
      myGLCD.setBackColor(0, 0, 0);
      myGLCD.setFont(SmallFont);

      myGLCD.printNumI(b2X, 150, 200,3);
      myGLCD.printNumI(b2Y, 150, 220,3);
*/

if ((triggerbarrel2 == true)) {// Only draw if Barrel in play
  
  drawbarrel(b2X, b2Y, b2image, b2direction, prevb2direction);  // Draw Barrel1

  // Increment barrel image 
  b2image++;
  if (b2image == 5) {
    b2image = 1;
  }

  // Capture previous direction to enable trail blanking
  prevb2direction = b2direction; 

  // Direction and Movement Logic 

  /*
    // Past first block decide to continue or go down
    if (xP == 62) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

  */
  if (b2direction == 0){ // Barrel Down direction   ***************************************************
  b2Y = b2Y + 2 ;  // First move Barrel down 2 pixels

     if ((b2X == 42)&&(b2Y<=120)){ // Monkey drop L5 (additional test for level) #1
        if (b2Y >= 112){ // Barrel hits L4
          b2direction = 1; // Only option here is right
        }
     } else
        if (b2X == 148){ // Middle Ladder L4 #2
          if (b2Y >= 151){ // Barrel hits L3
            b2direction = 2; // Only option here is left
          }
     } else
        if ((b2X == 278)&&(b2Y <= 160)){ // RHS Ladder L4 #3
          if (b2Y >= 146){ // Barrel hits L3
            b2direction = 2; // Only option here is left
          }
    } else 
        if ((b2X == 298)&&(b2Y<=160)){ // RHS  L4 drop #4
          if (b2Y >= 145){ // Barrel hits L3
            b2direction = 2; // Only option here is left
          }
    } else 

        if ((b2X == 30)&&(b2Y>=123)){ // LHS Ladder L3 #5
          if (b2Y >= 181){ // Barrel hits L2
            b2direction = 1; // Only option here is Right
          }
    } else 

        if (b2X == 4){ // LHS Ladder L3 #6
          if (b2Y >= 179){ // Barrel hits L2
            b2direction = 1; // Only option here is Right
          }

    } else 
        if (b2X <= 132){ // Middle Ladder L3 #7
          if (b2Y >= 184){ // Barrel hits L2
            b2direction = 1; // Only option here is Right
          }
    } else 
        if ((b2X == 278)&&(b2Y >= 160)){ // RHS Ladder L2 #8
          if (b2Y >= 215){ // Barrel hits L1
            b2direction = 2; // Only option here is left
          }
    } else
        if ((b2X == 298)&&(b2Y>=160)){ // RHS drop from L2 #9
          if (b2Y >= 212){ // Barrel hits L1
            b2direction = 2; // Only option here is left
          }    
        }
   } else
  if (b2direction == 1){ // Barrel Right direction  ***************************************************
    b2X = b2X + 2 ;  // First move Barrel down 2 pixels

   // Apply drop relative to position on levels 4 then 2
   // Level 4
   if ((b2Y <= 140)) { // Validate actually on level 4 then apply formulae for gradient
      b2Y = (((b2X - 30)/23)+112); // Apply drop

      // Now decide on direction at three points on level 4
      if (b2X == 148) { // Level 4 middle ladder decision
        b2direction = random(2);
      } else
      if (b2X == 278) { // Level 4 RHS ladder decision
        b2direction = random(2);
      } else
       if (b2X == 298) { // Level 4 end of scaffold
        b2direction = 0; // Only option down
      }     
      
      
   }else    // Level 2
   if (((b2Y >= 170)&&(b2Y <= 205))) { // Validate actually on level 2 then apply formulae for gradient
      b2Y = (((b2X - 30)/23)+179); // Apply drop
 
       // Now decide on direction at two points on level 2
      if (b2X == 278) { // Level 4 RHS ladder decision
        b2direction = random(2);
      } else
       if (b2X == 298) { // Level 4 end of scaffold
        b2direction = 0; // Only option down
      }
 
   }     
   
 
  } else
  if (b2direction == 2){ // Barrel Left direction **********************************************************
    b2X = b2X - 2 ;  // First move Barrel left 2 pixels


   // Level 3
   if (((b2Y <= 170)&&(b2Y >= 140))) { // Validate actually on level 3 then apply formulae for gradient
      b2Y = (((298 - b2X)/23)+144);
 
       // Now decide on direction at three points on level 3
      if (b2X == 132) { // Level 3 middle ladder decision
        bd = random(2);
        if ( bd == 1){ // Randomise selection
          b2direction = 0; 
          } else { 
          b2direction = 2;
        }   
      } else
      if (b2X == 30) { // Level 3 LHS ladder decision
        bd = random(2);
        if ( bd == 1){ // Randomise selection
          b2direction = 0; 
          } else { 
          b2direction = 2;
          }
        } else
       if (b2X == 4) { // Level 3 end of scaffold
        b2direction = 0; // Only option down
      }     
     
   } else   // Level 1 
   if (((b2X >=160)&&(b2X <= 298))&&(b2Y >= 205)) { // Validate actually on RHS half of level 1 then apply formulae for gradient
       b2Y = (((298 - b2X)/23)+212);
       } // else    
   if (((b2X >=20)&&(b2X <= 159))&&(b2Y >= 205)) { // Validate actually on LHS half of level 1 
       b2Y = 218;  // Flat finish for Barrell
   
       if (b2X <= 22) { // Turn off the Barrell and blank graphic
           triggerbarrel2 = false;
           b2image = 1; // Used to determine which image should be used
           b2direction = 0; // Where Down = 0, Right = 1, Left = 2
           prevb2direction = 0;
           myGLCD.fillRect(b2X, b2Y, b2X+14, b2Y+10); // Clear Barrel       
           b2X = 42;
           b2Y = 74;
           }
       }

  }
}




// **********************************************************************************************
// Barrel 3 Logic
// **********************************************************************************************

/*
// Test print out

      myGLCD.setColor(1, 73, 240);
      myGLCD.setBackColor(0, 0, 0);
      myGLCD.setFont(SmallFont);

      myGLCD.printNumI(b3X, 150, 200,3);
      myGLCD.printNumI(b3Y, 150, 220,3);
*/

if ((triggerbarrel3 == true)) {// Only draw if Barrel in play
  
  drawbarrel(b3X, b3Y, b3image, b3direction, prevb3direction);  // Draw Barrel1

  // Increment barrel image 
  b3image++;
  if (b3image == 5) {
    b3image = 1;
  }

  // Capture previous direction to enable trail blanking
  prevb3direction = b3direction; 

  // Direction and Movement Logic 

  /*
    // Past first block decide to continue or go down
    if (xP == 62) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

  */
  if (b3direction == 0){ // Barrel Down direction   ***************************************************
  b3Y = b3Y + 2 ;  // First move Barrel down 2 pixels

     if ((b3X == 42)&&(b3Y<=120)){ // Monkey drop L5 (additional test for level) #1
        if (b3Y >= 112){ // Barrel hits L4
          b3direction = 1; // Only option here is right
        }
     } else
        if (b3X == 148){ // Middle Ladder L4 #2
          if (b3Y >= 151){ // Barrel hits L3
            b3direction = 2; // Only option here is left
          }
     } else
        if ((b3X == 278)&&(b3Y <= 160)){ // RHS Ladder L4 #3
          if (b3Y >= 146){ // Barrel hits L3
            b3direction = 2; // Only option here is left
          }
    } else 
        if ((b3X == 298)&&(b3Y<=160)){ // RHS  L4 drop #4
          if (b3Y >= 145){ // Barrel hits L3
            b3direction = 2; // Only option here is left
          }
    } else 

        if ((b3X == 30)&&(b3Y>=123)){ // LHS Ladder L3 #5
          if (b3Y >= 181){ // Barrel hits L2
            b3direction = 1; // Only option here is Right
          }
    } else 

        if (b3X == 4){ // LHS Ladder L3 #6
          if (b3Y >= 179){ // Barrel hits L2
            b3direction = 1; // Only option here is Right
          }

    } else 
        if (b3X <= 132){ // Middle Ladder L3 #7
          if (b3Y >= 184){ // Barrel hits L2
            b3direction = 1; // Only option here is Right
          }
    } else 
        if ((b3X == 278)&&(b3Y >= 160)){ // RHS Ladder L2 #8
          if (b3Y >= 215){ // Barrel hits L1
            b3direction = 2; // Only option here is left
          }
    } else
        if ((b3X == 298)&&(b3Y>=160)){ // RHS drop from L2 #9
          if (b3Y >= 212){ // Barrel hits L1
            b3direction = 2; // Only option here is left
          }    
        }
   } else
  if (b3direction == 1){ // Barrel Right direction  ***************************************************
    b3X = b3X + 2 ;  // First move Barrel down 2 pixels

   // Apply drop relative to position on levels 4 then 2
   // Level 4
   if ((b3Y <= 140)) { // Validate actually on level 4 then apply formulae for gradient
      b3Y = (((b3X - 30)/23)+112); // Apply drop

      // Now decide on direction at three points on level 4
      if (b3X == 148) { // Level 4 middle ladder decision
        b3direction = random(2);
      } else
      if (b3X == 278) { // Level 4 RHS ladder decision
        b3direction = random(2);
      } else
       if (b3X == 298) { // Level 4 end of scaffold
        b3direction = 0; // Only option down
      }     
      
      
   }else    // Level 2
   if (((b3Y >= 170)&&(b3Y <= 205))) { // Validate actually on level 2 then apply formulae for gradient
      b3Y = (((b3X - 30)/23)+179); // Apply drop
 
       // Now decide on direction at two points on level 2
      if (b3X == 278) { // Level 4 RHS ladder decision
        b3direction = random(2);
      } else
       if (b3X == 298) { // Level 4 end of scaffold
        b3direction = 0; // Only option down
      }
 
   }     
   
 
  } else
  if (b3direction == 2){ // Barrel Left direction **********************************************************
    b3X = b3X - 2 ;  // First move Barrel left 2 pixels


   // Level 3
   if (((b3Y <= 170)&&(b3Y >= 140))) { // Validate actually on level 3 then apply formulae for gradient
      b3Y = (((298 - b3X)/23)+144);
 
       // Now decide on direction at three points on level 3
      if (b3X == 132) { // Level 3 middle ladder decision
        bd = random(2);
        if ( bd == 1){ // Randomise selection
          b3direction = 0; 
          } else { 
          b3direction = 2;
        }   
      } else
      if (b3X == 30) { // Level 3 LHS ladder decision
        bd = random(2);
        if ( bd == 1){ // Randomise selection
          b3direction = 0; 
          } else { 
          b3direction = 2;
          }
        } else
       if (b3X == 4) { // Level 3 end of scaffold
        b3direction = 0; // Only option down
      }     
     
   } else   // Level 1 
   if (((b3X >=160)&&(b3X <= 298))&&(b3Y >= 205)) { // Validate actually on RHS half of level 1 then apply formulae for gradient
       b3Y = (((298 - b3X)/23)+212);
       } // else    
   if (((b3X >=20)&&(b3X <= 159))&&(b3Y >= 205)) { // Validate actually on LHS half of level 1 
       b3Y = 218;  // Flat finish for Barrell
   
       if (b3X <= 22) { // Turn off the Barrell and blank graphic
           triggerbarrel3 = false;
           b3image = 1; // Used to determine which image should be used
           b3direction = 0; // Where Down = 0, Right = 1, Left = 2
           prevb3direction = 0;
           myGLCD.fillRect(b3X, b3Y, b3X+14, b3Y+10); // Clear Barrel       
           b3X = 42;
           b3Y = 74;
           }
       }

  }
}






// **********************************************************************************************
// Barrel 4 Logic
// **********************************************************************************************

/*
// Test print out

      myGLCD.setColor(1, 73, 240);
      myGLCD.setBackColor(0, 0, 0);
      myGLCD.setFont(SmallFont);

      myGLCD.printNumI(b4X, 150, 200,3);
      myGLCD.printNumI(b4Y, 150, 220,3);
*/

if ((triggerbarrel4 == true)) {// Only draw if Barrel in play
  
  drawbarrel(b4X, b4Y, b4image, b4direction, prevb4direction);  // Draw Barrel1

  // Increment barrel image 
  b4image++;
  if (b4image == 5) {
    b4image = 1;
  }

  // Capture previous direction to enable trail blanking
  prevb4direction = b4direction; 

  // Direction and Movement Logic 

  /*
    // Past first block decide to continue or go down
    if (xP == 62) { 
      direct = random(2); // generate random number between 0 and 1
      if (direct == 1){
         D = direct; // set Pacman direciton varialble to new direction D where 0 = right, 1 = down, 2 = left, 3 = up
      }
    }

  */
  if (b4direction == 0){ // Barrel Down direction   ***************************************************
  b4Y = b4Y + 2 ;  // First move Barrel down 2 pixels

     if ((b4X == 42)&&(b4Y<=120)){ // Monkey drop L5 (additional test for level) #1
        if (b4Y >= 112){ // Barrel hits L4
          b4direction = 1; // Only option here is right
        }
     } else
        if (b4X == 148){ // Middle Ladder L4 #2
          if (b4Y >= 151){ // Barrel hits L3
            b4direction = 2; // Only option here is left
          }
     } else
        if ((b4X == 278)&&(b4Y <= 160)){ // RHS Ladder L4 #3
          if (b4Y >= 146){ // Barrel hits L3
            b4direction = 2; // Only option here is left
          }
    } else 
        if ((b4X == 298)&&(b4Y<=160)){ // RHS  L4 drop #4
          if (b4Y >= 145){ // Barrel hits L3
            b4direction = 2; // Only option here is left
          }
    } else 

        if ((b4X == 30)&&(b4Y>=123)){ // LHS Ladder L3 #5
          if (b4Y >= 181){ // Barrel hits L2
            b4direction = 1; // Only option here is Right
          }
    } else 

        if (b4X == 4){ // LHS Ladder L3 #6
          if (b4Y >= 179){ // Barrel hits L2
            b4direction = 1; // Only option here is Right
          }

    } else 
        if (b4X <= 132){ // Middle Ladder L3 #7
          if (b4Y >= 184){ // Barrel hits L2
            b4direction = 1; // Only option here is Right
          }
    } else 
        if ((b4X == 278)&&(b4Y >= 160)){ // RHS Ladder L2 #8
          if (b4Y >= 215){ // Barrel hits L1
            b4direction = 2; // Only option here is left
          }
    } else
        if ((b4X == 298)&&(b4Y>=160)){ // RHS drop from L2 #9
          if (b4Y >= 212){ // Barrel hits L1
            b4direction = 2; // Only option here is left
          }    
        }
   } else
  if (b4direction == 1){ // Barrel Right direction  ***************************************************
    b4X = b4X + 2 ;  // First move Barrel down 2 pixels

   // Apply drop relative to position on levels 4 then 2
   // Level 4
   if ((b4Y <= 140)) { // Validate actually on level 4 then apply formulae for gradient
      b4Y = (((b4X - 30)/23)+112); // Apply drop

      // Now decide on direction at three points on level 4
      if (b4X == 148) { // Level 4 middle ladder decision
        b4direction = random(2);
      } else
      if (b4X == 278) { // Level 4 RHS ladder decision
        b4direction = random(2);
      } else
       if (b4X == 298) { // Level 4 end of scaffold
        b4direction = 0; // Only option down
      }     
      
      
   }else    // Level 2
   if (((b4Y >= 170)&&(b4Y <= 205))) { // Validate actually on level 2 then apply formulae for gradient
      b4Y = (((b4X - 30)/23)+179); // Apply drop
 
       // Now decide on direction at two points on level 2
      if (b4X == 278) { // Level 4 RHS ladder decision
        b4direction = random(2);
      } else
       if (b4X == 298) { // Level 4 end of scaffold
        b4direction = 0; // Only option down
      }
 
   }     
   
 
  } else
  if (b4direction == 2){ // Barrel Left direction **********************************************************
    b4X = b4X - 2 ;  // First move Barrel left 2 pixels


   // Level 3
   if (((b4Y <= 170)&&(b4Y >= 140))) { // Validate actually on level 3 then apply formulae for gradient
      b4Y = (((298 - b4X)/23)+144);
 
       // Now decide on direction at three points on level 3
      if (b4X == 132) { // Level 3 middle ladder decision
        bd = random(2);
        if ( bd == 1){ // Randomise selection
          b4direction = 0; 
          } else { 
          b4direction = 2;
        }   
      } else
      if (b4X == 30) { // Level 3 LHS ladder decision
        bd = random(2);
        if ( bd == 1){ // Randomise selection
          b4direction = 0; 
          } else { 
          b4direction = 2;
          }
        } else
       if (b4X == 4) { // Level 3 end of scaffold
        b4direction = 0; // Only option down
      }     
     
   } else   // Level 1 
   if (((b4X >=160)&&(b4X <= 298))&&(b4Y >= 205)) { // Validate actually on RHS half of level 1 then apply formulae for gradient
       b4Y = (((298 - b4X)/23)+212);
       } // else    
   if (((b4X >=20)&&(b4X <= 159))&&(b4Y >= 205)) { // Validate actually on LHS half of level 1 
       b4Y = 218;  // Flat finish for Barrell
   
       if (b4X <= 22) { // Turn off the Barrell and blank graphic
           triggerbarrel4 = false;
           b4image = 1; // Used to determine which image should be used
           b4direction = 0; // Where Down = 0, Right = 1, Left = 2
           prevb4direction = 0;
           myGLCD.fillRect(b4X, b4Y, b4X+14, b4Y+10); // Clear Barrel       
           b4X = 42;
           b4Y = 74;
           }
       }

  }
}
















// **********************************************************************************************


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
        userT = 1; // Request to go right   
    }
    if ((xT>=110) && (xT<=210) && (yT>=1) && (yT<=80)) { // Up
        userT = 7; // Request to go Up or Jump 
    }
     if ((xT>=110) && (xT<=210) && (yT>=160) && (yT<=238)) { // Down
        userT = 6; // Request to go Down   
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

       if (userT == 2 && mdirection == 1 ){ // Going Right request to turn Left OK
         mdirection = 2;
         }
       if (userT == 1 && mdirection == 2 ){ // Going Left request to turn Right OK
         mdirection = 1;
         }
       if (userT == 6 && mdirection == 7 ){ // Going Up request to turn Down OK
         mdirection = 6;
         }
       if (userT == 7 && mdirection == 6 ){ // Going Down request to turn Up OK
         mdirection = 7;
         }

      // Set Flag for Jump only if going left or right
        if (userT == 7 && ((mprevdirection == 1) || (mprevdirection == 2 ))){ // Going Up request to turn Down OK     
        jumptrigger = true;
        }
     screenPressed = true;
 }
    // Doesn't allow holding the screen / you must tap it
    else if ( !myTouch.dataAvailable() && screenPressed){
      screenPressed = false;
   }







// Burning Oil Barrels on ground level Platform
myGLCD.drawBitmap (10, 206, 16, 24, Oil1); //   First Oil Barrel

delay(dly); 

// Burning Oil Barrels on ground level Platform
myGLCD.drawBitmap (10, 206, 16, 24, Oil2); //   Second Oil Barrel

//Monkey graphic

if (monkeygraphic == Barreldelay) {
  myGLCD.drawBitmap (27, 43, 45, 32, Monkey1); //   Monkey1 graphic
  }
if (monkeygraphic == Barreldelay*2) {
  myGLCD.drawBitmap (27, 43, 45, 32, Monkey2); //   Monkey2 graphic
  }
if (monkeygraphic == Barreldelay*3) {
  myGLCD.drawBitmap (27, 43, 45, 32, Monkey3); //   Monkey3 graphic
  }
if (monkeygraphic == Barreldelay*4) {
  myGLCD.drawBitmap (27, 43, 45, 32, Monkeyleft); //   Monkey4 graphic
  }
if (monkeygraphic == Barreldelay*5) {

  // trigger off the next Barrel
  if ((triggerbarrel1 == false) && (b1X == 42)&&(b1Y == 74)){
    triggerbarrel1 = true;
  }  else
  if ((triggerbarrel2 == false) && (b2X == 42)&&(b2Y == 74)){
    triggerbarrel2 = true;
  } else
  if ((triggerbarrel3 == false) && (b3X == 42)&&(b3Y == 74)){
    triggerbarrel3 = true;
  } else
    if ((triggerbarrel4 == false) && (b4X == 42)&&(b4Y == 74)){
    triggerbarrel4 = true;
  }

  if ((triggerbarrel1 == false) || (triggerbarrel2 == false) || (triggerbarrel3 == false)|| (triggerbarrel4 == false))  { // Only display Monkey with Barrel if less than 3 barrels in play
    myGLCD.drawBitmap (27, 43, 45, 32, MonkeyBarrel); //   MonkeyBarrel graphic
  }
  // reset the Monkey Graphic to start sequence
  monkeygraphic = 0;
 }
monkeygraphic++; // increment counter
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
    myGLCD.printNumI(d1,54,0); // Printing thisnumber impacts LFH walls so redraw impacted area   

// ---------------- Clear lines on Outside wall
//    myGLCD.setColor(0,0,0);
//    myGLCD.drawRoundRect(1, 238, 318, 1); 

}

//If prevous time 12:59 or 00:59 and change in time then blank First Digit

if((c1 == 1) && (c2 == 2) && (c3 == 5) && (c4 == 9) && (d2 != c2) ){ // Clear the previouis First Digit and redraw wall

    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(54, 0, 118, 100);


}

if((c1 == 0) && (c2 == 0) && (c3 == 5) && (c4 == 9) && (d2 != c2) ){ // Clear the previouis First Digit and redraw wall

    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(54, 0, 118, 100);
}
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(SevenSeg_XXXL_Num);
  
// Second Digit
if((d2 != c2)||(xsetup == true)){
  myGLCD.printNumI(d2,121,0); // Print 0
}

// Third Digit
if((d3 != c3)||(xsetup == true)){
  myGLCD.printNumI(d3,188,0); // Was 145    
}

// Fourth Digit
if((d4 != c4)||(xsetup == true)){
  myGLCD.printNumI(d4,255,0); // Was 205  
}

if (xsetup == true){
  xsetup = false; // Reset Flag now leaving setup mode
  } 
 // Print PM or AM
 
//    myGLCD.setColor(1, 73, 240);
      myGLCD.setBackColor(0, 0, 0);
      myGLCD.setFont(SmallFont);
  if (pm == 0) {
      myGLCD.print("am", 300, 227); 
   } else {
      myGLCD.print("pm", 300, 227);  
   }
      myGLCD.drawBitmap (296, 224, 23, 8, DK_scaffold); //   Scaffold
// ----------- Alarm Set on LHS lower pillar
if (alarmstatus == true) { // Print AS on fron screenleft hand side
      myGLCD.print("as", 0, 135); 
}


  // Round dots
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  
  myGLCD.fillRect(183, 26, 188, 35);
  myGLCD.fillRect(183, 65, 188, 74);
  
// Cover any blanks caused by digits

//Fifth   Level
myGLCD.drawBitmap (0, 76, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (22, 76, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (45, 76, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (68, 76, 23, 8, DK_scaffold); //   Scaffold
// myGLCD.drawBitmap (91, 90, 23, 8, DK_scaffold); //   Scaffold

//Sixth   Level
myGLCD.drawBitmap (0, 28, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (22, 28, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (45, 28, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (68, 28, 23, 8, DK_scaffold); //   Scaffold

//Redraw Monkey
if (monkeygraphic <= Barreldelay) {
  myGLCD.drawBitmap (27, 43, 45, 32, Monkey1); //   Monkey1 graphic
  }else
if (monkeygraphic <= Barreldelay*2) {
  myGLCD.drawBitmap (27, 43, 45, 32, Monkey2); //   Monkey2 graphic
  }else
if (monkeygraphic <= Barreldelay*3) {
  myGLCD.drawBitmap (27, 43, 45, 32, Monkey3); //   Monkey3 graphic
  }else
if (monkeygraphic <= Barreldelay*4) {
  myGLCD.drawBitmap (27, 43, 45, 32, Monkeyleft); //   Monkey3 graphic
  }else
if (monkeygraphic < Barreldelay*5) {
  myGLCD.drawBitmap (27, 43, 45, 32, MonkeyBarrel); //   Monkey3 graphic
  monkeygraphic = 0;
  }


// Four Barrels on Platform
myGLCD.drawBitmap (0, 59, 10, 16, Barrel1); //   First Barrel
myGLCD.drawBitmap (10, 59, 10, 16, Barrel1); //   First Barrel
myGLCD.drawBitmap (0, 43, 10, 16, Barrel1); //   First Barrel
myGLCD.drawBitmap (10, 43, 10, 16, Barrel1); //   First Barrel

// Girl on top level Platform
myGLCD.drawBitmap (20, 5, 15, 21, Girl1); //   Damsel

// Level 5 Ladders
myGLCD.drawBitmap (81, 0, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (81, 12, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (81, 24, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (81, 36, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (81, 48, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (81, 60, 10, 15, Ladder1); //   Ladders

// Level 6 Ladders
myGLCD.drawBitmap (50, 0, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (50, 12, 10, 15, Ladder1); //   Ladders

//--------------------- copy exising time digits to global variables so that these can be used to test which digits change in future

c1 = d1;
c2 = d2;
c3 = d3;
c4 = d4;

}




// ===== initiateGame - Custom Function
void drawscreen() {

  
  // Setup Clock Background
  //Draw Background lines

      myGLCD.setColor(1, 73, 240);
 
// ---------------- Outside wall
//        myGLCD.drawRoundRect(0, 239, 319, 0); 
//        myGLCD.drawRoundRect(2, 237, 317, 2); 

//First Level
myGLCD.drawBitmap (0, 231, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (22, 231, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (45, 231, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (68, 231, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (91, 231, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (114, 231, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (137, 231, 23, 8, DK_scaffold); //   Scaffold

myGLCD.drawBitmap (160, 230, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (183, 229, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (206, 228, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (229, 227, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (252, 226, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (275, 225, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (296, 224, 23, 8, DK_scaffold); //   Scaffold

//Second   Level
myGLCD.drawBitmap (0, 191, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (22, 192, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (45, 193, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (68, 194, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (91, 195, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (114, 196, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (137, 197, 23, 8, DK_scaffold); //   Scaffold

myGLCD.drawBitmap (160, 198, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (183, 199, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (206, 200, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (229, 201, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (252, 202, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (275, 203, 23, 8, DK_scaffold); //   Scaffold
//myGLCD.drawBitmap (296, 204, 23, 8, DK_scaffold); //   Scaffold

//Third   Level
// myGLCD.drawBitmap (0, 191, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (22, 168, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (45, 167, 23, 8, DK_scaffold); //   Scaffold

myGLCD.drawBitmap (68, 166, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (91, 165, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (114, 164, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (137, 163, 23, 8, DK_scaffold); //   Scaffold

myGLCD.drawBitmap (160, 162, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (183, 161, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (206, 160, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (229, 159, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (252, 158, 23, 8, DK_scaffold); //   Scaffold 

myGLCD.drawBitmap (275, 157, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (296, 156, 23, 8, DK_scaffold); //   Scaffold


//Fourth   Level
myGLCD.drawBitmap (0, 123, 23, 8, DK_scaffold); //   Scaffold modified to accomodate ladder
myGLCD.drawBitmap (22, 123, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (45, 124, 23, 8, DK_scaffold); //   Scaffold

myGLCD.drawBitmap (68, 125, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (91, 126, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (114, 127, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (137, 128, 23, 8, DK_scaffold); //   Scaffold

myGLCD.drawBitmap (160, 129, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (183, 130, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (206, 131, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (229, 132, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (252, 133, 23, 8, DK_scaffold); //   Scaffold 

myGLCD.drawBitmap (275, 134, 23, 8, DK_scaffold); //   Scaffold
//myGLCD.drawBitmap (296, 156, 23, 8, DK_scaffold); //   Scaffold

//Fifth   Level
myGLCD.drawBitmap (0, 76, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (22, 76, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (45, 76, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (68, 76, 23, 8, DK_scaffold); //   Scaffold
// myGLCD.drawBitmap (91, 90, 23, 8, DK_scaffold); //   Scaffold

//Sixth   Level
myGLCD.drawBitmap (0, 28, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (22, 28, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (45, 28, 23, 8, DK_scaffold); //   Scaffold
myGLCD.drawBitmap (68, 28, 23, 8, DK_scaffold); //   Scaffold

//Monkey
myGLCD.drawBitmap (27, 43, 45, 32, Monkey1); //   Monkey1 graphic

// Four Barrels on Platform
myGLCD.drawBitmap (0, 59, 10, 16, Barrel1); //   First Barrel
myGLCD.drawBitmap (10, 59, 10, 16, Barrel1); //   First Barrel
myGLCD.drawBitmap (0, 43, 10, 16, Barrel1); //   First Barrel
myGLCD.drawBitmap (10, 43, 10, 16, Barrel1); //   First Barrel

// Burning Oil Barrels on ground level Platform
myGLCD.drawBitmap (10, 206, 16, 24, Oil1); //   First Oil Barrel
//myGLCD.drawBitmap (10, 206, 16, 24, Oil2); //   Second Oil Barrel

// Girl on top level Platform
myGLCD.drawBitmap (20, 6, 15, 21, Girl1); //   Damsel

// Level 6 Ladders
myGLCD.drawBitmap (50, 0, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (50, 12, 10, 15, Ladder1); //   Ladders

// Level 5 Ladders
myGLCD.drawBitmap (81, 0, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (81, 12, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (81, 24, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (81, 36, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (81, 48, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (81, 60, 10, 15, Ladder1); //   Ladders

// Level 4 Ladders
myGLCD.drawBitmap (15, 84, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (15, 96, 10, 15, Ladder1); //   Ladders
myGLCD.drawBitmap (15, 108, 10, 15, Ladder1); //   Ladders

// Level 3 Ladders
myGLCD.drawBitmap (150, 136, 10, 15, Ladder1); //   Middle Ladders
myGLCD.drawBitmap (150, 148, 10, 15, Ladder1); //   Middle Ladders
myGLCD.drawBitmap (280, 141, 10, 15, Ladder1); //   RHS Ladders

// Level 2 Ladders
myGLCD.drawBitmap (134, 170, 10, 15, Ladder1); //   Middle Ladders
myGLCD.drawBitmap (134, 182, 10, 15, Ladder1); //   Middle Ladders
myGLCD.drawBitmap (32, 176, 10, 15, Ladder1); //   LHS Ladders

// Level 1 Ladder
myGLCD.drawBitmap (280, 210, 10, 15, Ladder1); //   RHS Ladders







 }
 


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

//      myGLCD.setColor(255, 255, 0);
//      myGLCD.setBackColor(0, 0, 0);

//   myGLCD.drawRoundRect(0, 239, 319, 0); 
//   myGLCD.drawRoundRect(2, 237, 317, 2); 
   
//Reset screenpressed flag
screenPressed = false;

// Read in current clock time and Alarm time
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);


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
    myGLCD.drawBitmap (5, 100, 16, 16, MarioR1); //   Closed Ghost 
    myGLCD.drawBitmap (240, 100, 45, 32, Monkeyleft); //   Closed Ghost 
//    myGLCD.drawBitmap (154, 208, 40, 40, scissors); //   Closed Ghost 

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
 
     else if ((xT>=130) && (xT<=154) && (yT>=133) && (yT<=157)) { // (132, 135, 152, 155); // alarm hour +
        alarmhour = alarmhour + 1; // Increment Hour
        if (alarmhour == 24) {  // reset hour to 0 hours if 24
           alarmhour = 0 ;
       
      } 
    } 

    else if ((xT>=130) && (xT<=154) && (yT>=178) && (yT<=202)) { // (132, 180, 152, 200);  // alarm hour -
        alarmhour = alarmhour - 1; // Increment Hour
        if (alarmhour == -1) {  // reset hour to 23 hours if < 0
           alarmhour = 23 ;
       
      } 
    }
    
    else if ((xT>=178) && (xT<=202) && (yT>=133) && (yT<=157)) { // (180, 135, 200, 155); // alarm minute +
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
 


// **********************************************************************************************
// Draw Barrel
// **********************************************************************************************
/*
int b1X = 30; 
int b1Y = 79;
int b1image = 1; // Used to determine which image should be used
int b1direction = 0; // Where Down = 0, Right = 1, Left = 2
int prevb1direction = 0; // Where Down = 0, Right = 1, Left = 2
extern unsigned int Barrelroll1[0x78]; // 12x10 
extern unsigned int Barrelroll2[0x78]; // 12x10 
extern unsigned int Barrelroll3[0x78]; // 12x10
extern unsigned int Barrelroll4[0x78]; // 12x10
extern unsigned int Barrelh1[0x96]; // 15x10 
extern unsigned int Barrel1h2[0x96]; // 15x10  
boolean triggerbarrel3 = false; // switch used to trigger release of a barrel 
if direction is down then use Barrelh1 and Barrellh2
*/

void drawbarrel(int x, int y, int image, int cdir, int prevdir) { // Make provision for 3 barrels

  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(0, 0, 0);

// firstly based on previous direction blank out 2 pixels worth of screen
// Where Down = 0, Right = 1, Left = 2

if (prevdir == 0) { // Down  *************************************************************************************************
    myGLCD.fillRect(x, y, x+15, y-3); // Clear trail off graphic before printing new position  15x10


  // Cleanup Scaffold and Ladder Damage from downward activties

  // Monkey Drops Barrel

  if (x == 42 && y == 76) {
    myGLCD.drawBitmap (27, 43, 45, 32, Monkey1); //   Monkey1 graphic
  }
  if (x == 42  && y == 84) {
    myGLCD.drawBitmap (22, 76, 23, 8, DK_scaffold); //   Scaffold
    myGLCD.drawBitmap (45, 76, 23, 8, DK_scaffold); //   Scaffold
  }

  // Level 3 middle Ladder
  if (x == 148 && y == 139) {
    myGLCD.drawBitmap (137, 128, 23, 8, DK_scaffold); //   Scaffold
    myGLCD.drawBitmap (160, 129, 23, 8, DK_scaffold); //   Scaffold
  }
  if (x == 148 && y == 151) {
    myGLCD.drawBitmap (150, 136, 10, 15, Ladder1); //   Top of L3 Middle Ladder
  }


  // Level 2 middle Ladder
   if (x == 132 && y == 173) {
    myGLCD.drawBitmap (114, 164, 23, 8, DK_scaffold); //   Scaffold
     myGLCD.drawBitmap (137, 163, 23, 8, DK_scaffold); //   Scaffold   
  }
   if (x == 132 && y == 183) {
    myGLCD.drawBitmap (134, 170, 10, 15, Ladder1); //   Top of L2 Middle Ladder
  }

   if (x == 278 && y == 144) {
    myGLCD.drawBitmap (275, 134, 23, 8, DK_scaffold);  //   Scaffold above L4 RHS Ladder
  }
   if (x == 278 && y == 211) {
    myGLCD.drawBitmap (275, 203, 23, 8, DK_scaffold);  //   Scaffold above L1 RHS Ladder
  }

   if (x == 30 && y == 177) {
    myGLCD.drawBitmap (22, 168, 23, 8, DK_scaffold);  //   Scaffold above L1 RHS Ladder
  }

} else 
if (prevdir == 1) { // Right  *************************************************************************************************
    myGLCD.fillRect(x+2, y-2, x-3, y+10); // Clear trail off graphic before printing new position 12x10

   if (x == 146 && y == 184) {
    myGLCD.drawBitmap (134, 182, 10, 15, Ladder1); //   Bottom of L2 Middle Ladder
  } else

  if (x == 46 && y == 179) {
    myGLCD.drawBitmap (32, 176, 10, 15, Ladder1); //   LHS Ladder Level 2
  }


} else
if (prevdir == 2) { // Left *************************************************************************************************
    myGLCD.fillRect(x+9, y-2, x+14, y+10); // Clear trail off graphic before printing new position 12x10


  
  if (x == 136 && y == 151) {
    myGLCD.drawBitmap (150, 148, 10, 15, Ladder1); //   Bottom of L3 Middle Ladder
  }  
  if (x == 264 && y == 145) {
    myGLCD.drawBitmap (280, 141, 10, 15, Ladder1); //   L3 RHS Ladder
  } 
  if (x == 264 && y == 213) {
    myGLCD.drawBitmap (280, 210, 10, 15, Ladder1); //   L3 RHS Ladder
  } 


} 


// Special Circumstances where barel does Down/Left turn

if ((cdir == 2) && (prevdir == 0)) {
  
    if (x == 298 && y == 212) {
      myGLCD.fillRect(x+13, y-1, x+16, y+10); // Clear trail off graphic before printing new position 12x10
    } else
    if (x == 148 && y == 151) {
      myGLCD.fillRect(x+13, y-1, x+16, y+10); // Clear trail off graphic before printing new position 12x10
    } else 
    if (x == 298 && y == 145) {
      myGLCD.fillRect(x+13, y-1, x+16, y+10); // Clear trail off graphic before printing new position 12x10
    }  else
    if (x == 278 && y == 215) {
      myGLCD.fillRect(x+13, y-1, x+16, y+10); // Clear trail off graphic before printing new position 12x10
    }  else
    if (x == 278 && y == 146) {
      myGLCD.fillRect(x+13, y-1, x+16, y+10); // Clear trail off graphic before printing new position 12x10
    }  
}


/*
// Special Circumstance where Barrel Y value drops by 1 leaving a trail

  myGLCD.setColor(200, 0, 0);

    if (prevb1X != x ){ // Ifin transition wipe the trail

        if (cdir == 2) { // If travelling left
            myGLCD.fillRect(x-2, y-2, x+14, y); // Clear trail off graphic before printing new position 12x10
        } else
        if (cdir == 1) { // If travelling right
            myGLCD.fillRect(x, y+2, x+14, y); // Clear trail off graphic before printing new position 12x10
        }  
    }
*/

// Now display Barrel at X,Y coordinates using the correct graphic image

if (cdir != 0) { // only display barrelroll image if travelling left or right
  if (image == 1) {
    myGLCD.drawBitmap(x, y, 12, 10, Barrelr1); //   Rolling Barrel image 1  

  }else 
  if (image == 2) {
    myGLCD.drawBitmap(x, y, 12, 10, Barrelr2); //   Rolling Barrel image 2  
  }else 
  if (image == 3) {
    myGLCD.drawBitmap(x, y, 12, 10, Barrelr3); //   Rolling Barrel image 3
  }else 
  if (image == 4) {
    myGLCD.drawBitmap(x, y, 12, 10, Barrelr4); //   Rolling Barrel image 4  
  } 
} else { // Display horizontal barrel images

  if ((image == 1)||(image == 3)) {
    myGLCD.drawBitmap(x, y, 15, 10, Barrelh1); //  H Rolling Barrel image 1  
  }else 
  if ((image == 2)||(image == 4)) {
    myGLCD.drawBitmap(x, y, 15, 10, Barrelh2); //  H Rolling Barrel image 2  
  
  }
 }  
}


// **********************************************************************************************
// Draw Mario
// **********************************************************************************************
/*
int mX = 30; 
int mY = 230;
int mimage = 1; // Used to determine which image should be used
int mdirection = 0; // Where Down = 0, Right = 1, Left = 2, Up = 3, Jump Right = 4, Jump Left = 6
int mprevdirection = 0; // Where Down = 0, Right = 1, Left = 2
boolean mtrigger = false; // switch used to trigger release of a mario

extern unsigned int MarioL1[0x100]; // 16x16 
extern unsigned int MarioL2[0x100]; // 16x16 
extern unsigned int MarioL3[0x100]; // 16x16 Jump Left
extern unsigned int MarioR1[0x100]; // 16x16 
extern unsigned int MarioR2[0x100]; // 16x16 
extern unsigned int MarioR3[0x100]; // 16x16 Jump Right
extern unsigned int MarioU1[0x100]; // 16x16 Climb Ladder 1
extern unsigned int MarioU2[0x100]; // 16x16 Climb Ladder 2
extern unsigned int MarioU3[0x100]; // 16x16 Climb Ladder 3
extern unsigned int MarioU4[0x100]; // 16x16 Climeb Ladder 4
extern unsigned int MarioStop[0x100]; // 16x16 Mario Stand Still


boolean mtrigger = false; // switch used to trigger release of a Mario direction is 




*/

void drawMario(int x, int y, int image, int cdir, int prevdir) { // Draw Mario function

  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(0, 0, 0);

/* **************************************************************************************************************************
Firstly based on previous direction blank out 2 pixels worth of screen where image is 16 x 16
*****************************************************************************************************************************
Mario Movements
1 - Left as expected left of touch screen
2 - Right as expected left & right of touch screen
3 - Jump straight Up = Top of touch screen
4 - Right Jump = Top RHS of touch screem
5 - Left Jump = Top LHS of touch screen 
6 - Climb Down = When at top of ladder Bottom of touch screen
7 - Climb Up - Whn at bottom of a ladder touch bottom of screen
8 - Stop = Bottom of touch screen
*/ 
if (prevdir == 2) { // Left *************************************************************************************************
    myGLCD.fillRect(x+11, y-1, x+16, y+16); // Clear trail off graphic before printing new position 16x16

  if (x == 212 && y == 209) {
    myGLCD.drawBitmap (229, 201, 23, 8, DK_scaffold); //   Scaffold to the left of Ladder 6
  }
  if (x == 236 && y == 208) {
    myGLCD.drawBitmap (252, 202, 23, 8, DK_scaffold); //   Scaffold to the left of Ladder 6
  }     
  if (x == 260 && y == 207) {
    myGLCD.drawBitmap (276, 203, 23, 8, DK_scaffold); //   Scaffold above Ladder 6
    myGLCD.drawBitmap (280, 210, 10, 15, Ladder1); //   Ladder 6
  }   

  if (x == 114 && y == 178) {
    myGLCD.drawBitmap (134, 182, 10, 15, Ladder1); // ladder 5 bottom rungs
    myGLCD.drawBitmap (134, 170, 10, 15, Ladder1); // ladder 5 top rungs
  }   

  if (x == 212 && y == 209) { // Level 3 LHS scaffold
    myGLCD.drawBitmap (22, 168, 23, 8, DK_scaffold); //   Scaffold to the left of Ladder 4
    myGLCD.drawBitmap (45, 167, 23, 8, DK_scaffold); //   Scaffold to the right of Ladder 4
  }

  if (x == 116 && y == 178) {
    myGLCD.drawBitmap (134, 182, 10, 15, Ladder1); // ladder 5 bottom rungs
    myGLCD.drawBitmap (134, 170, 10, 15, Ladder1); // ladder 5 top rungs
  }
  if (x == 8 && y == 173 ) {
    myGLCD.drawBitmap (45, 167, 23, 8, DK_scaffold); //   Scaffold above right of ladder 4
    myGLCD.drawBitmap (32, 176, 10, 15, Ladder1); //   Ladder 4
    myGLCD.drawBitmap (22, 168, 23, 8, DK_scaffold); //   Scaffold above left of ladder 4    
  }

  if (x == 130 && y == 146) {
    myGLCD.drawBitmap (150, 136, 10, 15, Ladder1); // ladder 2 top rungs
    myGLCD.drawBitmap (150, 148, 10, 15, Ladder1); // ladder 2 bottom rungs
    
  }
  if (x == 134 && y == 146) {// test
    myGLCD.drawBitmap (150, 148, 10, 15, Ladder1); // ladder 4 lower
    myGLCD.drawBitmap (150, 136, 10, 15, Ladder1); // ladder 4 uper
  }



  if (x == 278 && y == 117) {
    myGLCD.drawBitmap (275, 134, 23, 8, DK_scaffold); //   Ladder 3 top right Scaffold     
  }  
  if (x == 274 && y == 139) {
    myGLCD.drawBitmap (275, 134, 23, 8, DK_scaffold); //   Ladder 3 top right Scaffold     
  }  
   if (x == 260 && y == 140) {
    myGLCD.drawBitmap (275, 134, 23, 8, DK_scaffold); //   Ladder 3 top right Scaffold     
  }
   if (x == 202 && y == 143) {
    myGLCD.drawBitmap (252, 133, 23, 8, DK_scaffold); //   Ladder 3 top right Scaffold 
    myGLCD.drawBitmap (275, 134, 23, 8, DK_scaffold); //   Ladder 3 top right Scaffold      
  } 
   if (x == 240 && y == 141) {
    myGLCD.drawBitmap (252, 133, 23, 8, DK_scaffold); //   Ladder 3 top right Scaffold     
  } 

  if (x == 258 && y == 140) {
    myGLCD.drawBitmap (280, 141, 10, 15, Ladder1); // ladder 3 
  }
 
   if (x == 4 && y == 104) {
    myGLCD.drawBitmap (15, 96, 10, 15, Ladder1); // ladder 1 middle
    myGLCD.drawBitmap (15, 108, 10, 15, Ladder1); // ladder 1 bottom
  } 
   if (x == 264 && y == 116) {
    myGLCD.drawBitmap (252, 133, 23, 8, DK_scaffold); //   Ladder 3 top right Scaffold     
  }  

// Clear at top of ladder 2
  if (x == 144 && y == 111) {
    myGLCD.fillRect(x, y, x+16, y+3); // Clear trail off graphic before printing new position  16x16
  }
  
// Clear at top of ladder 5
  if (x == 130 && y == 146) {
    myGLCD.fillRect(x, y, x+16, y+3); // Clear trail off graphic before printing new position  16x16
  }  

  
} else
if (prevdir == 1) { // Right  *************************************************************************************************
    myGLCD.fillRect(x+3, y-1, x-2, y+16); // Clear trail off graphic before printing new position 16x16
    
    
  if (x == 252 && y == 208) {
    myGLCD.drawBitmap (229, 201, 23, 8, DK_scaffold); //   Scaffold
  }
  if (x == 276 && y == 206) {
    myGLCD.drawBitmap (252, 202, 23, 8, DK_scaffold); //   Scaffold
  }     
  if (x == 298 && y == 206) {
    myGLCD.drawBitmap (276, 203, 23, 8, DK_scaffold); //   Scaffold above ladder 6
    myGLCD.drawBitmap (280, 210, 10, 15, Ladder1); //   Ladder 6
  }   

  if (x == 146 && y == 179) {
    myGLCD.drawBitmap (134, 182, 10, 15, Ladder1); // ladder 5 bottom rungs
    myGLCD.drawBitmap (134, 170, 10, 15, Ladder1); // ladder 5 top rungs
  }
  if (x == 48 && y == 175 ) {
    myGLCD.drawBitmap (22, 168, 23, 8, DK_scaffold); //   Scaffold above left of ladder 4
    myGLCD.drawBitmap (32, 176, 10, 15, Ladder1); //   Ladder 4
  }
  if (x == 68 && y == 175 ) {
    myGLCD.drawBitmap (45, 167, 23, 8, DK_scaffold); //   Scaffold above right of ladder 4
  }

  if (x > 296 && y <156 ) {
    myGLCD.drawBitmap (280, 141, 10, 15, Ladder1); // ladder 3 
  }

  if (x > 296 && y >210 ) {
    myGLCD.drawBitmap (280, 210, 10, 15, Ladder1); // ladder 3 
  }

  if (x == 162 && y == 144) {
    myGLCD.drawBitmap (150, 148, 10, 15, Ladder1); // ladder 4 lower
    myGLCD.drawBitmap (150, 136, 10, 15, Ladder1); // ladder 4 uper
  }
  
  if (x == 272 && y == 140) {
    myGLCD.drawBitmap (252, 133, 23, 8, DK_scaffold); //   Scaffold
  }

  if (x == 26 && y == 106) {
    myGLCD.drawBitmap (15, 96, 10, 15, Ladder1); // ladder 1 middle
    myGLCD.drawBitmap (15, 108, 10, 15, Ladder1); // ladder 1 bottom
    myGLCD.drawBitmap (0, 123, 23, 8, DK_scaffold); //   Scaffold left bottom ladder 1
    myGLCD.drawBitmap (22, 123, 23, 8, DK_scaffold); //   Scaffold right bottom ladder 1
  }
  
    if (x == 32 && y == 150) {
    myGLCD.drawBitmap (45, 167, 23, 8, DK_scaffold); //   Scaffold upper right from ladder 4
  }


// Trail appears aobve Marios Head when dropping a level right to left like barrels
  
} else
if (prevdir == 3) { // Jump straight Up then down  *****************************************************************************
    myGLCD.fillRect(x-1, y+16, x+17, y+18); // When jump trajectory going up use 3 this then When jump trajectory going down use 6 Climb Down 
}else

if (prevdir == 4) { // Right Jump *************************************************************************************************
    myGLCD.fillRect(x, y-2, x-2, y+16); // Clear trail off graphic before printing new position 16x16
} else

if (prevdir == 5) { // Left Jump*************************************************************************************************
    myGLCD.fillRect(x+15, y-2, x+18, y+16); // Clear trail off graphic before printing new position 16x16
} else

if (prevdir == 6) { // Climb Down  ********************************************************************************************
    myGLCD.fillRect(x-2, y, x+17, y-2); // Clear trail off graphic before printing new position  16x16
    
  if (x == 132 && y == 176) {
    myGLCD.drawBitmap (137, 163, 23, 8, DK_scaffold); //   Ladder 5 top right Scaffold 
    myGLCD.drawBitmap (114, 164, 23, 8, DK_scaffold); //   Ladder 5 top left Scaffold     
  }    


  if (x == 148 && y == 143) {
    myGLCD.drawBitmap (150, 136, 10, 15, Ladder1); // ladder 4 
  }     

  if (x == 148 && y == 139) {
    myGLCD.drawBitmap (137, 128, 23, 8, DK_scaffold); //   Ladder 2 top  Scaffold 
    myGLCD.drawBitmap (160, 129, 23, 8, DK_scaffold); //   Ladder 2 top  Scaffold         
  }

   if (x == 12 && y == 97) {
    myGLCD.drawBitmap (0, 76, 23, 8, DK_scaffold); //   Ladder 1 top left Scaffold 
    myGLCD.drawBitmap (22, 76, 23, 8, DK_scaffold); //   Ladder 1 top right Scaffold 
    
    myGLCD.drawBitmap (15, 84, 10, 15, Ladder1); // ladder 1 top  
  } 

  if (x == 12 && y == 107) {
    myGLCD.drawBitmap (15, 96, 10, 15, Ladder1); // ladder 1 middle
  }







    
} else  
if (prevdir == 7) { // Climb Up  ********************************************************************************************
    myGLCD.fillRect(x-2, y+16, x+17, y+18); // Clear trail off graphic before printing new position  16x16
    
  if (x == 278 && y == 186) {
    myGLCD.drawBitmap (280, 210, 10, 15, Ladder1); //    Ladder 6 
    myGLCD.drawBitmap (276, 203, 23, 8, DK_scaffold); //   Scaffold above RHS ladder 6
  }     
    
  if (x == 132 && y == 166) {
    myGLCD.drawBitmap (134, 182, 10, 15, Ladder1); // ladder 5 bottom rungs
  }

  if (x == 132 && y == 154) {
    myGLCD.drawBitmap (134, 170, 10, 15, Ladder1); // ladder 5 top rungs
  }

  if (x == 132 && y == 146) {
    myGLCD.drawBitmap (137, 163, 23, 8, DK_scaffold); //   Ladder 5 top right Scaffold 
    myGLCD.drawBitmap (114, 164, 23, 8, DK_scaffold); //   Ladder 5 top left Scaffold 
    myGLCD.drawBitmap (137, 197, 23, 8, DK_scaffold); //   Ladder 5 top left Scaffold     
    
  } 

  if (x == 30 && y == 154) {
    myGLCD.drawBitmap (32, 176, 10, 15, Ladder1); // ladder 4 
  }
  if (x == 30 && y == 152) {
    myGLCD.drawBitmap (22, 168, 23, 8, DK_scaffold); //   Ladder 4 top left Scaffold     
  }
  if (x == 148 && y == 141) {
    myGLCD.drawBitmap (160, 162, 23, 8, DK_scaffold); //   Scaffold above right of Ladder 5
    myGLCD.drawBitmap (183, 161, 23, 8, DK_scaffold); //   Scaffold above right of Ladder 5    
  }
  if (x == 148 && y == 133) {
    myGLCD.drawBitmap (150, 148, 10, 15, Ladder1); // ladder 4 
  }
  if (x == 148 && y == 117) {
    myGLCD.drawBitmap (150, 136, 10, 15, Ladder1); // ladder 4 
  }    
  if (x == 148 && y == 111) {
    myGLCD.drawBitmap (137, 128, 23, 8, DK_scaffold); //   Ladder 4 top left Scaffold     
  }  

  if (x == 278 && y == 123) {
    myGLCD.drawBitmap (280, 141, 10, 15, Ladder1); // ladder 3 
  }

  if (x == 278 && y == 117) {
    myGLCD.drawBitmap (275, 134, 23, 8, DK_scaffold); //   Ladder 3 top right Scaffold     
  }  

  if (x == 278 && y == 135) {
    myGLCD.drawBitmap (275, 157, 23, 8, DK_scaffold); //   Ladder 3 bottom Scaffold     
  } 

  if (x == 148 && y == 111) {
    myGLCD.drawBitmap (137, 128, 23, 8, DK_scaffold); //   Ladder 2 top  Scaffold 
    myGLCD.drawBitmap (160, 129, 23, 8, DK_scaffold); //   Ladder 2 top  Scaffold     
    
  }
  if (x == 278 && y == 123) {
    myGLCD.drawBitmap (280, 141, 10, 15, Ladder1); // ladder 3 
  }

   if (x == 12 && y == 103) {
    myGLCD.drawBitmap (0, 123, 23, 8, DK_scaffold); //   Ladder 1 bottom Scaffold     
  }
  if (x == 12 && y == 95) {
    myGLCD.drawBitmap (15, 108, 10, 15, Ladder1); // ladder 1 bottom
  }
  if (x == 12 && y == 85) {
    myGLCD.drawBitmap (15, 96, 10, 15, Ladder1); // ladder 1 middle
    myGLCD.drawBitmap (15, 108, 10, 15, Ladder1); // ladder 1 bottom
    
  }
    if (x == 148 && y == 141) {
    myGLCD.drawBitmap (160, 162, 23, 8, DK_scaffold); //   Ladder 3 top right Scaffold  
    myGLCD.drawBitmap (183, 161, 23, 8, DK_scaffold); //   Ladder 3 top right Scaffold     
   
  }
  
  if (x == 30 && y == 172) {
    myGLCD.drawBitmap (22, 192, 23, 8, DK_scaffold); //   Ladder 4 bottom Scaffold     
  }



}





// 8 - If previous direction was stopped then no action so ignore 
 
// Now display Mario at X,Y coordinates using the correct graphic image
/*
Mario Movements
1 - Right as expected left & right of touch screen
2 - Left as expected left of touch screen
3 - Jump straight Up = Top of touch screen
4 - Right Jump = Top RHS of touch screem
5 - Left Jump = Top LHS of touch screen 
6 - Climb Down = When at top of ladder Bottom of touch screen
7 - Climb Up = When at bottom of ladder Top of touch screen
8 - Stop = Stand upright when touch Bottom of touch screen

Use Image Number to Toggle the correct image based on Direction

*/

/*
Mario Left
I# cdir   Image
1    2    MarioL1
2    2    MarioL2
3    2    MarioL3
*/
if (cdir == 2) { // only display image based on current direction
  if (image == 1) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioL1); //   Rolling Mario image 1  
  }else 
  if (image == 2) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioL2); //   Rolling Mario image 1   
  }else 
  if (image == 3) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioL3); //   Rolling Mario image 1  
  }
 } else
/*
Mario Right
I# cdir    Image
1    1     MarioR1
2    1     MarioR2
3    1     MarioR3
*/  
if (cdir == 1) { // Mario Right
  if (image == 1) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioR1); //    Mario image 1  
  }else 
  if (image == 2) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioR2); //    Mario image 1   
  }else 
  if (image == 3) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioR3); //    Mario image 1  
  }
 } else

if (cdir == 2) { // Mario Left
  if (image == 1) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioL1); //    Mario image 1  
  }else 
  if (image == 2) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioL2); //    Mario image 1   
  }else 
  if (image == 3) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioL3); //    Mario image 1  
  }
 } else

if (cdir == 3) { // Mario Jump based on previous direction
  if (prevdir == 1) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioL3); //   Face left  
  }else {
    myGLCD.drawBitmap(x, y, 16, 16, MarioR3); //   Otherwise face right  
  }
 } else
if (cdir == 4) { // Right Jump 
    myGLCD.drawBitmap(x, y, 16, 16, MarioR3); //   Otherwise face right  
 } else
if (cdir == 5) { // Left Jump 
    myGLCD.drawBitmap(x, y, 16, 16, MarioL3); //   Face left  
 } else
if (cdir == 6) { // Climb Down 
  if (image == 1) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioU4); //    Mario climb left
  }else 
  if (image == 2) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioU3); //    Mario climb right  
  }else 
  if (image == 3) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioU2); //    Mario top of ladder  left
  } else
  if (image == 4) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioU1); //    Mario top of ladder  right
  }
 } else
if (cdir == 7) { // Climb Up
  if (image == 1) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioU1); //    Mario climb left
  }else 
  if (image == 2) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioU2); //    Mario climb right  
  }else 
  if (image == 3) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioU3); //    Mario top of ladder  left
  } else
  if (image == 4) {
    myGLCD.drawBitmap(x, y, 16, 16, MarioU4); //    Mario top of ladder  right
  }
 } else
if (cdir == 8) { // Stop and stand straignt 
    myGLCD.drawBitmap(x, y, 16, 16, MarioStop); //   Face left  
 } 
}

// ********************************************
// Redraw damage to screen during Mario Jump
void redraw(int x, int y, int d) {
  
if (y < 134) { // Level 4

  if (x < 68){             // Sector 0
   
    myGLCD.drawBitmap (15, 96, 10, 15, Ladder1); 
    myGLCD.drawBitmap (15, 108, 10, 15, Ladder1); 
  
  }
  
    if (x>=252){             // Above Sector 3

    myGLCD.drawBitmap (275, 134, 23, 8, DK_scaffold); 
    
    myGLCD.drawBitmap (280, 141, 10, 15, Ladder1); 

  
  }

} else
if (y < 168 && y > 134) { // Level 3




  if (x>=68 && x <= 182){ // Sector 1


    myGLCD.drawBitmap (91, 126, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (114, 127, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (137, 128, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (160, 129, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (183, 130, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (206, 131, 23, 8, DK_scaffold); 

    
    myGLCD.drawBitmap (150, 136, 10, 15, Ladder1); 
    myGLCD.drawBitmap (150, 148, 10, 15, Ladder1); 


  }
  if (x>=183 && x <= 251){ // Sector 2

    myGLCD.drawBitmap (160, 129, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (183, 130, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (206, 131, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (229, 132, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (252, 133, 23, 8, DK_scaffold); 

    myGLCD.drawBitmap (280, 141, 10, 15, Ladder1); 

  
  }
  if (x>=252){             // Sector 3

    myGLCD.drawBitmap (229, 132, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (252, 133, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (275, 134, 23, 8, DK_scaffold); 
    
    myGLCD.drawBitmap (280, 141, 10, 15, Ladder1); 

  
  }


} else
if (y <= 205 && y > 170) { // Level 2

  if (x<45){                // Sector 4

    myGLCD.drawBitmap (22, 168, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (45, 167, 23, 8, DK_scaffold); 
    
    myGLCD.drawBitmap (32, 176, 10, 15, Ladder1); 
  
  }
  if (x>=45 && x <= 113){    // Sector 5

    myGLCD.drawBitmap (22, 168, 23, 8, DK_scaffold);   
    myGLCD.drawBitmap (32, 176, 10, 15, Ladder1); 

    myGLCD.drawBitmap (45, 167, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (68, 166, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (91, 165, 23, 8, DK_scaffold);   
    myGLCD.drawBitmap (114, 164, 23, 8, DK_scaffold);     
  }
  if (x>=114 && x <= 183){   // Sector 6
  
    myGLCD.drawBitmap (91, 165, 23, 8, DK_scaffold);   
    myGLCD.drawBitmap (114, 164, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (137, 163, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (160, 162, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (183, 161, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (206, 160, 23, 8, DK_scaffold); 


    myGLCD.drawBitmap (134, 170, 10, 15, Ladder1); 
    myGLCD.drawBitmap (134, 182, 10, 15, Ladder1);   
  }
  if (x>=184 && x <= 251){   // Sector 7

    myGLCD.drawBitmap (160, 162, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (183, 161, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (206, 160, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (229, 159, 23, 8, DK_scaffold);   

  }

} else
if (y > 205) { // Level 1

  if (x<45){                // Sector 8

    myGLCD.drawBitmap (22, 192, 23, 8, DK_scaffold);   
    myGLCD.drawBitmap (45, 193, 23, 8, DK_scaffold); 
  }
  if (x>=45 && x <= 113){    // Sector 9

    myGLCD.drawBitmap (22, 192, 23, 8, DK_scaffold);   
    myGLCD.drawBitmap (45, 193, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (68, 194, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (91, 195, 23, 8, DK_scaffold);    
    myGLCD.drawBitmap (114, 196, 23, 8, DK_scaffold); 


  }
  if (x>=114 && x <= 183){   // Sector 10

    myGLCD.drawBitmap (91, 195, 23, 8, DK_scaffold);    
    myGLCD.drawBitmap (114, 196, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (137, 197, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (160, 198, 23, 8, DK_scaffold);  
    myGLCD.drawBitmap (183, 199, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (206, 200, 23, 8, DK_scaffold); 

  }
  if (x>=184 && x <= 251){   // Sector 11

    myGLCD.drawBitmap (160, 198, 23, 8, DK_scaffold);  
    myGLCD.drawBitmap (183, 199, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (206, 200, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (229, 201, 23, 8, DK_scaffold);    
    myGLCD.drawBitmap (252, 202, 23, 8, DK_scaffold); 

  }
  if (x>=252){ // Sector 12

    myGLCD.drawBitmap (183, 199, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (206, 200, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (229, 201, 23, 8, DK_scaffold);    
    myGLCD.drawBitmap (252, 202, 23, 8, DK_scaffold); 
    myGLCD.drawBitmap (275, 203, 23, 8, DK_scaffold); 

    myGLCD.drawBitmap (280, 210, 10, 15, Ladder1);   
  }
  
 } 
  
}



 // ================= Decimal to BCD converter

byte decToBcd(byte val) {
  return ((val/10*16) + (val%10));
} 
