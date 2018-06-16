/****************************************************
Arduino code for the Yellow Submarine box, an interactive art "wonderbox"
for the Pataphysical Slot Machine.

This software module controls the lights and sounds for that box, 
using an Arduino Uno with  Servo, Wire and Adafruit_MCP23008 libraries.

About the Pataphysical Slot Machine:
http://pataphysics.us

Wonderbox specifications:
http://bit.ly/wonderbox-spec

Last updated on October 4, 2015.

Written by Fabrice Florin and Janey Fritsche, based on free libraries from Arduino, Adafruit and others. 
Sound playback code by Donald Day and Tim Pozar.

This free software is licensed under GPLv2. 
 ****************************************************/
 
// #include <Wire.h> // to connnect with other boxes with the i2c protocol, for sound playback
// #include "Adafruit_MCP23008.h" // to connnect with the i2c expander, for sound playback
// Download the latest Adafruit_MCP23008 code here: https://github.com/adafruit/Adafruit-MCP23008-library
#include <I2C.h>

// Connect the red wire from the pataphysical bus to the 5V pin on Arduino
// Connect the black wire from the pataphysical bus to any ground pin on Arduino
// Connect the green wire from the pataphysical bus to Analog 5 (i2c clock) 
// Connect the blue wire from the pataphysical bus to Analog 4 (i2c data) 

// Adafruit_MCP23008 mcp; // instantiate Adafruit_MCP23008 mcp

const int box_button = 12; // the switch for the whole box may be placed on pin 12 -- it is triggered when you open the box. 
int buttonState = 0;         // variable for reading the pushbutton status
int oldbuttonState = 0;      // for button changes

int songValue = 22;  // Play Track 22 (Box #22)
int quietValue = 0;  // TRK0 means stop playing

const int led = 10; // LED group is on pin 10

//********************  SETUP    **********************
void setup() 
{ 
  pinMode(box_button, INPUT);   // initialize the button's digital pin as an input.
  pinMode(led, OUTPUT);     
 
  digitalWrite(led, HIGH);    // turn the LED off by making the voltage LOW
   
  delay(200*songValue); // waitby# 
//  mcp.begin();      // use default address 0, based at 0x20
  I2c.begin();
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
       // Turn on the LEDs
       digitalWrite(led, HIGH);    // turn the LED off by making the voltage LOW
       // delay(1000);           
       // Then play a sound, since the button has been pressed.       
       // mcp.writeGPIO(songValue);
       I2c.write(32, 9, songValue);
    } else { 
       // mcp.writeGPIO(quietValue);
       I2c.write(32, 9, quietValue);
       digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
       // delay(1000);   
    }
  }
  oldbuttonState = buttonState;
  delay(20); 
}

