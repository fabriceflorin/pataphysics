/*

Arduino code for Oracle Play, the interactive system that plays instructions from the Pataphysical Slot Machine's Oracle Controller.

THIS VERSION IS FOR THE ARDUINO MEGA 2560.
* Speaks words sent by the Oracle controller (Wiseclock Sanguino)
* Oracle instructions are sent over serial port (Rx), as 3-word text strings.
* Converts text into sound, using a Waveshield and .WAV audio files stored on an SD card.
* Yellow Tx wire from the Wiseclock Sanguino goes to Rx pin 19 on the Mega (and to Rx on the Diavolino print processor)
* For other pin specs, see Waveshield Library documentation on LadyAda.net (see link below) 
* The print functions have been moved to a separate Arduino controller: Oracle Print.

See Interactive specifications:
http://bit.ly/wonderbox-spec

Wonderbox schematics:
http://bit.ly/wonderbox-schema

Photos and info about Pataphysical Studios:
http://pataphysics.us


Last updated on November 1, 2014.
Written by Donald Day, Fabrice Florin and Anselm Hook. 
Sound code based on the Digital Audio Player sketch (dapHC.pde) using the Wave HC library published by Adafruit:
http://www.ladyada.net/make/waveshield/libraryhc.html 

 This code is licensed freely under CC-BY-SA-3.0.
 
*/

char foo;            // required to clean up some other problems
#include <Arduino.h> //needed for Serial.println

#include <ArduinoPins.h>
#include <FatReader.h>
#include <FatStructs.h>
#include <mcpDac.h>
#include <SdInfo.h>
#include <SdReader.h>

#include <WaveHC.h>

#include <Wavemainpage.h>
#include <WavePinDefs.h>

#include <WaveUtil.h>

#include <FatReader.h>
#include <SdReader.h>
#include <avr/pgmspace.h>
#include "WaveUtil.h"
#include "WaveHC.h"

#include "SoftwareSerial.h"


SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the filesystem on the card
FatReader f;      // This holds the information for the file we're play

WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

String inString = "";
String stringF = "";
String stringS = "";
String stringT = "";
String stringFword = "";
String stringSword = "";
String stringTword = "";
char charsF[14];


#define DEBOUNCE 100  // button debouncer

/*byte indexF;
byte indexS;
byte indexT;*/

// this handy function will return the number of bytes currently free in RAM, great for debugging!   
int freeRam(void)
{
  extern int  __bss_end; 
  
  extern int  *__brkval; 
  int free_memory; 
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  }
  return free_memory; 
} 

void sdErrorCheck(void)
{
  if (!card.errorCode()) return;
  putstring("\n\rSD I/O error: ");
  Serial1.print(card.errorCode(), HEX);
  putstring(", ");
  Serial1.println(card.errorData(), HEX);
  while(1);
}

void setup() {
   
 // set up serial port
  Serial.begin(9600);
  putstring_nl("WaveHC with 6 buttons");
  Serial1.begin(9600); // Serial1 for messages from wonder oracle - mega
  
   putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
  Serial.println(freeRam());      // if this is under 150 bytes it may spell trouble!
  
  // Set the output pins for the DAC control. This pins are defined in the library
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
 // disable SPI pins used by Uno instead of Megs
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(13, INPUT);
 
  // enable pull-up resistors on switch pins (analog inputs)
  digitalWrite(14, HIGH);
  digitalWrite(15, HIGH);
  digitalWrite(16, HIGH);
  digitalWrite(17, HIGH);
  digitalWrite(18, HIGH);
  digitalWrite(19, HIGH);
 
  //  if (!card.init(true)) { //play with 4 MHz spi if 8MHz isn't working for you
  if (!card.init()) {         //play with 8 MHz spi (default faster!)  
    putstring_nl("Card init. failed!");  // Something went wrong, lets print out why
    sdErrorCheck();
    while(1);                            // then 'halt' - do nothing!
  }
  
  // enable optimize read - some cards may timeout. Disable if you're having problems
  card.partialBlockRead(true);
 
// Now we will look for a FAT partition!
  uint8_t part;
  for (part = 0; part < 5; part++) {     // we have up to 5 slots to look in
    if (vol.init(card, part)) 
      break;                             // we found one, lets bail
  }
  if (part == 5) {                       // if we ended up not finding one  :(
    putstring_nl("No valid FAT partition!");
    sdErrorCheck();      // Something went wrong, lets print out why
    while(1);                            // then 'halt' - do nothing!
  }
  
  // Lets tell the user about what we found
  putstring("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(),DEC);     // FAT16 or FAT32?
  
  // Try to open the root directory
  if (!root.openRoot(vol)) {
    putstring_nl("Can't open root dir!"); // Something went wrong,
    while(1);                             // then 'halt' - do nothing!
  }
  
  // Whew! We got past the tough parts.
  putstring_nl("Ready!");
}

void loop() {
  //putstring(".");            // uncomment this to see if the loop isnt running
  while(Serial1.available() > 0) 
    {
    char inChar = Serial1.read();
    if(inChar != '\n')
      {
      inString += inChar;
      }
    else
      {
      inString.trim();
      int len = inString.length();
      int i = 0;  // i is current position in inString
      int j = 0;  // j is current word's first character position
      while(inString.charAt(i) != ' ')
        { i++ ; }  //find end of first word
      stringFword = inString.substring(0, i);   //put all of second word into stringFword
      if( i > 8)
        { stringF = inString.substring(0, 8); }  //put initial 8 of first word into stringF
      else
        { stringF = inString.substring(0, i); }  //put all of second word into stringF if less than 8
      stringF += ".WAV"; //append .WAV
      
      while(inString.charAt(i) == ' ')
        { i++ ; }  
      j = i;  //find beginning of second word
      while(inString.charAt(i) != ' ')
        { i++ ; }  //find end of second word
      stringSword = inString.substring(j, i);   //put all of second word into stringSword
      if( i > j + 8)
        { stringS = inString.substring(j, j + 8); }  //put initial 8 of second word into stringS
      else
        { stringS = inString.substring(j, i); }  //put all of second word into stringS if less than 8
      stringS += ".WAV"; //append .WAV
      
      while(inString.charAt(i) == ' ')
        { i++ ; }  
      j = i;  //find beginning of third word
      stringTword = inString.substring(j, len);   //put all of third word into stringTword
      if( len > j + 8)
        { stringT = inString.substring(j, j + 8); }  //put initial 8 of third word into stringF
      else
        { stringT = inString.substring(j, len); }  //put all of third word into stringT if less than 8
     stringT += ".WAV"; //append .WAV
      
      
             
     Serial.println(stringF + "_A_" + stringS + "_B_" + stringT + "_C_");
    
     
      playcomplete("GONG0.WAV"); 
 
      stringF.toCharArray(charsF,14);
      playcomplete(charsF);
         Serial.println(charsF);      // put first word in serial port
      stringS.toCharArray(charsF,14);
      playcomplete(charsF); 
          Serial.println(charsF);      // put second word in serial port
      stringT.toCharArray(charsF,14);
      playcomplete(charsF); 
           Serial.println(charsF);      // put third word in serial port    
      
      
      
      inString = "";
      stringF = "";
      stringS = "";
      stringT = "";
      }
      
  }
}

byte check_switches()
{
  static byte previous[6];
  static long time[6];
  byte reading;
  byte pressed;
  byte index;
  pressed = 0;

  for (byte index = 0; index < 6; ++index) {
    reading = digitalRead(14 + index);
    if (reading == LOW && previous[index] == HIGH && millis() - time[index] > DEBOUNCE)
    {
      // switch pressed
      time[index] = millis();
      pressed = index + 1;
      break;
    }
    previous[index] = reading;
  }
  // return switch number (1 - 6)
  return (pressed);
}


// Plays a full file from beginning to end with no pause.
void playcomplete(char *name) {
  // call our helper to find and play this name
  playfile(name);
  while (wave.isplaying) {
  // do nothing while its playing
  }
  // now its done playing
}

void playfile(char *name) {
  // see if the wave object is currently doing something
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  // look in the root directory and open the file
  if (!f.open(root, name)) {
    putstring("Couldn't open file "); Serial.println(name); return;
  }
  // OK read the file and turn it into a wave object
  if (!wave.create(f)) {
    putstring_nl("Not a valid WAV"); return;
  }
  
  // ok time to play! start playback
  wave.play();
}

