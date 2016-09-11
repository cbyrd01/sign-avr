#include <Adafruit_NeoPixel.h>

// five strips, one each for F O R G E
#define STARTPIN 6
#define POWERTAILPIN 12
#define REEDSWITCHPIN 11

const int NUM_PIXELS  = 120; // 120 pixels per strip
const int NUM_STRIPS  = 5;   // one strip per letter
const int FULL_BRIGHT = 255; // 0-255
const int OFF         = 0;   // 0-255

// Letter O is one short pixels (oops), so we have to keep a list per letter
int numLed[] = {NUM_PIXELS, NUM_PIXELS - 1, NUM_PIXELS, NUM_PIXELS, NUM_PIXELS};


// We setup 5 strips, one for each letter.
Adafruit_NeoPixel strip[NUM_STRIPS] = {
  Adafruit_NeoPixel(numLed[0],STARTPIN, NEO_GRB + NEO_KHZ800),   // F
  Adafruit_NeoPixel(numLed[1],STARTPIN+1, NEO_GRB + NEO_KHZ800), // O
  Adafruit_NeoPixel(numLed[2],STARTPIN+2, NEO_GRB + NEO_KHZ800), // R
  Adafruit_NeoPixel(numLed[3],STARTPIN+3, NEO_GRB + NEO_KHZ800), // G
  Adafruit_NeoPixel(numLed[4],STARTPIN+4, NEO_GRB + NEO_KHZ800)  // E
};

// Serial variables
char cmd = NULL;
int curCharNum = 0;
char valueList[8] = {};

void setup()
{
  // Start serial at 38400 baud
  Serial.begin(9600);
  for(int i=0; i<NUM_STRIPS;i++) {
    strip[i].begin();
    strip[i].setBrightness(FULL_BRIGHT);
  }
}

void setStripColor(int letterNumber, int redValue, int greenValue, int blueValue) {
  for(int i=0; i<numLed[letterNumber]; i++) {
    strip[letterNumber].setPixelColor(i, redValue, greenValue, blueValue);
  }
  strip[letterNumber].show();
}
void allOn() {
  for(int i=0; i<NUM_STRIPS;i++) {
    setStripColor(i, FULL_BRIGHT, FULL_BRIGHT, FULL_BRIGHT);  
  }
}

void allOff() {
  for(int i=0; i<NUM_STRIPS;i++) {
    setStripColor(i, OFF, OFF, OFF);  
  }
}

void doFlash(int numTimes,int myDelay) {
 for(int i=0; i<numTimes;i++) {
    allOn();
    delay(myDelay);
    allOff();
    delay(myDelay);
  }
}


void doMarquee() {
  for(int i=0; i<NUM_STRIPS;i++) {
   setStripColor(i, FULL_BRIGHT, FULL_BRIGHT, FULL_BRIGHT);  
   delay(1000);
  }
  doFlash(3,500);
}

void runSpecial(int valType) {
  switch(valType)
  {
    case 0: doMarquee();break;
    case 1: doFlash(10,500); break;
    case 2: allOn(); break;
    case 3: allOff(); break;
  }
}

void resetCommand() {
  cmd = NULL;
  curCharNum = 0;
  for(int i=0; i<8;i++) {
    valueList[i] = NULL;
  }
}


void loop()
{
  if(!Serial.available()) {
    return;
  }

  char readChar = Serial.read(); 

  // Reset whenever an end of command character ";" is read
  if (readChar == ';') {
    resetCommand();
    return;
  }

  // First read a command, then a value.  If a command hasn't been read yet, 
  // then read it and store for next iteration of the loop.
  if (!cmd) {
    cmd = readChar;
  }
  else {
    valueList[curCharNum] = readChar;
    // values are two characters, read the first character and store for next
    // iteration of the loop if one hasn't already been read
    if(curCharNum == 7) {

      char mybuf [3];

      sprintf(mybuf,"%c%c",valueList[0],valueList[1]);
      int lightNumber = strtol(mybuf,NULL,16);

      sprintf(mybuf,"%c%c",valueList[2],valueList[3]);
      int redValue = strtol(mybuf,NULL,16);

      sprintf(mybuf,"%c%c",valueList[4],valueList[5]);
      int greenValue = strtol(mybuf,NULL,16);

      sprintf(mybuf,"%c%c",valueList[6],valueList[7]);
      int blueValue = strtol(mybuf,NULL,16);

      // After getting the command and value, determine what to execute
      switch(cmd)
      {
        case 'c': setStripColor(lightNumber, redValue, greenValue, blueValue);break;
        case 's': runSpecial(lightNumber); break;
      }
      // After running command, clear it before next iteration
      resetCommand();
    }
    curCharNum++;
  }
}

