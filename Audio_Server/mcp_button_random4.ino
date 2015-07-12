#include <Wire.h>
#include "Adafruit_MCP23008.h"

// Basic toggle test for i/o expansion. flips pin #0 of a MCP23008 i2c
// pin expander up and down. Public domain

// Connect pin #1 of the expander to Analog 5 (i2c clock)
// Connect pin #2 of the expander to Analog 4 (i2c data)
// Connect pins #3, 4 and 5 of the expander to ground (address selection)
// Connect pin #6 and 18 of the expander to 5V (power and reset disable)
// Connect pin #9 of the expander to ground (common ground)

// Output #0 is on pin 10 so connect an LED or whatever from that to ground
// writeGPIO(data);

Adafruit_MCP23008 mcp;

const int buttonPin = 12;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status
int oldbuttonState = 0;      // for button changes
int songValue = 0;
  
void setup() {  
  pinMode(buttonPin, INPUT_PULLUP);
  mcp.begin();      // use default address 0, based at 0x20
for (int i=0; i<8; i++) {
  mcp.pinMode(i, OUTPUT);  //all 8 pins output
  }
}


void loop() {
  songValue = random(4);
  oldbuttonState = buttonState;
  buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH, nc button:
  if (buttonState != oldbuttonState) {     
    if (buttonState == HIGH) {     
//    mcp.begin();      // use default address 0, based at 0x20
      mcp.writeGPIO(songValue);
    }
  }
  oldbuttonState = buttonState;
  delay(20);
}
