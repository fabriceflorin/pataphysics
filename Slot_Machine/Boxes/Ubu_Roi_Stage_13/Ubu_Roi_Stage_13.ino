/*************************************************** 
Arduino code for playing sounds from the Eros wonderbox, 
using a separate sound server with MCP23008 i2c expander and MusicMaker,
in the Wonderbox system that powers the Pataphysical Slot Machine.
Wonderbox specifications:
http://bit.ly/wonderbox-spec
Wonderbox schematics:
http://bit.ly/wonderbox-schema
Wonderbox code repository
https://github.com/fabriceflorin/pataphysics
About the Pataphysical Slot Machine:
http://pataphysics.us
This code requires the Arduino Library for the MCP23008 (and '9) I2C I/O expander at:
https://github.com/adafruit/Adafruit-MCP23008-library
(this code needs to be placed in 'Arduino/libraries/Adafruit_MCP23008' folder on your computer)
Last updated on September 28, 2015.
Written by Donald Day and Tim Pozar, based on free libraries from Arduino and Adafruit. 
This sketch used to be called mcp_button_random4.ino, before being renamed Sound_Client.ino for clarity.
This free software is licensed under GPLv2.
  
****************************************************/
 
// #include <Wire.h>
// #include "Adafruit_MCP23008.h"
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
The code below is a basic toggle test for i/o expansion. It flips pin #0 of a MCP23008 i2c pin expander up and down. 
*/

// writeGPIO(data);
// Adafruit_MCP23008 mcp;

const int buttonPin = 12;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status
int oldbuttonState = 0;      // for button changes
int songValue = 13;  // Play Track 13 (Box #13)
int quietValue = 0;  // TRK0 means stop playing

 // Pataphysical Tracks List:
 // http://bit.ly/pata-tracks-list
  
void setup() {  
  pinMode(buttonPin, INPUT);
  delay(5000); //wait five seconds after powerup
//  mcp.begin();      // use default address 0, based at 0x20
  I2c.begin();
}

void loop() {
  oldbuttonState = buttonState;
  buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH, nc button:
  if (buttonState != oldbuttonState) {     
    if (buttonState == HIGH) {     
//    mcp.begin();      // use default address 0, based at 0x20
//      mcp.writeGPIO(songValue);
      I2c.write(32, 9, songValue);
    }
    else {
      // mcp.writeGPIO(quietValue);
      I2c.write(32, 9, quietValue);
    }
  }
  oldbuttonState = buttonState;
  delay(20); 
}
