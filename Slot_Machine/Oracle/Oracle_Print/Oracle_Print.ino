/*

 Arduino code for Oracle Print, the interactive device that prints instructions from the Pataphysical Slot Machine's Oracle Controller.
 
 THIS VERSION IS FOR THE DIAVOLINO (evilmadscience.com)
 * Prints words sent by the Oracle controller (Sanguino), via the Oracle player (Arduino Mega 2560).
 * Oracle instructions are sent over serial port (Rx), as 3-word text strings.
 * Prints text one line at a time, using a Thermal Printer.
 * Yellow Tx wire from the Arduino Mega (19) goes to Rx pin 1 on the Diavolino 
 * Red power wire from the Arduino Mega (5V) goes to Vcc pin on the Diavolino 
 * Black ground wire from the Sanguino/Arduino Mega (GND) goes to Rx pin 1 on the Diavolino 
 * Green printer wire from the Thermal printer goes to pin 5 on the Diavolino 
 * Yellow printer wire from the Thermal printer goes to pin 6 on the Diavolino 
 
 
 Interactive specification:
 http://bit.ly/wonderbox-spec
 
 Wonderbox schematics:
 http://bit.ly/wonderbox-schema
 
 Photos and info about Pataphysical Studios:
 http://pataphysics.us
 
 
 Last updated on November 1, 2014.
 Written by Donald Day and Fabrice Florin 
 
 This code is licensed freely under CC-BY-SA-3.0.
 
 */

#include "SoftwareSerial.h"
#include "Adafruit_Thermal.h"

int printer_RX_Pin = 5;  // This is the green wire
int printer_TX_Pin = 6;  // This is the yellow wire

Adafruit_Thermal printer(printer_RX_Pin, printer_TX_Pin);

String inString = "";
String stringF = "";
String stringS = "";
String stringT = "";
String stringFword = "";
String stringSword = "";
String stringTword = "";
char charsF[14];

#define DEBOUNCE 100  // button debouncer


void setup() {
  // set up serial port
  Serial.begin(9600); // Serial for messages from wonder oracle

  pinMode(7, OUTPUT); 
  digitalWrite(7, LOW); // To also work w/IoTP printer

  // setup printer
  printer.begin();

  printer.println("hello");
} 

void loop() {
  //putstring(".");            // uncomment this to see if the loop isnt running
  while(Serial.available() > 0) 
  {
    char inChar = Serial.read();
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
      { 
        i++ ; 
      }  //find end of first word
      stringFword = inString.substring(0, i);   //put all of second word into stringFword
      if( i > 8)
      { 
        stringF = inString.substring(0, 8); 
      }  //put initial 8 of first word into stringF
      else
      { 
        stringF = inString.substring(0, i); 
      }  //put all of second word into stringF if less than 8
      stringF += ".WAV"; //append .WAV

      while(inString.charAt(i) == ' ')
      { 
        i++ ; 
      }  
      j = i;  //find beginning of second word
      while(inString.charAt(i) != ' ')
      { 
        i++ ; 
      }  //find end of second word
      stringSword = inString.substring(j, i);   //put all of second word into stringSword
      if( i > j + 8)
      { 
        stringS = inString.substring(j, j + 8); 
      }  //put initial 8 of second word into stringS
      else
      { 
        stringS = inString.substring(j, i); 
      }  //put all of second word into stringF if less than 8
      stringS += ".WAV"; //append .WAV

      while(inString.charAt(i) == ' ')
      { 
        i++ ; 
      }  
      j = i;  //find beginning of third word
      stringTword = inString.substring(j, len);   //put all of third word into stringTword
      if( len > j + 8)
      { 
        stringT = inString.substring(j, j + 8); 
      }  //put initial 8 of third word into stringF
      else
      { 
        stringT = inString.substring(j, len); 
      }  //put all of third word into stringF if less than 8
      stringT += ".WAV"; //append .WAV



      Serial.println(stringF + "_A_" + stringS + "_B_" + stringT + "_C_");

      printer.setLineHeight(50);  //make lines taller
      printer.justify('C');    // Center justified
      printer.println("  ");  // add  blank lines at the top
      printer.println("  ");
      printer.println("UBU SAYS:");  // tell them what this is all about, in small font
      printer.println("  ");
      printer.setSize('L');     // Large size type
      printer.doubleHeightOn();    // Double height type
      printer.boldOn();     // Bold type
      printer.println(stringFword + " " + stringSword + " " + stringTword);    // Print words 
      printer.setSize('M');     // Medium size type
      printer.doubleHeightOff();    // Double height off
      printer.boldOff();     // Bold off
      printer.println("  ");
      printer.println("www.pataphysics.us");
      printer.println("  "); // Add 4 blank lines
      printer.println("  ");
      printer.println("  ");
      printer.println("  ");


      inString = "";
      stringF = "";
      stringS = "";
      stringT = "";
    }

  }
}



