/*************************************************** 
Arduino code for playing sounds from a Wonderbox client, using a separate sound server with MCP23008 i2c expander and MusicMaker,
in the Wonderbox system that powers the Pataphysical Slot Machine.

Wonderbox specifications:
http://bit.ly/wonderbox-spec

Wonderbox schematics:
http://bit.ly/wonderbox-schema

Wonderbox code repository
https://github.com/fabriceflorin/pataphysics

About the Pataphysical Slot Machine:
http://pataphysics.us

Last updated on December 23, 2015.

Written by Donald Day and Tim Pozar, based on free libraries from Arduino and Adafruit. 

This free software is licensed under GPLv2.
  
****************************************************/
 
#include <I2C.h>

/* Wiring instructions:
For the Arduino or Diavolino clients in each Wonderbox:
- Connect the red wire from the pataphysical bus to the 5V pin on Arduino
- Connect the black wire from the pataphysical bus to any ground pin on Arduino
- Connect the green wire from the pataphysical bus to Analog 5 (i2c clock) (goes to pin #1 of the expander)
- Connect the blue wire from the pataphysical bus to Analog 4 (i2c data) (goes to pin #2 of the expander)

For the Arduino sound server with the MCP23008 i2c expander: (ignore these if you are only working on the clients)
- Connect pin #1 of the expander to Analog 5 (i2c clock - SCL) (green wire on the pataphysical bus)
- Connect pin #2 of the expander to Analog 4 (i2c data - SDA) (blue wire on the pataphysical bus, yellow wire on sound server)
- Connect pins #3, 4 and 5 of the expander to ground (address selection)
- Connect pin #6 and 18 of the expander to 5V (power and reset disable)
- Connect pin #9 of the expander to ground (common ground)
- Output #0 is on pin 10 so connect an LED or whatever from that to ground
*/

// Scheduler version using millis() for flashing 12-21-2015 DD

const int buttonPin = 12;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status
int oldbuttonState = 0;      // for button changes
int songValue = 24;  // Play Track 24 (Box #24)
int quietValue = 0;  // TRK0 means stop playing


//LIGHTS
//Cycle through red, green, blue, yellow, purple, by varying
//intensity of red, blue, and green. For two LEDs, blinking through colors faster and faster then slower and slower

/*
Adapted from Adafruit Arduino - Lesson 3. RGB LED
Diffuse RGB LED wired with common negative pin connected
to ground, other 3 pins connected via 270 Ohm resistors to pins
11, 10, 9 -- for two LEDS in synch, r and r2 are pin3 and pin6;
g and g2 are pin4 and pin9, b and b2 are pin5 and pin10 */

// pins below didn't match PWM pins above, so changing them 12/23 dd
int redPin = 3;
int greenPin = 5;
int bluePin = 6;
int red2Pin = 9;
int green2Pin = 10;
int blue2Pin = 11;
int delayPeriodSlow = 250 ; // just example values lower value is faster
// original value set to 250
int delayPeriodFast = 25;
int delayPeriodIncrement = 10;
int i = delayPeriodSlow;
long flashTime;
long oldflashTime;
boolean faster = true;

void setup()
{

pinMode(buttonPin, INPUT);

pinMode(redPin, OUTPUT);
pinMode(greenPin, OUTPUT);
pinMode(bluePin, OUTPUT); 
pinMode(red2Pin, OUTPUT);
pinMode(green2Pin, OUTPUT);
pinMode(blue2Pin, OUTPUT); 

delay(200*songValue); // waitby#
I2c.begin();
}

void loop()
{
//  oldbuttonState = buttonState; don't need to do this twice 12/23 dd
  buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH, nc button:
  if (buttonState != oldbuttonState) {     
    if (buttonState == HIGH) {     
      I2c.write(32, 9, songValue);
    }
    else {
      I2c.write(32, 9, quietValue);
    }
  }
  oldbuttonState = buttonState;
  delay(2);  // probably enough delay in flash colors - change to 2  - 12/23 dd

  flashColors(i);
}

void flashColors(int delayPeriod)
{
  flashTime = millis() - oldflashTime;
  if (flashTime < delayPeriod) 
    {
    setColor(255, 0, 0); // red
    }
  else if (flashTime < 2*delayPeriod) 
    {
    setColor(0, 255, 0); // green
    }
  else if (flashTime < 3*delayPeriod) 
    {
    setColor(0, 0, 255); // blue
    }
  else if (flashTime < 4*delayPeriod) 
    {
    setColor(255, 255, 0); // yellow
    }
  else if (flashTime < 5*delayPeriod) 
    {
    setColor(80, 0, 80); // purple
    }
  else if (faster) 
    {
    i -= delayPeriodIncrement;
    oldflashTime = millis();
    if (i < delayPeriodFast) 
      {
      faster = false;
      }
    }
  else 
    {
    i += delayPeriodIncrement;
    oldflashTime = millis();
    if (i > delayPeriodSlow) 
      {
      faster = true;
      }
    }  
}

void setColor(int red, int green, int blue)
{
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue); 
  analogWrite(red2Pin, red);
  analogWrite(green2Pin, green);
  analogWrite(blue2Pin, blue); 
}
