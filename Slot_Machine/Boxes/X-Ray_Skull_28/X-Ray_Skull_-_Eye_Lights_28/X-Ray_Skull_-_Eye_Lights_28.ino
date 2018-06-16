/****************************************************
Arduino code for X-Ray Skull, an interactive art "wonderbox"
for the Pataphysical Slot Machine.
This software module controls the skull's blinking eyes, 
now using a Teensy to cycle through red, green, blue, yellow, purple, 
by varying the intensity of red, blue, and green LEDs.
A separate software module controls the skull's sound and motion, 
using an Arduino Uno.
About the Pataphysical Slot Machine:
http://pataphysics.us
Wonderbox specifications:
http://bit.ly/wonderbox-spec
Last updated on September 21, 2015.
Written by Jean Bolte, with help from Donald Day and Howard Rheingold, 
based on free libraries from Arduino, Adafruit and others. 
Adapted from Adafruit Arduino-Lesson 3. RGB LED
Diffuse RGB LED wired with common negative pin connected
to ground, other 3 pins connected via 270 Ohm resistors to pins
11, 10, 9--for two LEDS in synch, r and r2 are pin3 and pin6;
g and g2 are pin4 and pin9, b and b2 are pin5 and pin10
This free software is licensed under GPLv2.
  
 ****************************************************/
 

int redPin = 3;       
int greenPin = 4;
int bluePin = 5;
int red2Pin = 6;
int green2Pin = 9;
int blue2Pin = 10;
int delayPeriodSlow = 250 ; //just example values lower value is faster
// original value set to 250
int delayPeriodFast = 25;
int delayPeriodIncrement = 10;


//********************  SETUP    **********************

void setup() 
{ 

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT); 
  pinMode(bluePin, OUTPUT);
  pinMode(red2Pin, OUTPUT);
  pinMode(green2Pin, OUTPUT);
  pinMode(blue2Pin, OUTPUT);

    
} 


//********************  MAIN LOOP    **********************

 
void loop() 
{ 
 
 int i;
for(i = delayPeriodSlow; i>= delayPeriodFast; i = i - delayPeriodIncrement) //speed up
{
  flashColors(i);
}
for(i = delayPeriodFast; i>= delayPeriodSlow; i=i + delayPeriodIncrement) //slow down
{
flashColors(i);
}
}
void flashColors(int delayPeriod)

{
setColor(255, 0, 0); // red
delay(delayPeriod);
setColor(0, 255, 0); // green
delay(delayPeriod);
setColor(0, 0, 255); // blue
delay(delayPeriod);
setColor(255, 255, 0); //yellow
delay(delayPeriod);
setColor(80, 0, 80); //purple
delay(delayPeriod);
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

