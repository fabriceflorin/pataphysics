/****************************************************
Arduino code for the 'Mother of Yes' wonderbox, an interactive artwork
for the Pataphysical Slot Machine.

This software module controls the lights and sounds for that box, 
using an Arduino Uno with  Servo, Wire and Adafruit_MCP23008 libraries.

About the Pataphysical Slot Machine:
http://pataphysics.us

Wonderbox specifications:
http://bit.ly/wonderbox-spec

Last updated on February 13, 2016.

Written by Fabrice Florin, based on free libraries from Arduino, Adafruit and others. 
Sound playback code by Donald Day and Tim Pozar.

This free software is licensed under GPLv2.
****************************************************/

#include <I2C.h>

/* Wiring instructions:
For the Arduino or Diavolino clients in each Wonderbox:
- Connect the red wire from the pataphysical bus to the 5V pin on Arduino
- Connect the black wire from the pataphysical bus to any ground pin on Arduino
- Connect the green wire from the pataphysical bus to Analog 5 (i2c clock) (goes to pin #1 of the expander)
- Connect the blue wire from the pataphysical bus to Analog 4 (i2c data) (goes to pin #2 of the expander)
*/


const int buttonPin = 12;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status
int oldbuttonState = 0;      // for button changes
int songValue = 16;  // Play Track 16 (Box #16)
int quietValue = 0;  // TRK0 means stop playing

 // Pataphysical Tracks List:
 // http://bit.ly/pata-tracks-list
  
void setup() {  
  Serial.begin(9600);
  Serial.println("Start of setup.");
  pinMode(buttonPin, INPUT);
  delay(200*songValue); // waitby#
  I2c.begin();
  Serial.println("End of setup.");
}

void loop() {
  oldbuttonState = buttonState;
  buttonState = digitalRead(buttonPin);
  Serial.println(buttonState);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH, nc button:
  if (buttonState != oldbuttonState) {     
    if (buttonState == HIGH) {     
       I2c.write(32, 9, songValue);
      Serial.println("Door is open.");
    }
    else {
      I2c.write(32, 9, quietValue);
      Serial.println("Door is closed.");
    }
  }
  delay(20);
  oldbuttonState = buttonState;
}
