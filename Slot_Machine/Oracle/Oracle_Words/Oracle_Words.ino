/***********************************************************************
 * Wonderbox3216.pde - Demo program for scrolling large font on 3216 display
 * - based on code by Bill Westfield ("WestfW");
 * - Bill Ho added many scrolling functions;
 * - FlorinC added functionality for scrolling with large fonts;
 *
 *   Copyrighted and distributed under the terms of the Berkeley license
 *   (copy freely, but include this notice of original author.)
 *
 ***********************************************************************/

#if ARDUINO < 100
  #include <WProgram.h>
#else
  #include <Arduino.h>
#endif

#include "HT1632.h"
#include <avr/pgmspace.h>
#include "fontLarge.h"


#define Number_of_Displays 2
#define CHIP_MAX 4*Number_of_Displays //Four HT1632Cs on one board
#define X_MAX 32*Number_of_Displays -1
#define Y_MAX 15

#define CLK_DELAY


//Atmega644 Version of fastWrite - for pins 0-15
#define digitalWriteFast(_pin_, _state_) ( _pin_ < 8 ? (_state_ ? PORTB |= 1 << _pin_ : \
PORTB &= ~(1 << _pin_ )) : (_state_ ? PORTD |= 1 << (_pin_ -8) : PORTD &= ~(1 << (_pin_ -8) )))


// possible values for a pixel;
//RANDOMCOLOR for each line a different color
//MULTICOLOR for each point a different random color
#define BLACK  0
#define GREEN  1
#define RED    2
#define ORANGE 3
#define RANDOMCOLOR 4 // only for scrolling functions
#define MULTICOLOR 5

// insert here the number of columns of your font files 
// the compiler will comment how large the number of columns
// should be
#define NCOLUMNS 8

#define LONGDELAY 1000  // This delay BETWEEN demos


#define plot(x,y,v)  ht1632_plot(x,y,v)
#define cls          ht1632_clear

const int buttonPin = 1;     // the number of the pushbutton pin

char FirstWord[130][20] = {"ABSORB", "ADD", "APPLY", "ATTRACT", "BLESS", "BOUNCE", "BREAK", "BUILD", "BURN", "CAPTURE", "CHANGE", "CHEER", "CLAIM", "CLIMB", "CLOSE", "CONNECT", "CONTACT", "CONTEMPLATE", "CREATE", "CULTIVATE", "DESIGN", "DESIRE", "DISCOVER", "DISGUISE", "DISMISS", "DRAW", "DREAM", "EARN", "EAT", "ENCOUNTER", "ENJOY", "ENLIGHTEN", "EXAMINE", "EXPLORE", "FERTILIZE", "FIND", "FIX", "FLY", "FOCUS", "FOLLOW", "FORGET", "FORGIVE", "GIVE", "GREET", "GROW", "HAVE", "HEAL", "HEAR", "HELP", "HIDE", "HOLD", "IMAGINE", "INVENT", "INVESTIGATE", "INVITE", "JOIN", "KEEP", "KISS", "LEAD", "LEARN", "LEAVE", "LICK", "LIGHT", "LIKE", "LOCK", "MAGNIFY", "MAKE", "MARRY", "MEND", "MOLD", "MOVE", "MULTIPLY", "MYSTIFY", "OBSERVE", "OCCUPY", "OPEN", "PAINT", "PERFORM", "PLAY", "PONDER", "PREPARE", "PUT", "QUIT", "RAISE", "REACH", "RECORD", "RESTORE", "REVEAL", "SCREW", "SEARCH", "SEE", "SEEK", "SEIZE", "SELL", "SEND", "SHARE", "SHOW", "SING", "SLIDE", "SOLVE", "SPLIT", "STROKE", "STUDY", "SUPPORT", "SURPRISE", "SURRENDER", "SWIPE", "TAKE", "TASTE", "TEACH", "TELL", "TEST", "THANK", "THINK", "TOUCH", "TRY", "TURN", "TWEET", "UNDO", "UNLOCK", "VIEW", "VISIT", "WATCH", "WEAR", "WELCOME", "WIN", "WISH", "WONDER", "WORSHIP", "WRITE"};

char SecondWord[125][20] = {"ABSOLUTE", "ABSURD", "AMAZING", "AMBIENT", "ANGRY", "ARTISTIC", "BAD", "BAFFLING", "BEAUTIFUL", "BENIGN", "BIG", "BLACK", "BLUE", "BORING", "BRILLIANT", "BUMPY", "CHOSEN", "CLEAN", "COLD", "COMPLEX", "COOL", "COSMIC", "CRAZY", "CREATIVE", "DAZZLING", "DEEP", "DIFFICULT", "DREAMY", "EARTHY", "EASY", "ELECTRIC", "EPIC", "ESOTERIC", "ETERNAL", "EXTRAORDINARY", "EXTREME", "FABULOUS", "FERTILE", "FIERY", "FIRST", "FORBIDDEN", "FRAGRANT", "FREE", "FRIENDLY", "FUZZY", "GIGANTIC", "GLOWING", "GOOD", "GREAT", "GREEN", "HAPPY", "HEALING", "HEAVY", "HIDDEN", "HIGH", "HOLY", "HOT", "HUMAN", "HUMBLE", "IMAGINARY", "INTENSE", "KIND", "LIGHT", "LIQUID", "LITTLE", "LONG", "LOUD", "LOVELY", "MAD", "MATHEMATICAL", "MEDICINAL", "MIRACULOUS", "MYSTERIOUS", "MYSTICAL", "NATURAL", "NEW", "ODD", "OLD", "OPEN", "ORANGE", "PASSIONATE", "PATAPHYSICAL", "PATIENT", "POOR", "PRETTY", "PRIVATE", "PUBLIC", "PURPLE", "QUICK", "QUIET", "RADIANT", "RED", "RICH", "RIGHT", "ROUND", "SACRED", "SAD", "SECOND", "SECRET", "SENTIENT", "SHARP", "SHORT", "SIMPLE", "SLICK", "SMART", "SMOOTH", "SOFT", "SOLID", "STRANGE", "STRONG", "SURPRISING", "SURREAL", "SWEET", "TASTY", "THIN", "TINY", "UNUSUAL", "WARM", "WET", "WHITE", "WISE", "YOUNG", "YUMMY", "FALSE", "TRUE"};

char ThirdWord[200][20] = {"ADVENTURE", "AIR", "ALIEN", "ANGEL", "ANOMALY", "ANT", "APPLE", "ART", "ATOM", "BABY", "BALL", "BAMBOODU", "BEAST", "BEAUTY", "BEING", "BELLY", "BICYCLE", "BIRD", "BOOK", "BOY", "BRAIN", "BREATH", "BURNER", "CAKE", "CAT", "CIRCLE", "CLOUD", "CODE", "COMMUNITY", "COMPASSION", "CONTROL", "COSMOS", "COW", "COYOTE", "CRAFT", "CREATURE", "CROW", "CULT", "CUP", "DANCE", "DATA", "DEBRAINING", "DESIRE", "DEVICE", "DEVIL", "DIMENSION", "DOCTOR", "DOG", "DOOR", "DREAM", "EAR", "EARTH", "EGG", "ELF", "ELIXIR", "EXPERIENCE", "EYE", "FACE", "FAIRY", "FAME", "FART", "FEAR", "FINGER", "FIRE", "FISH", "FLOWER", "FOOL", "FOOT", "FRIEND", "FROG", "FUTURE", "GALAXY", "GAS", "GIRL", "GOD", "GOLD", "GROUP", "HA HA", "HAND", "HAPPINESS", "HEART", "HEAVEN", "HELL", "HOLE", "HOME", "HOPE", "HORSE", "HUMAN", "IDEA", "IDOL", "IMAGE", "INTELLIGENCE", "ISLAND", "JOKE", "JOY", "KEY", "KING", "KNOWLEDGE", "LAUGH", "LAUGHTER", "LIFEFORM", "LIGHT", "LIP", "LIQUID", "LORE", "LOVE", "LOVER", "LUCK", "MACHINE", "MAN", "MEDICINE", "MEMORY", "MESSAGE", "METAL", "MIND", "MIRACLE", "MOB", "MONEY", "MONKEY", "MOON", "MOUNTAIN", "MOUTH", "MUSIC", "MUTANT", "MYTH", "NEON", "NERVE", "NOSE", "ORACLE", "PATAPHYSICIAN", "PATAPHYSICS", "PLACE", "PLAN", "PLANET", "PLOT", "POEM", "PONY", "POTATO", "PRINCE", "PRINCESS", "PRINCIPLE", "QUEEN", "QUEST", "RAINBOW", "RITUAL", "RIVER", "ROBOT", "ROCKET", "ROOT", "RULES", "SAIL", "SCHOOL", "SCIENCE", "SCREEN", "SCRIPT", "SEARCH", "SECRET", "SEED", "SHIP", "SHOE", "SILVER", "SKIN", "SOCIETY", "SOLUTION", "SONG", "SPIRAL", "SPOON", "STAR", "STREET", "STUDENT", "SUN", "SURPRISE", "TALISMAN", "TEACHER", "TEST", "THOUGHT", "THRONE", "TIME", "TOOL", "TOOTHPICK", "TOY", "TRANCE", "TREE", "TRUTH", "UBU", "UNICORN", "UNIVERSE", "VEHICLE", "VENTURE", "VINE", "WATER", "WHALE", "WHEEL", "WINDOW", "WING", "WISH", "WOMAN", "WONDER", "WOOD", "WORLD"};



int  clickCount = 0; // Fabio: set the click count to the start;

char* msgLine = "            Click for pataphysical instructions from the future.";
int crtPos = 0;
int crtColor = GREEN;
int nSpeed = 1;


// our own copy of the "video" memory; 64 bytes for each of the 4 screen quarters;
// each 64-element array maps 2 planes:
// indexes from 0 to 31 are allocated for green plane;
// indexes from 32 to 63 are allocated for red plane;
// when a bit is 1 in both planes, it is displayed as orange (green + red);
byte ht1632_shadowram[64][4*Number_of_Displays] = {0};



/*
 * Set these constants to the values of the pins connected to the SureElectronics Module
 */
static const byte ht1632_data = 14;  // Data pin (pin 7 of display connector)
static const byte ht1632_wrclk = 12; // Write clock pin (pin 5 of display connector)
static const byte ht1632_cs = 13;    // Chip Select (pin 1 of display connnector)
static const byte ht1632_clk = 15; // clock pin (pin 2 of display connector)


//**************************************************************************************************
//Function Name: OutputCLK_Pulse
//Function Feature: enable CLK_74164 pin to output a clock pulse
//Input Argument: void
//Output Argument: void
//**************************************************************************************************
void OutputCLK_Pulse(void) //Output a clock pulse
{
  digitalWriteFast(ht1632_clk, HIGH);
  digitalWriteFast(ht1632_clk, LOW);
}


//**************************************************************************************************
//Function Name: OutputA_74164
//Function Feature: enable pin A of 74164 to output 0 or 1
//Input Argument: x: if x=1, 74164 outputs high. If x?1, 74164 outputs low.
//Output Argument: void
//**************************************************************************************************
void OutputA_74164(unsigned char x) //Input a digital level to 74164
{
  digitalWriteFast(ht1632_cs, (x==1 ? HIGH : LOW));
}


//**************************************************************************************************
//Function Name: ChipSelect
//Function Feature: enable HT1632C
//Input Argument: select: HT1632C to be selected
// If select=0, select none.
// If s<0, select all.
//Output Argument: void
//**************************************************************************************************
void ChipSelect(int select)
{
  unsigned char tmp = 0;
  if(select<0) //Enable all HT1632Cs
  {
    OutputA_74164(0);
    CLK_DELAY;
    for(tmp=0; tmp<CHIP_MAX; tmp++)
    {
      OutputCLK_Pulse();
    }
  }
  else if(select==0) //Disable all HT1632Cs
  {
    OutputA_74164(1);
    CLK_DELAY;
    for(tmp=0; tmp<CHIP_MAX; tmp++)
    {
      OutputCLK_Pulse();
    }
  }
  else
  {
    OutputA_74164(1);
    CLK_DELAY;
    for(tmp=0; tmp<CHIP_MAX; tmp++)
    {
      OutputCLK_Pulse();
    }
    OutputA_74164(0);
    CLK_DELAY;
    OutputCLK_Pulse();
    CLK_DELAY;
    OutputA_74164(1);
    CLK_DELAY;
    tmp = 1;
    for( ; tmp<select; tmp++)
    {
      OutputCLK_Pulse();
    }
  }
}


//**************************************************************************************************
/*
 * ht1632_writebits
 * Write bits (up to 8) to h1632 on pins ht1632_data, ht1632_wrclk
 * Chip is assumed to already be chip-selected
 * Bits are shifted out from MSB to LSB, with the first bit sent
 * being (bits & firstbit), shifted till firsbit is zero.
 */
void ht1632_writebits (byte bits, byte firstbit)
{
  while (firstbit) {
    digitalWriteFast(ht1632_wrclk, LOW);
    if (bits & firstbit) {
      digitalWriteFast(ht1632_data, HIGH);
    } 
    else {
      digitalWriteFast(ht1632_data, LOW);
    }
    digitalWriteFast(ht1632_wrclk, HIGH);
    firstbit >>= 1;
  }
}


//**************************************************************************************************
/*
 * ht1632_sendcmd
 * Send a command to the ht1632 chip.
 */
static void ht1632_sendcmd (byte chipNo, byte command)
{
  ChipSelect(chipNo);
  ht1632_writebits(HT1632_ID_CMD, 1<<2);  // send 3 bits of id: COMMMAND
  ht1632_writebits(command, 1<<7);  // send the actual command
  ht1632_writebits(0, 1); 	/* one extra dont-care bit in commands. */
  ChipSelect(0);
}


//**************************************************************************************************
/*
 * ht1632_senddata
 * send a nibble (4 bits) of data to a particular memory location of the
 * ht1632.  The command has 3 bit ID, 7 bits of address, and 4 bits of data.
 *    Select 1 0 1 A6 A5 A4 A3 A2 A1 A0 D0 D1 D2 D3 Free
 * Note that the address is sent MSB first, while the data is sent LSB first!
 * This means that somewhere a bit reversal will have to be done to get
 * zero-based addressing of words and dots within words.
 */
static void ht1632_senddata (byte chipNo, byte address, byte data)
{
  ChipSelect(chipNo);
  ht1632_writebits(HT1632_ID_WR, 1<<2);  // send ID: WRITE to RAM
  ht1632_writebits(address, 1<<6); // Send address
  ht1632_writebits(data, 1<<3); // send 4 bits of data
  ChipSelect(0);
}


//**************************************************************************************************
void ht1632_setup()
{
  pinMode(ht1632_cs, OUTPUT);
  digitalWriteFast(ht1632_cs, HIGH); 	/* unselect (active low) */
  pinMode(ht1632_wrclk, OUTPUT);
  pinMode(ht1632_data, OUTPUT);
  pinMode(ht1632_clk, OUTPUT);

  for (int j=1; j<=CHIP_MAX; j++)
  {
    ht1632_sendcmd(j, HT1632_CMD_SYSDIS);  // Disable system
    ht1632_sendcmd(j, HT1632_CMD_COMS00);
    ht1632_sendcmd(j, HT1632_CMD_MSTMD); 	/* Master Mode */
    ht1632_sendcmd(j, HT1632_CMD_RCCLK);  // HT1632C
    ht1632_sendcmd(j, HT1632_CMD_SYSON); 	/* System on */
    ht1632_sendcmd(j, HT1632_CMD_LEDON); 	/* LEDs on */
  
  for (byte i=0; i<96; i++)
  {
    ht1632_senddata(j, i, 0);  // clear the display!
  }
  }
}


//**************************************************************************************************
//Function Name: xyToIndex
//Function Feature: get the value of x,y
//Input Argument: x: X coordinate
//                y: Y coordinate
//Output Argument: address of xy
//**************************************************************************************************
byte xyToIndex(byte x, byte y)
{
  byte nChip, addr;

  if (x>=96) {
    nChip = 7 + x/16 + (y>7?2:0);
  } 
  else if (x>=64) {
    nChip = 5 + x/16 + (y>7?2:0);
  }    
   else if (x>=32) {
    nChip = 3 + x/16 + (y>7?2:0);
  }  
  else {
    nChip = 1 + x/16 + (y>7?2:0);
  } 

  x = x % 16;
  y = y % 8;
  addr = (x<<1) + (y>>2);

  return addr;
}

//**************************************************************************************************
//Function Name: calcBit
//Function Feature: calculate the bitval of y
//Input Argument: y: Y coordinate
//Output Argument: bitval
//**************************************************************************************************
#define calcBit(y) (8>>(y&3))


//**************************************************************************************************
//Function Name: get_pixel
//Function Feature: get the value of x,y
//Input Argument: x: X coordinate
//                y: Y coordinate
//Output Argument: color setted on x,y coordinates
//**************************************************************************************************
int get_pixel(byte x, byte y) {
  byte addr, bitval, nChip;

  if (x>=96) {
    nChip = 7 + x/16 + (y>7?2:0);
  } 
  else if (x>=64) {
    nChip = 5 + x/16 + (y>7?2:0);
  }    
   else if (x>=32) {
    nChip = 3 + x/16 + (y>7?2:0);
  }  
  else {
    nChip = 1 + x/16 + (y>7?2:0);
  } 

  addr = xyToIndex(x,y);
  bitval = calcBit(y);

  if ((ht1632_shadowram[addr][nChip-1] & bitval) && (ht1632_shadowram[addr+32][nChip-1] & bitval)) {
    return ORANGE;
  } else if (ht1632_shadowram[addr][nChip-1] & bitval) {
    return GREEN;
  } else if (ht1632_shadowram[addr+32][nChip-1] & bitval) {
    return RED;
  } else {
    return 0;
  }
}


//**************************************************************************************************
/*
 * plot a point on the display, with the upper left hand corner
 * being (0,0), and the lower right hand corner being (31, 15);
 * parameter "color" could have one of the 4 values:
 * black (off), red, green or yellow;
 */
void ht1632_plot (int x, int y, byte color)
{
   byte nChip, addr, bitval;
  
  if (x<0 || x>X_MAX || y<0 || y>Y_MAX)
    return;
  
  if (color != BLACK && color != GREEN && color != RED && color != ORANGE)
    return;
  
  if (x>=96) {
    nChip = 7 + x/16 + (y>7?2:0);
  } 
  else if (x>=64) {
    nChip = 5 + x/16 + (y>7?2:0);
  }    
   else if (x>=32) {
    nChip = 3 + x/16 + (y>7?2:0);
  }  
  else {
    nChip = 1 + x/16 + (y>7?2:0);
  } 
  
  addr = xyToIndex(x,y);
  bitval = calcBit(y);
  
  switch (color)
  {
    case BLACK:
      if (get_pixel(x,y) != BLACK) { // compare with memory to only set if pixel is other color
        // clear the bit in both planes;
        ht1632_shadowram[addr][nChip-1] &= ~bitval;
        ht1632_senddata(nChip, addr, ht1632_shadowram[addr][nChip-1]);
        addr = addr + 32;
        ht1632_shadowram[addr][nChip-1] &= ~bitval;
        ht1632_senddata(nChip, addr, ht1632_shadowram[addr][nChip-1]);
      }
      break;
    case GREEN:
      if (get_pixel(x,y) != GREEN) { // compare with memory to only set if pixel is other color
        // set the bit in the green plane and clear the bit in the red plane;
        ht1632_shadowram[addr][nChip-1] |= bitval;
        ht1632_senddata(nChip, addr, ht1632_shadowram[addr][nChip-1]);
        addr = addr + 32;
        ht1632_shadowram[addr][nChip-1] &= ~bitval;
        ht1632_senddata(nChip, addr, ht1632_shadowram[addr][nChip-1]);
      }
      break;
    case RED:
      if (get_pixel(x,y) != RED) { // compare with memory to only set if pixel is other color
        // clear the bit in green plane and set the bit in the red plane;
        ht1632_shadowram[addr][nChip-1] &= ~bitval;
        ht1632_senddata(nChip, addr, ht1632_shadowram[addr][nChip-1]);
        addr = addr + 32;
        ht1632_shadowram[addr][nChip-1] |= bitval;
        ht1632_senddata(nChip, addr, ht1632_shadowram[addr][nChip-1]);
      }
      break;
    case ORANGE:
      if (get_pixel(x,y) != ORANGE) { // compare with memory to only set if pixel is other color
        // set the bit in both the green and red planes;
        ht1632_shadowram[addr][nChip-1] |= bitval;
        ht1632_senddata(nChip, addr, ht1632_shadowram[addr][nChip-1]);
        addr = addr + 32;
        ht1632_shadowram[addr][nChip-1] |= bitval;
        ht1632_senddata(nChip, addr, ht1632_shadowram[addr][nChip-1]);
      }
      break;
  }
}


//**************************************************************************************************
/*
 * ht1632_clear
 * clear the display, and the shadow memory, and the snapshot
 * memory.  This uses the "write multiple words" capability of
 * the chipset by writing all 96 words of memory without raising
 * the chipselect signal.
 */
void ht1632_clear()
{
  for (int i=1; i<=8; i++)
  {
    ChipSelect(-1);
    ht1632_writebits(HT1632_ID_WR, 1<<2);  // send ID: WRITE to RAM
    ht1632_writebits(0, 1<<6); // Send address
    for (i = 0; i < 96/2; i++) // Clear entire display
      ht1632_writebits(0, 1<<7); // send 8 bits of data
    ChipSelect(0);

    for (int j=0; j < 64; j++)
      ht1632_shadowram[j][i] = 0;
  }
}




/***********************************************************************
 * traditional Arduino sketch functions: setup and loop.
 ***********************************************************************/

void setup ()  // flow chart from page 17 of datasheet
{
  digitalWrite(buttonPin, HIGH);
  ht1632_setup(); 
  randomSeed(analogRead(0));
  
  Serial.begin(9600);

  cls();
}

boolean  lastButtonState = HIGH;
boolean  buttonState = LOW;
long lastPush = 0;



void loop ()
{
  lastButtonState = buttonState;
  buttonState = digitalRead(buttonPin);
  // Serial.println(clickCount); Fabio: Remove all the 1's and 2's from the serial line.

  // allow 100 milli-second between button pushes, for debouncing;
  if (buttonState == LOW && (millis()-lastPush > 100) && lastButtonState == HIGH) // only leading edge of button press
  {
    // button pressed;
    lastPush = millis();
    
  if (clickCount == 2) // Fabio: check to see is this the even click,  get new prompt message
  {
        // Fabio: display opening message;
    sprintf(msgLine, "            Click for instructions from the future.");
//    Serial.println(msgLine);
    // start from the beginning of the message;
    crtPos = 0;       
    clickCount = 1; // Fabio: reset to null value;
  }  
  else if (clickCount == 1) // if odd click get Oracular messe, and send to speaking Arduino
  {
    // make a new random message;

    sprintf(msgLine, "         %s   %s   %s", FirstWord[random(130)], SecondWord[random(125)], ThirdWord[random(200)]);
    Serial.println(msgLine);
    // start from the beginning of the message;
    crtPos = 0;    
    clickCount = 2; // Fabio: get ready to return to start by incrementing the click count;
   } 
   else
   {
//    Serial.println(msgLine);  // the original first message, until a button press
    // start from the beginning of the message;
    crtPos = 0;
    clickCount = 1;
   }
 }


  displayLargeScrollingLine();

  if (crtPos >= strlen(msgLine))
      crtPos = 0;
    
 
      
}



//*********************************************************************************************************
// Display scrolling text with large 14 bit high by 2-11 bit wide proportional font
//*********************************************************************************************************
//
void displayLargeScrollingLine()
{
	int leftCol, tmpPos, x;

	// shift the 14 bit high characters one character to the left, 1 character is 2 - 11 bits wide;
	for (leftCol = 0; leftCol > -12; --leftCol)	
	{
		x = leftCol;
		tmpPos = crtPos;
		do
		{
			x = ht1632_putLargeChar(x, 1, ((tmpPos < strlen(msgLine)) ? msgLine[tmpPos] : ' '), crtColor);
			++tmpPos;
		}	
		while ((x < X_MAX + 1) && (x != 0));		// fill display with 32 dots and stop if 1 first char is moved out of the display;	
//		delay(40 - nSpeed * 10);
		if (x == 0)
			break;					// we have moved the first complete character so stop;
	}
	++crtPos;
}



//**************************************************************************************************
int ht1632_putLargeChar(int x, int y, char c, byte color)
{
	// fonts defined for ascii 32 and beyond (index 0 in font array is ascii 32);
	byte charIndex, col, row;
	
	// replace undisplayable characters with blank;
	if (c < 32 || c > 127)
	{
		charIndex	=	0;
	}
	else
	{
		charIndex	=	c - 32;
	}

	// move character definition, pixel by pixel, onto the display;
	// Fonts are defined as up to 14 bit per row and max. 11 columns;
	// first row is always zero to create the space between the characters;
	
	for (col=0; col < 11; ++col)					// max 11 columns;
	{
		uint16_t dots = pgm_read_word_near(&largeFont[charIndex][col]);
		if (dots == 0) 								// stop if all bits zero;
			break;
	
		for (row=0; row < 14; row++) 
		{
			if (dots & (0x4000 >> row))    			// max 14 rows;
				plot(x+col, y+row, color);
			else 
				plot(x+col, y+row, color & PUTINSNAPSHOTRAM);
		}
	}
	return x+col;
}


