/****************************************************

Arduino code for Bali Cuckoo Clock, an interactive art "wonderbox"
for the Pataphysical Slot Machine.

About Bali Cuckoo Clock:
http://bit.ly/bali-cuckoo-clock

Interactive specification:
http://bit.ly/wonderbox-spec

Wonderbox schematics:
http://bit.ly/wonderbox-schema

About the Pataphysical Slot Machine:
http://pataphysics.us

Last updated on February 6, 2015.

Written by Fabrice Florin, based on free libraries from Arduino and Adafruit. 
Sound playback code by Donald Day and Tim Pozar.

This free software is licensed under GPLv2.
  
 ****************************************************/
 
#include <Servo.h> // for controlling servo motors
// #include <Wire.h> // to connnect with other boxes with the i2c protocol, for sound playback
#include <I2C.h>

// Connect the red wire from the pataphysical bus to the 5V pin on Arduino
// Connect the black wire from the pataphysical bus to any ground pin on Arduino
// Connect the green wire from the pataphysical bus to Analog 5 (i2c clock) 
// Connect the blue wire from the pataphysical bus to Analog 4 (i2c data) 

const int box_button = 2; // the switch for the whole box is on pin 2 -- it is triggered when you open the box 
const int temple_outside_light_left = 3; // temple left light LED is on pin 3
const int temple_outside_light_right = 4; // temple right light LED is on pin 4
const int temple_inside_light = 5; // temple inside light LEDs is on pin 5 
const int garuda_front_light = 6; // garuda front light LED is on pin 6 
const int garuda_eyes = 7; // garuda_eyes LEDs is on pin 7 
// const int temple_back_light = n/a; // temple back light LED (rainbow) is on the 3.3V pin on Arduino

Servo servo1;  // create 1st servo object (Garuda moves back and forth - HiTech HS 785HB winch servo - pin 8)  
Servo servo2;  // create 2nd servo object (Garuda right wing - pin 9)  
Servo servo3;  // create 3rd servo object (Garuda left wing - pin 10)
Servo servo4;  // create 4th servo object (Garuda head - pin 11)
Servo servo5;  // create 5th servo object (right door - pin 12)
Servo servo6;  // create 6th servo object (left door - pin 13)

int doors = 0;    // generic variable to store door positions for moving them: closed (0) to open (180) 
int doorstatus = 0;    // generic variable for current door status: closed (0) or open (1) 
int loc = 0;    // generic variable to store Garuda's location: angle between 93 and 112 
int dest = 0;    // generic variable to store Garuda's destination angle: front (e.g.: 93) or back (e.g.: 112)
int dest_front = 93; // Garuda's front destination angle
int dest_back = 112; // Garuda's back destination angle 
int pos = 0;    // generic variable to store Garuda's head positions: left (0) to right (45) 
int head_pos_left = 0;    // Garuda's left head position 
int head_pos_right = 45;    // Garuda's right head position 
int wing_pos = 0;    // generic variable to store Garuda's wing positions: back (0) to front (60) 
int wing_pos_back = 0;    // Garuda's back wing position 
int wing_pos_front = 75;    // Garuda's front wing position 

int garudaState = 0; // set to "0" if Garuda is asleep. He needs to wait until someone opens the box (which releases the button and starts the sequence).
int buttonState = 0;         // variable for reading the button status
int lastButtonState = 0;      // for tracking button changes
int songValue = 12;  // Play Track 12 (Box #12), see tracks list below
int quietValue = 0;  // TRK0 means stop playing
  
 // Pataphysical Tracks List:
 // http://bit.ly/pata-tracks-list
 
unsigned long garuda_start;
unsigned long garuda_time;
// unsigned long garuda_act_total_duration;


//********************  SETUP    **********************

void setup() 
{ 

pinMode(box_button, INPUT_PULLUP);   // initialize the button's digital pin as an input. 
pinMode(temple_outside_light_left, OUTPUT);   // initialize the left temple light's digital pin as an output. 
pinMode(temple_outside_light_right, OUTPUT);   // initialize the right temple light's digital pin as an output. 
pinMode(temple_inside_light, OUTPUT);   // initialize the temple inside light's digital pin as an output. 
pinMode(garuda_front_light, OUTPUT);   // initialize Garuda front light digital pin as an output. 
pinMode(garuda_eyes, OUTPUT);   // initialize Garuda eye lights digital pin as an output. 

servo1.attach(8);   // attaches the servo 1 on pin 8 to the servo object that makes Garuda move
servo2.attach(9);   // attaches the servo 2 on pin 9 to the servo object that makes the right wing flap
servo3.attach(10);  // attaches the servo 3 on pin 10 to the servo object that makes the left wing flap 
servo4.attach(11);  // attaches the servo 4 on pin 11 to the servo object that makes Garuda's head spin
servo5.attach(12);  // attaches the servo 5 on pin 12 to the servo object that makes the right door move 
servo6.attach(13);  // attaches the servo 6 on pin 13 to the servo object that makes the left door move

servo1.write(112); // tell servo1 (Garuda moves) to go back
servo3.write(0);  // makes the left wing flap to the back
// servo3.writeMicroseconds(1500);  // set servo to mid-point?
servo5.write(180); // tell servo 5 (right door) to close
servo6.write(0);   // tell servo 6 (left door) to close


Serial.begin(9600); // enables serial connection

Serial.println("=== SETUP"); // to check we're doing the right thing in setup
loc = servo1.read(); 
Serial.print("Current location is: ");
Serial.print(loc);
Serial.println(" (back) ");
  
Serial.print("Setup Garuda state: ");
Serial.println(garudaState);

Serial.println(" ");
 
 // Setup Sound Server (i2c)
   delay(200*songValue); // wait a few seconds to call the sound server
  I2c.begin();   
    
} 


//********************  MAIN LOOP    **********************

/* Sequence of loop events:
0. Garuda sleeps (default state)
1. Open box (button release)
2. Play sound / turn lights on
3. Open doors
4. Garuda moves forward
5. Garuda performs (turn head, blink eyes, wings forward/back)
6. Garuda moves backwards
7. Close doors
8. Wait for button (if you want to restart this sequence)
9. Close box (button is pressed, which puts Garuda back to sleep)

Notes: Each of the numbers above corresponds to a different state, tracked by the garudaState variable.
Any button press can change this sequence, see buttonCheck() function below.
*/


void loop() 
{ 
  
   Serial.println("=== New Loop");  // to check we're doing the right thing in setup

    locationCheck(); // Check to see where Garuda is

    buttonCheck(); // Check to see if the button has been pressed or released
   
    if (garudaState == 0) // Garuda is asleep. He is waiting for someone to open the box (which releases the button)
    {  

    buttonCheck(); // check to see if the button is pressed or released
    
    Serial.println("Garuda is sleeping. He is waiting for a button release (garudaState is 0).");  // explain that we're stopped because Garuda is asleep.

    }
 
    if (garudaState == 8) // Garuda is done, waiting for someone to press and release the button.
    {  

    buttonCheck(); // check to see if the button is pressed AND released
    
    Serial.println("Garuda is done, waiting for someone to press and release the button (garudaState is 8).");  // explain that we're waiting because Garuda is done.

    }
 
     if (garudaState == 1)  // if the button has been released, make Garuda start his act.  // STANDARD START LOOP
     { 
       
     turnTempleLightsOn();
     
     flapWingsBackward();  
   
     playSound() ; // This will play the sound.

       if ( loc == dest_back )   // STANDARD LOOP 1: GARUDA IS IN THE BACK, TELL HIM TO OPEN DOORS, THEN MOVE TO THE FRONT (OR CLOSE DOORS IF HE'S DONE) 
       {

         if (doorstatus == 0) // Doors are closed, open them and have Garuda move to front
         {
 
         openDoors();
   
         turnFrontLightOn();
  
         moveForward();
          
         turnInsideLightsOn();
 
         garudaActions(); 

         } 
       }
    
       locationCheck(); // Check to see where Garuda is
   
       if ( loc == dest_front )     // STANDARD LOOP 2: GARUDA IS IN THE FRONT, TELL HIM TO MOVE TO THE BACK, THEN CLOSE DOORS
       {
 
         moveBackward();
 
         turnFrontLightOff();

         closeDoors();
  
         turnInsideLightsOff();
       
       }
     }
  
} // END LOOP 
 

//********************  BUTTON CHECK    **********************

void buttonCheck() // CHECK BUTTON: Check if the button has been pressed, and respond accordingly, based on Garuda's state (tracked in garudaState).

{
  
  //  Button actions: 
  //  START: if you release the button by opening the box, make Garuda come out and play (garudaState = 1)
  //  STOP: if you press the button anytime after the main sequence has started, make Garuda stop and go back in the temple (garudaState >=2 && garudaState <=7)
  //  RE-START: if you press AND release the button anytime after the main sequence has ended, make Garuda come out and play again as if we had opened the box (garudaState = 8)
  //  SLEEP: if the button is pressed (but not released) by closing the box, put Garuda to sleep and do nothing (the default state when the wonderbox door is closed: garudaState == 9)

  lastButtonState = buttonState;
  
  buttonState = digitalRead(box_button);
  
  if (buttonState == LOW && garudaState == 0 )  // if button was released by opening the box while Garuda was sleeping, make him start his act.  // STANDARD LOOP
  { 
        
  garudaState = 1; // Garuda is ready to START
      
  Serial.println("Garuda is ready to START (garudaState = 1).");  // Confirm that Garuda is ready to START
  Serial.println(" "); // add blank line
   
  }
  
  if (buttonState != lastButtonState) // Check to see if the button state has changed.
  {         

   Serial.println("Button state just changed.");  // report the button state change
   Serial.print("Button state is now: ");  // print the button state
   Serial.print(buttonState);  // print the button state
   Serial.print(". Last button state was: ");  // print the button state
   Serial.println(lastButtonState);  // print the button state  
   Serial.println(" "); // add blank line
   Serial.print("Garuda's state is now: ");  // print the button state
   Serial.println(garudaState);  // print the button state
   Serial.println(" "); // add blank line

   // Check if the button has been pressed or released.
   // If it has been pressed, the button state should be HIGH (for a Normally Open button set as Input with a Pullup resistor).
   // If it has been released, the button state should be LOW (for a Normally Open button set as Input with a Pullup resistor).
   // Next, we will determine what action should take place, based on Garuda's state (tracked in garudaState).

      if (buttonState == HIGH && garudaState >=2 && garudaState <=7) // STOP BUTTON LOOP: if button is pressed during main sequence, stop everything, send Garuda to back and close doors.
      {  
          
      Serial.println("Stop everything because button was pressed during the main sequence.");  // explain why we're stopping.

      blinkTempleLights();

      moveBackward();
  
      turnFrontLightOff();

      closeDoors();
 
      stopSound() ; // This will stop the sound.
 
      turnInsideLightsOff();
     
      turnTempleLightsOff();
 
      garudaState = 8; // Garuda now has to wait for the button to be pressed and released to start again -- or go to sleep if it's only pressed but not released

      delay(2000); // wait a couple seconds for the button to be released, so it doesn't trigger the sequence again
      
      return;

      }
      else if (buttonState == HIGH && garudaState == 8 )  // if button was pressed after Garuda's sequence ends, check to see if it's released to re-start it, or pressed after closing the box.
      {  
        
        stopSound() ; // This will stop the sound.

        delay(2000); // wait a couple seconds for the button to be released, so it doesn't trigger the sequence again

     
        if (buttonState == LOW && garudaState == 8 )  // if button was released by opening the box while Garuda was sleeping, make him start his act.  // STANDARD LOOP
        { 
        garudaState = 1; // Garuda is ready to RE-START
      
        Serial.println("Garuda is ready to RE-START (garudaState = 1).");  // Confirm that Garuda is ready to RE-START
        Serial.println(" "); // add blank line
        
        blinkTempleLights();
        delay(100);
        blinkTempleLights();
   
        }
        else if (buttonState == HIGH && garudaState == 8 )  // the button is still pressed (was not released to re-start), so we assume the box is now closed.
        {
       
         
      stopSound() ; // This will stop the sound.

      garudaState = 0; // Garuda is now going to sleep      
          
      turnTempleLightsOff();

        Serial.println("Garuda is now going to sleep (garudaState = 0).");  // Confirm that Garuda is now going to sleep
        Serial.println(" "); // add blank line
         
        }
      }

      Serial.print("Garuda's state is now: ");  // print the button state
      Serial.print(garudaState);  // print the button state
      Serial.println(" "); // add blank line

   }
      
   lastButtonState = buttonState;
   delay(20);  
  
} // end buttonCheck


//********************  PLAY SOUND    **********************

void playSound() // PLAY SOUND: This will play a sound, if all goes well, using the i2c expander, for sound playback

  {
  
    I2c.write(32, 9, songValue);

 // Play track 12, using the songValue variable. 
 // The file name is actually 'TRK12.MP3', but we're only sending the song value '12'.
 // The sound server will fill in 'TRK' and '.mp3' for us.
  
  garudaState = 2;

  Serial.print("We are now playing the sound. (garudaState is 2).");
  Serial.println(" ");


  } // end playSound


void stopSound() // STOP SOUND

  {
  
  I2c.write(32, 9, quietValue);
  // Play track 0, using the quietValue variable. 
  // The file name is actually 'TRK0.MP3', but we're only sending the song value '0'.
 
  Serial.print("We are now stopping the sound.");
  Serial.println(" ");


  } // end stopSound


//********************  LOCATION CHECK    **********************

void locationCheck() // CHECK LOCATION: Check to see where Garuda is, and update the loc variable so it can be used anywhere.
{
  
   loc = servo1.read(); 
   Serial.print("Current location is: ");
   Serial.print(loc);
   if ( loc == dest_back ) { Serial.println(" (back) "); }
   else { Serial.println(" (front) "); }
   Serial.println(" ");

} // end locationCheck


//********************  GARUDA MOVES    **********************


void moveForward()
{
  dest = dest_front;
  
  buttonCheck(); // check to see if the button is pressed
  
  servo1.attach(8);  // attaches the servo when Garuda is ready to roll

  delay(2000); 
      
  Serial.print("Going to the ");
  Serial.print("front: ");  // tell us which direction Garuda is expected to be going   
  Serial.println(dest);  // tell us which direction Garuda is expected to be going   
 
  servo1.write(dest);              // tell servo1 to go in the direction it’s been told (front or back)

  delay(2000); 

  Serial.println("Garuda is in the front");
  
  garudaState = 5;

  Serial.print("Garuda has moved forward. He can now perform his act (garudaState is 5).");
  Serial.println(" ");


} // end moveForward


void moveBackward()
{
  dest = dest_back;
 
  blinkEyes();
   
  delay(2000); 
  
  blinkEyes();
   
  Serial.print("Going to the ");
  Serial.print("back: ");  // tell us which direction Garuda is expected to be going   
  Serial.println(dest);  // tell us which direction Garuda is expected to be going  

  servo1.write(dest);              // tell servo1 to go in the direction it’s been told (front or back)

  blinkEyes();
     
  delay(2000); 

  blinkEyes();

  Serial.println("Garuda is in the back");
  
  servo1.detach();  // detaches the servo to avoid straining it while Garuda sleeps
  
  garudaState = 7;

  Serial.print("Garuda has moved backward. We can now close the doors (garudaState is 7).");
  Serial.println(" ");


} // end moveBackward



//********************  GARUDA ACTIONS    **********************

void garudaActions() // Garuda does his act (flaps its wings, shakes its head and sings advice) 

{ 
  
 if (dest == dest_front) // If Garuda is going to the front
 {

    garuda_start = millis(); // Garuda start time
    garuda_time = 0; //  Garuda's time spent since start
   // garuda_act_total_duration = 8000; // total time of the Garuda act 
 
    Serial.println(" "); // add a blank line
    Serial.print("Start Garuda actions. The time is now: "); // tell us when Garuda loop is invoked
    Serial.println(garuda_time); // tell how much time Garuda has used so far
    
   while (garuda_time <= 8000) // previously used garuda_act_total_duration
   {
    
    buttonCheck(); // check to see if the button is pressed
    
    shakeHeadLeft();

    shakeHeadRight();

    flapWingsForward();
    
    flapWingsBackward();
   
    shakeHeadLeft();

    shakeHeadRight();

    garuda_time = millis() - garuda_start; //  Garuda's time spent since start

    } //end while

    Serial.print("End Garuda actions. Time spent (milliseconds):"); // tell us when Garuda loop ends
    Serial.println(garuda_time); // tell how much time Garuda has used so far
    Serial.println(" "); // add a blank line
  }

  garudaState = 6;

  Serial.print("Garuda has performed his act. He can now  move back (garudaState is 6).");
  Serial.println(" ");


} // end garudaActions


//********************  HEAD    **********************

void shakeHeadLeft()
{

  Serial.println("Garuda shakes his head left"); // tell us when Garuda left head shake loop is invoked

  servo4.attach(11);  // attaches the servo on pin 11 to the servo object that makes Garuda's head shake

  for(pos = 0; pos < 60; pos += 2)  // GARUDA SHAKES HIS HEAD LEFT - servo 4 goes from 0 degrees to 60 degrees in steps of 1 degrees 
   {                                  // 
     servo4.write(pos);              // tell servo 4 (Garuda head) to go to position in variable 'pos' 

     blinkEyes();  
   } 
   
   servo4.detach();  // detaches the servo to avoid straining it while Garuda sleeps
    
} // end shakeHeadLeft


void shakeHeadRight()
{
  Serial.println("Garuda shakes his head right"); // tell us when Garuda right head shake loop is invoked

 servo4.attach(11);  // attaches the servo on pin 11 to the servo object that makes Garuda's head shake

  for(pos = 60; pos >=1; pos -=2)     // GARUDA SHAKES HIS HEAD RIGHT - servo 4 goes from 60 degrees to 0 degrees in steps of 1 degrees 
  {                                  //  
     servo4.write(pos);              // tell servo 4 (Garuda head) to go to position in variable 'pos' 
    
     blinkEyes();
   }  
   
   servo4.detach();  // detaches the servo to avoid straining it while Garuda sleeps

} // end shakeHeadRight


//********************  WINGS    **********************

void flapWingsForward()
{
  
  Serial.println("Garuda flaps wings forward"); // tell us when Garuda forward wing loop is invoked

  servo2.attach(9);  // attaches the servo 2 on pin 9 to the servo object that makes the right wing flap
  servo3.attach(10);  // attaches the servo 3 on pin 10 to the servo object that makes the left wing flap 

   for(wing_pos = wing_pos_front; wing_pos >= wing_pos_back; wing_pos -=2)     // GARUDA FLAPS HIS WINGS FORWARD - servos go from about 75 degrees to 0 degrees in steps of 2 degrees 
  {                                      
    servo2.write(wing_pos);              // tell servo 2 (Garuda right wing) to go to position in variable 'wing_pos'
    servo3.write(-(wing_pos - wing_pos_front));        // tell servo 3 (Garuda left wing) to go to position in variable 'wing_pos'
  
   blinkEyes();
   } 
          
   servo2.detach();  // detaches the servo to avoid straining it while Garuda sleeps
   servo3.detach();  // detaches the servo to avoid straining it while Garuda sleeps

} // end flapWingsForward


void flapWingsBackward()
{
  Serial.println("Garuda flaps wings backward"); // tell us when Garuda backward wing loop is invoked

  servo2.attach(9);  // attaches the servo 2 on pin 9 to the servo object that makes the right wing flap
  servo3.attach(10);  // attaches the servo 3 on pin 10 to the servo object that makes the left wing flap 

  for(wing_pos = wing_pos_back; wing_pos < wing_pos_front; wing_pos += 2)  // GARUDA FLAPS HIS WINGS BACKWARD - servos go from 0 degrees to about 75 degrees in steps of 2 degrees 
  {                                      
    servo2.write(wing_pos);              // tell servo 2 (Garuda right wing) to go to position in variable 'wing_pos' 
    servo3.write((wing_pos_front - wing_pos));         // tell servo 3 (Garuda left wing) to go to position "75" minus variable 'wing_pos' 
 
   blinkEyes();
   } 
                
   servo2.detach();  // detaches the servo to avoid straining it while Garuda sleeps
   servo3.detach();  // detaches the servo to avoid straining it while Garuda sleeps

} // end flapWingsBackward



//********************  DOORS    **********************

void openDoors()
{

  if (doorstatus == 0)        // OPEN DOOR LOOP
  {
 
    
 buttonCheck(); // check to see if the button is pressed
    
 servo5.attach(12);  // attaches the servo on pin 12 when doors are ready to open 
 servo6.attach(13);  // attaches the servo on pin 13 when doors are ready to open 
    
  Serial.println("The door is now closed: 0"); // tell us if the door was "opened" ("1") or "closed" ("0")
  Serial.println("Opening the door"); // "opening door"
  Serial.println(" "); // add a blank line

     for(doors = 180; doors >=0; doors -=2)     // servos go from 180 degrees to 0 degrees in steps of 1 degrees 
     {                                  // except servo 6 that goes from 0 degrees to 180 degrees 
     servo5.write(doors);              // tell servo 5 (right door) to go to position in variable 'doors'
     servo6.write(-(doors-180));        // tell servo 6 (left door) to go to position in variable 'doors'
     delay(50);                       // waits 50ms for the servo to reach the position 
     }
  
  doorstatus = 1;

  garudaState = 4;

  Serial.print("Doors are open. Garuda can go out now (garudaState is 4).");
  Serial.println(" ");

  }

} // end openDoors()



void closeDoors()
  
{
  
  if (doorstatus == 1)        // CLOSE DOOR LOOP
  {

  Serial.println("The door is now open: 1"); // tell us if the door was "opened" ("1") or "closed" ("0")
  Serial.println("Closing the door"); // "closing door"
  Serial.println(" "); // add a blank line
  
     for(doors = 0; doors <=180; doors +=2)     // servos go from 0 degree to 180 degrees in steps of 1 degrees 
     {                                  // except servo 6 that goes from 0 degrees to 180 degrees 
     servo5.write(doors);              // tell servo 5 (right door) to go to position in variable 'doors'
     servo6.write(-(doors-180));        // tell servo 6 (left door) to go to position in variable 'doors'

     delay(50);                       // waits 50ms for the servo to reach the position 
     }

  doorstatus = 0;

  garudaState = 8;

  Serial.print("Doors are closed. Garuda needs to wait until the button is released again (garudaState is 8).");
  Serial.println(" ");

  servo5.detach();  // detaches the servo to avoid straining it while Garuda sleeps
  servo6.detach();  // detaches the servo to avoid straining it while Garuda sleeps

  }
    
} // end closeDoors()


//********************  LIGHTS    **********************


void blinkTempleLights()
{ 
    
     turnFrontLightOn();
     delay (50);
     turnFrontLightOff();   
     delay (50);   
     turnFrontLightOn();
     delay (50);
     turnFrontLightOff();   

} // end blinkTempleLights


void blinkEyes()
{ 
    if ( doorstatus == 1 ) // only blink if the doors are open
    { 
     
    buttonCheck(); // check to see if the button is pressed
    
     turnEyesOn();
     delay (50);
     turnEyesOff();   
    }
} // end blinkEyes


void turnEyesOn()
{
  digitalWrite(garuda_eyes, HIGH);   // turn the LED on (HIGH is the voltage level)
} // end turnEyesOn


void turnEyesOff()
{
 digitalWrite(garuda_eyes, LOW);    // turn the LED off by making the voltage LOW
} // end turnEyesOff


void turnFrontLightOn()
{
  digitalWrite(garuda_front_light, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println("Garuda front light is on");
  
} // end turnEyesOn


void turnFrontLightOff()
{
 digitalWrite(garuda_front_light, LOW);    // turn the LED off by making the voltage LOW
   Serial.println("Garuda front light is off");
} // end turnEyesOff

void turnInsideLightsOn()
{
  digitalWrite(temple_inside_light, HIGH);   // turn the LED on (HIGH is the voltage level)
  
} // turnInsideLightsOn


void turnInsideLightsOff()
{
 digitalWrite(temple_inside_light, LOW);    // turn the LED off by making the voltage LOW
} // end turnInsideLightsOff


void turnTempleLightsOn()
{
  digitalWrite(temple_outside_light_left, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(temple_outside_light_right, HIGH);   // turn the LED on (HIGH is the voltage level)
      
  garudaState = 3;

  Serial.print("Lights are now turned on. Doors can open (garudaState is 3).");
  Serial.println(" ");

} // end turnTempleLightsOn


void turnTempleLightsOff()
{
 digitalWrite(temple_outside_light_left, LOW);    // turn the LED off by making the voltage LOW
 digitalWrite(temple_outside_light_right, LOW);    // turn the LED off by making the voltage LOW
} // end turnTempleLightsOff

