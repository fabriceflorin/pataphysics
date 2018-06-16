/****************************************************

Arduino code for X-Ray Skull, an interactive art "wonderbox"
for the Pataphysical Slot Machine.

This software module controls the skull's sound and motion, 
using an Arduino Uno with  Servo, Wire and Adafruit_MCP23008 libraries.

A separate software module controls the skull's eye lights, 
using an Arduino Uno.

About the Pataphysical Slot Machine:
http://pataphysics.us

Wonderbox specifications:
http://bit.ly/wonderbox-spec

Last updated on February 6 2016.

Written by Jean Bolte and Fabrice Florin, 
based on free libraries from Arduino and Adafruit. 
Sound playback code by Donald Day and Tim Pozar.

This free software is licensed under GPLv2.
  
 ****************************************************/
 
#include <Servo.h> // for controlling servo motor
// #include <Wire.h> // to connnect with other boxes with the i2c protocol, for sound playback
// #include "Adafruit_MCP23008.h" // to connnect with the i2c expander, for sound playback
#include <I2C.h>  // improved I2C library    DD1-9-2016
// Download the latest Adafruit_MCP23008 code here: https://github.com/adafruit/Adafruit-MCP23008-library

// Connect the red wire from the pataphysical bus to the 5V pin on Arduino
// Connect the black wire from the pataphysical bus to any ground pin on Arduino
// Connect the green wire from the pataphysical bus to Analog 5 (i2c clock) 
// Connect the blue (or yellow) wire from the pataphysical bus to Analog 4 (i2c data) 

// Adafruit_MCP23008 mcp; // instantiate Adafruit_MCP23008 mcp

const int box_button = 12 ; // the switch for the whole box is on pin 12 -- it is triggered when you open the box. 

Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
long pos = 30;    // variable to store the servo position 


int buttonState = 0;         // variable for reading the pushbutton status
int oldbuttonState = 0;      // for button changes
// int SoundPlaybackState = 0; // set to "0" by default, so the sound will play the first time.
int songValue = 28;  // Play Track 28 (Box #28), the X-Ray Skull
int quietValue = 0;  // TRK0 means stop playing
long skullTime = 0;
long oldskullTime = 0;


//********************  SETUP    **********************

void setup() 
{ 
  
delay(200*songValue); // waitby#
I2c.begin();
Serial.begin(9600);
 
// position the skull's head in the center
myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
myservo.write(30);  // tell servo to go to the center, while we wait for sound server delay. 
myservo.detach();  // detaches the servo to avoid straining it while we wait   
  
}

//********************  MAIN LOOP    **********************

 
void loop() 
{ 
  oldbuttonState = buttonState;
  buttonState = digitalRead(box_button);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH, nc button:
  if (buttonState != oldbuttonState) {      
    if (buttonState == HIGH) {     
      oldskullTime = millis();
      // start playing the sound
      I2c.write(32, 9, songValue);
      // start moving the skull
      myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
      myservo.write(0);  // tell servo to go to the beginning position 
      }
     else {
      // stop playing the sound
      I2c.write(32, 9, quietValue);
      // stop moving the skull
      // position the skull's head in the center
      myservo.write(30);  // tell servo to go to the center 
      myservo.detach();  // detaches the servo to avoid straining it while we wait     
      }
      delay(20);
    }  
    else if (buttonState == HIGH) {    //stuff to do if the box is open 
      // is it time to make a step increasing angle?   
      skullTime = millis() - oldskullTime;
      if (skullTime < 3000) { // 60 50ms cycles
        pos = skullTime/50;
        myservo.write(pos);              // tell servo to go to position in variable 'pos' - 0 up to 60
      }
      // is it time to make a step decreasing angle?   
      else if (skullTime < 6000) { // 60 50ms cycles
        pos = 60 - (skullTime - 3000)/50;
        myservo.write(pos);              // tell servo to go to position in variable 'pos' - 60 down to 0 
      }
      else oldskullTime = millis();  // ready for another skull nod back and forth

    }
    // nothing to do when the box is closed 
}



