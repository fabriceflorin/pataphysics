/****************************************************

Arduino code for Ask Ubu, an interactive art "wonderbox"
for the Pataphysical Slot Machine.

Interactive specification:
http://bit.ly/wonderbox-spec

Wonderbox schematics:
http://bit.ly/wonderbox-schema

About the Pataphysical Slot Machine:
http://pataphysics.us

Last updated on September 27, 2015.

Written by Fabrice Florin, based on free libraries from Arduino and Adafruit, with guidance by Donald Day.

This free software is licensed under GPLv2.

It includes code for the Adafruit Motor Shield v2, which we are using for this wonderbox.
---->	http://www.adafruit.com/products/1438

This sketch creates a fun motor party on your desk, driving DC motors, Servo and Step Motors all at once.

It also includes code from the Adafruit_MCP23008 library for eventual sound playback (now commented out):

https://github.com/adafruit/Adafruit-MCP23008-library

 ****************************************************/

#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <Servo.h>

/* Commenting out i2c server code for now, to avoid conflict with Motorshield
#include <Wire.h>
#include "Adafruit_MCP23008.h" // to connnect with the i2c expander, for sound playback

// Connect the red wire from the pataphysical bus to the 5V pin on Arduino
// Connect the black wire from the pataphysical bus to any ground pin on Arduino
// Connect the green wire from the pataphysical bus to Analog 5 (i2c clock)
// Connect the blue wire from the pataphysical bus to Analog 4 (i2c data)
*/


// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Connect a DC motor to port M1
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);

// We'll also test out the built-in Arduino Servo library
Servo servo1;


const int box_button = 2; // the switch for the whole box may be placed on pin 2 -- it is triggered when you open the box.
int buttonState = 0;         // variable for reading the button status
int lastButtonState = 0;      // for tracking button changes

int ubuState = 0; // set to "0" if Ubu is asleep. He needs to wait until someone opens the box (which releases the button and starts the sequence).

const int outside_light_left = 3; // temple left light LED is on pin 3
const int outside_light_right = 4; // temple right light LED is on pin 4
const int front_light = 5; // ubu front light LED is on pin 5
const int inside_light = 6; // temple inside light LEDs is on pin 6
const int ubu_eyes = 7; // ubu_eyes LEDs is on pin 7
int pos = 0;    // generic variable to store Ubu's head positions: left (0) to right (45)

unsigned long ubu_start;
unsigned long ubu_time;

int SoundPlaybackState = 0; // set to "0" by default, so the sound will play the first time.
int songValue = 15;  // Play Track 15 (Box #15)
int quietValue = 0;  // TRK0 means stop playing



//********************  SETUP    **********************

void setup()
{

  pinMode(box_button, INPUT);   // initialize the button's digital pin as an input.
  pinMode(outside_light_left, OUTPUT);   // initialize the left temple light's digital pin as an output.
  pinMode(outside_light_right, OUTPUT);   // initialize the right temple light's digital pin as an output.
  pinMode(inside_light, OUTPUT);   // initialize the temple inside light's digital pin as an output.
  pinMode(front_light, OUTPUT);   // initialize Ubu front light digital pin as an output.
  pinMode(ubu_eyes, OUTPUT);   // initialize Ubu eye lights digital pin as an output.


  Serial.begin(9600);           // set up Serial library at 9600 bps
  // Serial.println("MMMMotor party!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz

  // Attach the head turn servo to pin #10
  servo1.attach(10);

  // turn on motor M1
  myMotor->setSpeed(200);
  myMotor->run(RELEASE);

    servo1.write(30);              // tell servo 4 (Ubu head) to go to position in variable 'pos'

/* Commenting out i2c server code for now, to avoid conflict with Motorshield
  delay(5000); // wait before calling the sound server
  mcp.begin();      // use default address 0, based at 0x20
 */
 
}


//********************  MAIN LOOP    **********************


/* Sequence of loop events:
0. Ubu sleeps (default state, lights are on)
1. Button is clicked
2. Play sound (eventually)
3. Spin wheel forward (DC motor)
4. Ubu wakes up, shakes his head (blink eyes, servo turns left and right)
5. Spiral gidouille spins backward, then stops
6. Wait for button (if you want to restart this sequence)

Notes: Each of the numbers above corresponds to a different state, tracked by the ubuState variable.
Any button press can change this sequence, see buttonCheck() function below.

*/


void loop()
{


  Serial.println("=== New Loop");  // to check we're doing the right thing in setup


  buttonCheck(); // Check to see if the button has been pressed or released

  if (ubuState == 0) // Ubu is asleep. He is waiting for someone to open the box (which releases the button)
  {
    
    turnInsideLightsOn();
    
    buttonCheck(); // check to see if the button is pressed or released

    Serial.println("Ubu is sleeping. He is waiting for a button release (ubuState is 0).");  // explain that we're stopped because Ubu is asleep.

  }

  if (ubuState == 6) // Ubu is done, waiting for someone to press and release the button.
  {

    buttonCheck(); // check to see if the button is pressed or released

    Serial.println("Ubu is done, waiting for someone to press and release the button (ubuState is 6).");  // explain that we're waiting because Ubu is done.

  }

  if (ubuState == 1)  // if the button has been released, turn on the lights.
  {

    turnBoxLightsOn();

  }


  if (ubuState == 2)  // Once the lights are turned on, play sound (eventually)
  {

    // playSound() ; // This will play the sound, when the code is ready.

  ubuState = 3; // temporarily make it go to the next stage (remove this after we uncomment playSound)

  }

  if (ubuState == 3) // Time to spin the wheel forward
  {

    spinWheelForward();

  }


  if (ubuState == 4) // Time for Ubu to perform
  {

    ubuActions();

  }

  if (ubuState == 5) // Time to spin the wheel backward
  {

    spinWheelBackward();


  }

  if (ubuState == 6) // Ubu is done, time to put him to sleep, wait for a button click.
  {

    // turnInsideLightsOff();

  }


} // END LOOP




//********************  BUTTON CHECK    **********************

void buttonCheck() // CHECK BUTTON: Check if the button has been pressed, and respond accordingly, based on Ubu's state (tracked in ubuState).

{

  //This is probably more complicated than it needs to be, will streamline later.

  //  Button actions:
  //  START: if you release the button by opening the box, make Ubu come out and play (ubuState = 1)
  //  STOP: if you press the button anytime after the main sequence has started, make Ubu stop and go back in the temple (ubuState >=2 && ubuState <=7)
  //  RE-START: if you press AND release the button anytime after the main sequence has ended, make Ubu come out and play again as if we had opened the box (ubuState = 8)
  //  SLEEP: if the button is pressed (but not released) by closing the box, put Ubu to sleep and do nothing (the default state when the wonderbox door is closed: ubuState == 9)

  lastButtonState = buttonState;

  buttonState = digitalRead(box_button);

  if (buttonState == LOW && ubuState == 0 )  // if button was released while Ubu was sleeping, make him start his act.  // STANDARD LOOP
  {

    ubuState = 1; // Ubu is ready to START

    Serial.println("Ubu is ready to START (ubuState = 1).");  // Confirm that Ubu is ready to START
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
    Serial.print("Ubu's state is now: ");  // print the button state
    Serial.println(ubuState);  // print the button state
    Serial.println(" "); // add blank line

    blinkLights(); // to confirm that the button state has changed.

    // Next, we will determine what action should take place, based on Ubu's state (tracked in ubuState).

    if (buttonState == LOW && ubuState >= 2 && ubuState <= 7) // STOP BUTTON LOOP: if button is pressed during main sequence, stop everything, put Ubu to sleep and stop spinning wheel.
     {

      Serial.println("Stop everything because button was pressed during the main sequence.");  // explain why we're stopping.

     // blinkLights();

     // turnFrontLightOff();

     // turnInsideLightsOff();

     // turnBoxLightsOff();

      ubuState = 6; // Ubu now has to wait for the button to be pressed and released to start again -- or go to sleep if it's only pressed but not released

      delay(2000); // wait a couple seconds for the button to be released, so it doesn't trigger the sequence again

      return;
     }
     else if (buttonState == HIGH && ubuState == 6 )  // if button was pressed after Ubu's sequence ends, check to see if it's released to re-start it, or pressed after closing the box.
     {
      delay(2000); // wait a couple seconds for the button to be released, so it doesn't trigger the sequence again

        ubuState = 1; // Ubu is ready to RE-START

        Serial.println("Ubu is ready to RE-START (ubuState = 1).");  // Confirm that Ubu is ready to RE-START
        Serial.println(" "); // add blank line

       // blinkLights();
       // delay(100);
       // blinkLights();

     }

    Serial.print("Ubu's state is now: ");  // print the button state
    Serial.print(ubuState);  // print the button state
    Serial.println(" "); // add blank line
  }

  lastButtonState = buttonState;
  delay(20);

} // end buttonCheck


//********************  PLAY SOUND    **********************

void playSound() // PLAY SOUND: This will play a sound, if all goes well, using the i2c expander, for sound playback

{

  // mcp.writeGPIO(songValue);

  Serial.print("We are now playing the sound.");
  Serial.println(" ");

  ubuState = 3; // Let's do the next thing in the queue: make the gidouille spin forward. 

} // end playSound



//********************  SPINNING WHEEL    **********************

int i;    // generic variable to control the spinning wheel

void spinWheelForward()
{


  // buttonCheck(); // check to see if the button is pressed


  Serial.println("The spinning wheel moves forward"); // tell us when the spiral gidouille starts spinning forward


  myMotor->run(FORWARD);
  for (i = 0; i < 255; i++) 
   {
    myMotor->setSpeed(i);
    delay(20);
   }

  // We keep the motor spinning forward through the next sequence


  ubuState = 4;

  Serial.print("GThe wheel is now spinning forward. He can now perform his act (ubuState is 5).");
  Serial.println(" ");


} // end spinWheelForward


void spinWheelBackward()
{
  Serial.println("The spinning wheel moves backward"); // tell us when the spiral gidouille starts spinning backward


 myMotor->run(FORWARD); // First we have to spin down the forward the motion.
  for (i = 255; i != 0; i--) {
    myMotor->setSpeed(i);
    delay(20);
  }

  myMotor->run(BACKWARD); // Then start the backward motion.
  for (i = 0; i < 255; i++) {
    myMotor->setSpeed(i);
    delay(20);
  }

  for (i = 255; i != 0; i--) {
    myMotor->setSpeed(i);
    delay(20);
  }

  ubuState = 6;


} // end spinWheelBackward



//********************  UBU ACTIONS    **********************

void ubuActions() // Ubu does his act (flaps its wings, shakes its head and sings advice)
{

  if (ubuState == 4) // If Ubu has woken up, and his gidouille is spinning forward.
  {

    ubu_start = millis(); // Ubu start time
    ubu_time = 0; //  Ubu's time spent since start

    Serial.println(" "); // add a blank line
    Serial.print("Start Ubu actions. The time is now: "); // tell us when Ubu loop is invoked
    Serial.println(ubu_time); // tell how much time Ubu has used so far

    while (ubu_time <= 15000) // do this for 15 seconds.
    {

      buttonCheck(); // check to see if the button is pressed

      shakeHeadLeft();

      shakeHeadRight();

      shakeHeadLeft();

      shakeHeadRight();

      ubu_time = millis() - ubu_start; //  Ubu's time spent since start

    } //end while

    Serial.print("End Ubu actions. Time spent (milliseconds):"); // tell us when Ubu loop ends
    Serial.println(ubu_time); // tell how much time Ubu has used so far
    Serial.println(" "); // add a blank line
  }

  ubuState = 5;

  Serial.print("Ubu has performed his act. He can now wind down and go to sleep, after the gidouille stops spinning (ubuState is 5).");
  Serial.println(" ");

} // end ubuActions


//********************  HEAD    **********************

void shakeHeadLeft()
{

  Serial.println("Ubu shakes his head left"); // tell us when Ubu left head shake loop is invoked

  servo1.attach(10);  // attaches the servo on pin 10 to the servo object that makes Ubu's head shake

  for (pos = 0; pos < 60; pos += 2) // UBU SHAKES HIS HEAD LEFT - servo 4 goes from 0 degrees to 60 degrees in steps of 1 degrees
  { //
    servo1.write(pos);              // tell servo 4 (Ubu head) to go to position in variable 'pos'

    blinkEyes();
  }

  servo1.detach();  // detaches the servo to avoid straining it while Ubu sleeps

} // end shakeHeadLeft


void shakeHeadRight()
{
  Serial.println("Ubu shakes his head right"); // tell us when Ubu right head shake loop is invoked

  servo1.attach(10);  // attaches the servo on pin 10 to the servo object that makes Ubu's head shake

  for (pos = 60; pos >= 1; pos -= 2)  // UBU SHAKES HIS HEAD RIGHT - servo 4 goes from 60 degrees to 0 degrees in steps of 1 degrees
  { //
    servo1.write(pos);              // tell servo 4 (Ubu head) to go to position in variable 'pos'

    blinkEyes();
  }

  servo1.detach();  // detaches the servo to avoid straining it while Ubu sleeps

} // end shakeHeadRight



//********************  LIGHTS    **********************


void blinkLights()
{
  turnInsideLightsOn();
  delay (50);
  turnInsideLightsOff();
  delay (50);

} // end blinkLights


void turnFrontLightOn()
{
  digitalWrite(front_light, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println("Ubu front light is on");

} // end turnFrontLightOn


void turnFrontLightOff()
{
  digitalWrite(front_light, LOW);    // turn the LED off by making the voltage LOW
  Serial.println("Ubu front light is off");
} // end turnFrontLightOff

void turnInsideLightsOn()
{
  digitalWrite(inside_light, HIGH);   // turn the LED on (HIGH is the voltage level)

} // turnInsideLightsOn


void turnInsideLightsOff()
{
  digitalWrite(inside_light, LOW);    // turn the LED off by making the voltage LOW
} // end turnInsideLightsOff


void turnBoxLightsOn()
{
  digitalWrite(outside_light_left, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(outside_light_right, HIGH);   // turn the LED on (HIGH is the voltage level)

    turnFrontLightOn();

    turnInsideLightsOn();

  Serial.print("Lights are now turned on (ubuState is now 2).");
  Serial.println(" ");

 ubuState = 2; // next step is to play the sound, when the code is ready.

} // end turnBoxLightsOn


void turnBoxLightsOff()
{
  digitalWrite(outside_light_left, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(outside_light_right, LOW);    // turn the LED off by making the voltage LOW
} // end


void blinkEyes()
{

  buttonCheck(); // check to see if the button is pressed

  turnEyesOn();
  delay (50);
  turnEyesOff();

} // end blinkEyes


void turnEyesOn()
{
  digitalWrite(ubu_eyes, HIGH);   // turn the LED on (HIGH is the voltage level)
} // end turnEyesOn


void turnEyesOff()
{
  digitalWrite(ubu_eyes, LOW);    // turn the LED off by making the voltage LOW
} // end turnEyesOff


