// wait10
/*************************************************** 
Arduino code for playing sounds from a sound server with i2c expander and MusicMaker
in the Wonderbox system that powers the Pataphysical Slot Machine.
Wonderbox specifications:
http://bit.ly/wonderbox-spec
Wonderbox schematics:
http://bit.ly/wonderbox-schema
Wonderbox code repository
https://github.com/fabriceflorin/pataphysics
About the Pataphysical Slot Machine:
http://pataphysics.us
This is an example for the Adafruit VS1053 Codec Breakout
  
Designed specifically to work with the Adafruit VS1053 Codec Breakout 
----> https://www.adafruit.com/products/1381
  
Last updated on November 28, 2015.
Written by Donald Day, Fabrice Florin and Tim Pozar, 
based on free libraries from Arduino 
and Limor Fried/Ladyada for Adafruit Industries. 
  
This free software is licensed under GPLv2. The Adafruit code is licensed under BSD, all text above must be included in any redistribution.
  
****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_MCP23008.h>

Adafruit_MCP23008 mcp;

String trackname = ""; 

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  //Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

int I2Cvalue = 0;
int oldI2Cvalue = 0;
int tracknum = 1;

int boxValue = 32;

void setup() {
  Serial.begin(9600);
  
  Serial.println("Initialize I2C hardware receiver");
  // Initialize the I2C chip...
    delay(10000); 
    mcp.begin();      // use default address 0, based at 0x20 // This setup routine will initiate the sound playback via i2c expander

  for (int i=0; i<8; i++) // commenting out the server initialization
   {
     mcp.pinMode(i, OUTPUT);  //all 8 pins output
   }       

  Serial.println("Adafruit VS1053 Library Test");
  // initialise the music player
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
 
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  
  // list files
  // printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(1,1);

  /***** Two interrupt options! *******/ 
  // This option uses timer0, this means timer1 & t2 are not required
  // (so you can use 'em for Servos, etc) BUT millis() can lose time
  // since we're hitchhiking on top of the millis() tracker
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT);
  
  // This option uses a pin interrupt. No timers required! But DREQ
  // must be on an interrupt pin. For Uno/Duemilanove/Diecimilla
  // that's Digital #2 or #3
  // See http://arduino.cc/en/Reference/attachInterrupt for other pins
  // *** This method is preferred
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
    
// pin config for I2C hardware receiver    
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A1, INPUT); //changed for I2C compatibility
  pinMode(A0, INPUT); //changed for I2C compatibility
  pinMode(2, INPUT);
  pinMode(5, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
 }

void loop() {
      oldI2Cvalue = I2Cvalue;  
  // read the I2C expander pins:
      I2Cvalue = digitalRead(A1);         //changed for I2C compatibility
      I2Cvalue += (digitalRead(A0) << 1); //changed for I2C compatibility
      I2Cvalue += (digitalRead(A3) << 2);
      I2Cvalue += (digitalRead(A2) << 3);
      I2Cvalue += (digitalRead(2) << 4);
      I2Cvalue += (digitalRead(5) << 5);
      I2Cvalue += (digitalRead(8) << 6);
      I2Cvalue += (digitalRead(9) << 7);
      
      if (I2Cvalue != oldI2Cvalue) {
        musicPlayer.stopPlaying();    
     // Let's build a new file name to play...        
        trackname = "TRK";
     //   trackname += "000";  new filename format with no leading zeroes
        trackname += String(I2Cvalue);
        trackname += ".MP3";
        // The startPlayingFile function wants a char array...
        char chartrackname[trackname.length()+1]; //make this the size of the String
        trackname.toCharArray(chartrackname, trackname.length()+1);
     // Start playing a file, then we can do stuff while waiting for it to finish
        if (! musicPlayer.startPlayingFile(chartrackname)) {
          Serial.println("Could not open file \"" + trackname + "\".");
        } else {
          Serial.println("Started playing file \"" + trackname + "\".");        
        }
      }
}

/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
