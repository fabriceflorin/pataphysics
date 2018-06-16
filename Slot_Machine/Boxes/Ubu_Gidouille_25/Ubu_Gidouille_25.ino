/****************************************************

Arduino code for the Ubu Gidouille box, an interactive art "wonderbox"
for the Pataphysical Slot Machine.

This software module controls the lights and sounds for that box, 
using an Arduino Uno with an Adafruit Motor Shield, as well as 
Wire and Adafruit_MCP23008 libraries.

About the Pataphysical Slot Machine:
http://pataphysics.us

Wonderbox specifications:
http://bit.ly/wonderbox-spec

Last updated on September 30, 2015.

Written by Fabrice Florin, based on free libraries from Arduino, Adafruit and others. 
Sound playback code by Donald Day and Tim Pozar.

This free software is licensed under GPLv2.
  
 ****************************************************/

#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <Servo.h>

/* Commenting out i2c server code for now, to avoid conflicts with Motor Shield
 #include <Wire.h>
 #include "Adafruit_MCP23008.h" // to connnect with the i2c expander, for sound playback
*/

// Connect the red wire from the pataphysical bus to the 5V pin on Arduino
// Connect the black wire from the pataphysical bus to any ground pin on Arduino
// Connect the green wire from the pataphysical bus to Analog 5 (i2c clock)
// Connect the blue wire from the pataphysical bus to Analog 4 (i2c data)

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Connect a DC motor to port M1
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);

 /* Commenting out i2c server code for now, to avoid conflicts with Motor Shield
 Adafruit_MCP23008 mcp; // instantiate Adafruit_MCP23008 mcp
*/

const int box_button = 4; // the switch for the whole box may be placed on pin 12 -- it is triggered when you open the box. 
int buttonState = 0;         // variable for reading the pushbutton status
int oldbuttonState = 0;      // for button changes

/* Commenting out i2c server code for now, to avoid conflicts with Motor Shield
int songValue = 25;  // Play Track 25 (Box #25)
int quietValue = 0;  // TRK0 means stop playing
*/

const int led_1 = 7; // LED group 1 is on pin 3
const int led_2 = 6; // LED group 2 is on pin 4


//********************  SETUP    **********************

void setup() 
{ 

  // initialize the digital pin as an output.
  pinMode(box_button, INPUT);   // initialize the button's digital pin as an input.
  pinMode(led_1, OUTPUT);     
  pinMode(led_2, OUTPUT);   
   
  
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("MMMMotor party!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz

  // turn on motor M1
  myMotor->setSpeed(200);
  myMotor->run(RELEASE);

/* Commenting out i2c server code for now, to avoid conflicts with Motor Shield
delay(5000); //wait five seconds after powerup    
  mcp.begin();      // use default address 0, based at 0x20
*/
} 


//********************  MAIN LOOP    **********************

 
void loop() 
{ 
 
  // First ask the server to play a sound if the button has been pressed.
  
  oldbuttonState = buttonState;
  buttonState = digitalRead(box_button);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH, nc button:
  if (buttonState != oldbuttonState) 
  {     
    if (buttonState == HIGH) 
    {     
      
 /* Commenting out i2c server code for now, to avoid conflicts with Motor Shield
 // Play a sound, since the button has been pressed.    
  mcp.writeGPIO(songValue);
*/
     
   // Turn on the two LEDs 
  
  digitalWrite(led_1, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(led_2, HIGH);    // turn the LED off by making the voltage HIGH
 
    // Spin the spiral wheel 
     
  spinWheelForward();

  
   }
    else 
    {
      
 /* Commenting out i2c server code for now, to avoid conflicts with Motor Shield
 mcp.writeGPIO(quietValue);
*/
  
  spinWheelDown();

   // Turn off the LEDs  
  
  digitalWrite(led_1, LOW);   // tturn the LED off by making the voltage LOW
  digitalWrite(led_2, LOW);    // turn the LED off by making the voltage LOW

   }
  }
 
  oldbuttonState = buttonState;
  delay(20);
 

}


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


  Serial.print("The wheel is now spinning forward. He can now perform his act (ubuState is 5).");
  Serial.println(" ");


} // end spinWheelForward


void spinWheelDown()
{
  Serial.println("The spinning wheel stops"); // tell us when the spiral gidouille starts spinning backward


 myMotor->run(FORWARD); // Spin down the forward the motion.
  for (i = 255; i != 0; i--) {
    myMotor->setSpeed(i);
    delay(20);
  }


} // end spinWheelDown

