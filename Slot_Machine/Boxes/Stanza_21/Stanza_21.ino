/****************************************************

Arduino code for Stanza, an interactive art "wonderbox"
for the Pataphysical Slot Machine.

This software module controls the light and sounds, 
using an Arduino Uno with Wire and Adafruit_MCP23008 libraries.

About the Pataphysical Slot Machine:
http://pataphysics.us

Wonderbox specifications:
http://bit.ly/wonderbox-spec

Written by Ilyse Magy, with help from Justin Hall,  
based on free libraries from Arduino, Adafruit and others. 
Sound playback code by Donald Day and Tim Pozar.

This free software is licensed under GPLv2.
 ****************************************************/

#include <TrueRandom.h>   //Include the TrueRandom Library
#include <Servo.h> // for controlling servo motor
// #include <Wire.h> // to connnect with other boxes with the i2c protocol, for sound playback
// #include "Adafruit_MCP23008.h" // to connnect with the i2c expander, for sound playback
#include <I2C.h>

// Download the latest Adafruit_MCP23008 code here: https://github.com/adafruit/Adafruit-MCP23008-library

// Connect the red wire from the pataphysical bus to the 5V pin on Arduino
// Connect the black wire from the pataphysical bus to any ground pin on Arduino
// Connect the green wire from the pataphysical bus to Analog 5 (i2c clock) 
// Connect the blue wire from the pataphysical bus to Analog 4 (i2c data) 

// Adafruit_MCP23008 mcp; // instantiate Adafruit_MCP23008 mcp

const int box_button = 3; // the switch for the whole box is placed on pin 3 -- it is triggered when you open the box. 
int buttonState = 0;         // variable for reading the pushbutton status
int oldbuttonState = 0;      // for button changes

int songValue = 21;  // Play Track 21 (Box #21)
int quietValue = 0;  // TRK0 means stop playing

int ledPin1 = 13; //Set the number of LEDs you use.
int ledPin2 = 12;
int ledPin3 = 11;
int ledPin4 = 10;
int ledPin5 = 9;
int ledPin6 = 8;
int ledPin7 = 7;
int ledPin8 = 6;
int ledPin9 = 5;
int ledPin10 = 4;

//********************  SETUP    **********************
void setup()
{
pinMode(box_button, INPUT);

pinMode(ledPin1, OUTPUT); //Sets the LED pins as output pins.
pinMode(ledPin2, OUTPUT);
pinMode(ledPin3, OUTPUT);
pinMode(ledPin4, OUTPUT);
pinMode(ledPin5, OUTPUT);
pinMode(ledPin6, OUTPUT);
pinMode(ledPin7, OUTPUT);
pinMode(ledPin8, OUTPUT);
pinMode(ledPin9, OUTPUT);
pinMode(ledPin10, OUTPUT);

  delay(200*songValue); //waitby#    
  // mcp.begin();      // use default address 0, based at 0x20
  I2c.begin();
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
//    mcp.begin();      // use default address 0, based at 0x20
//    mcp.writeGPIO(songValue);
      I2c.write(32, 9, songValue);
    }
    else {
      // mcp.writeGPIO(quietValue);
      I2c.write(32, 9, quietValue);
    }
  }
  oldbuttonState = buttonState;
  delay(20);
   
//Sets a random value of LED voltage in the range of 0V - 5V with PWM
analogWrite(ledPin1, TrueRandom.random(100,255)); //Use the same pins used above
analogWrite(ledPin2, TrueRandom.random(100,255));
analogWrite(ledPin3, TrueRandom.random(100,255));
analogWrite(ledPin4, TrueRandom.random(100,255));
analogWrite(ledPin5, TrueRandom.random(100,255));
analogWrite(ledPin6, TrueRandom.random(100,255));
analogWrite(ledPin7, TrueRandom.random(100,255));
analogWrite(ledPin8, TrueRandom.random(100,255));
analogWrite(ledPin9, TrueRandom.random(100,255));
analogWrite(ledPin10, TrueRandom.random(100,255));
delay(TrueRandom.random(0,300)); //Limits the speed.
}
