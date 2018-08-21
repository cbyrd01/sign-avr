//
// Arduino code for Inventor Forge Makerspace sign. Hardware is an Adafruit M0 Feather Express with a NeoPXL8 FeatherWing
// to control the Adafruit NeoPixel strips. There are five strips, one each for the sign letters F O R G E. The protocol
// is defined in PROTOCOL.md

#include <Arduino.h>           // Base Arduino
#include <Wire.h>              // Wire library for I2C
#include <Adafruit_NeoPXL8.h>  // Adafruit NeoPXL8 LED library

#define SLAVE_ADDRESS 0x08     // I2C slave address

#define REEDSWITCHPIN 11       // Digitial input pin for sensing when to stop the gear
#define POWERTAILPIN 12        // Digital output pin which controls the gear

const int NUM_PIXELS  = 120;   // 120 pixels per strip
const int NUM_STRIPS  = 8;     // one strip per letter, NeoPXL8 has max of 8 strips, extras are ignored
const int FULL_BRIGHT = 255;   // Basic constant for full intensity (0-255)
const int OFF         = 0;     // Basic constant for off (0-255)
const int BRIGHTNESS = 255;    // Set strip brightness (0-255)
const int MAXGEARTIME = 60000; // Time in mills until the gear stops without seeing the reed switch

// Shared state variables
int newCommandFlag = 0;        // flag if we get commands from I2C
int ongoingEffectFlag = 0;     // flag if we need to process an ongoing lighting or gear effect
byte receivedCommand[2];       // store what we got from I2C
int parsedCmd = 0;             // store the result of parsing the cmd from receivedCommand
int parsedOption = 0;          // store the result of parsing the option from receivedCommand
int parsedValue = 0;           // store the result of parsing the value from receivedCommand
int stopGearFlag = 0;          // flag that we want to stop the gear from turning
unsigned long timerStart = millis();
// Matrix to hold each LED strip and rgb values
int stripMatrix[8][3] = {
  {0,0,0}, // F
  {0,0,0}, // O
  {0,0,0}, // R
  {0,0,0}, // G
  {0,0,0}, // E
  {0,0,0}, // future use
  {0,0,0}, // future use
  {0,0,0}  // future use
};
// Matrix to hold preset color rgb values
int colorMatrix[10][3] = {
  {0,0,0},       // black (off)
  {255,255,255}, // white (on)
  {255,0,0},     // red
  {0,255,0},     // green
  {0,0,255},     // blue
  {245,130,48},  // orange
  {255,225,25},  // yellow
  {70,240,240},  // cyan
  {250,190,190}, // pink
  {145,30,180}   // purple
};

// Set up the NeoPXL8 LEDs. Sixth pin has been changed from SDA to MOSI
int8_t pins[8] = { PIN_SERIAL1_RX, PIN_SERIAL1_TX, MISO, 13, 5, MOSI, A4, A3 };
Adafruit_NeoPXL8 leds(NUM_PIXELS, pins, NEO_GRB);

// The following function is called when I2C receives data
// The slave doesn't ACK until this function returns
// So it breaks stuff to spend too much time here
void receiveData(int byteCount){
  int receiveByte = 0;        // set index to 0
  while(Wire.available())     // loop through all incoming bytes
  {
   receivedCommand[receiveByte] = Wire.read(); // receive byte as a character
   receiveByte++;             // increase index by 1
  }
    newCommandFlag = 1;
}

// The following function sets the LED strips to a specified RGB color
void setStripColor(int stripNumber, int redValue, int greenValue, int blueValue) {
  // NeoPXL8 treats all strips as a single long strip. Find the starting pixel for the virtual "strip"
  int startPixel = stripNumber*NUM_PIXELS;
  // Iterate through all individual pixels on the strip
  for(int p=startPixel; p<startPixel+NUM_PIXELS; p++) {
    // Set the strip to the specified values
    leds.setPixelColor(p, redValue, greenValue, blueValue);
  }
}

// The following function sets all LED strips to a specified RGB color
void setAllStripColor(int redValue, int greenValue, int blueValue) {
  // Iterate through all LED strips
   for(int s=0; s<NUM_STRIPS; s++) {
     // Set the strip to the specified values
    setStripColor(s, redValue, greenValue, blueValue);
  }
}

void gearOn() {
  digitalWrite(POWERTAILPIN, HIGH);
}

void gearOff() {
    digitalWrite(POWERTAILPIN, LOW);
}

void setup()
{
  // Start serial at 9600 baud
  Serial.begin(9600);
  
  // Set up NeoPXL8 LEDs
  leds.begin();
  leds.setBrightness(BRIGHTNESS);

  // Set up pins for gear
  pinMode(POWERTAILPIN, OUTPUT);
  pinMode(REEDSWITCHPIN, INPUT_PULLUP);

 // Starting I2C slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
}

void loop()
{
  // Check for new commands
  if (newCommandFlag) {
    // parse the parts of the received command
    parsedCmd = receivedCommand[0] << 4;
    parsedOption = receivedCommand[0] >> 4;
    parsedValue = receivedCommand[1];
    // decide what to do with the new command
    if (parsedCmd < 8) { // cmd 0-7 means an update to an LED strip
      stripMatrix[parsedCmd][parsedOption] = parsedValue;
      setStripColor(parsedCmd, stripMatrix[parsedCmd][0],stripMatrix[parsedCmd][1],stripMatrix[parsedCmd][2]);
      leds.show();
    }
    else if (parsedCmd == 8) { // cmd 8 means a lighting effect
      if (parsedOption == 0) { // all effects OFF
        // TODO
      }
      else if (parsedOption == 1) { // all LEDs to a preset color
        for(int s=0; s<NUM_STRIPS; s++){
          for(int c=0; c<3; c++) {
          stripMatrix[s][c] = colorMatrix[parsedValue][c];
          }
        }
        setAllStripColor(colorMatrix[parsedValue][0], colorMatrix[parsedValue][1], colorMatrix[parsedValue][2]);
        leds.show();
      }
      else if (parsedOption == 2) { // "Breathing" effect
        // TODO
      }
      else if (parsedOption == 3) { // "Fire" effect
        // TODO
      }
      else if (parsedOption == 4) { // "Build" effect
        // TODO
      }
      else if (parsedOption == 5) { // "Rotate" effect
        // TODO
      }
      else if (parsedOption == 6) { // "Rainbow" effect
        // TODO
      }     
      else if (parsedOption == 7) { // "Bounce" effect
        // TODO
      }     
      else {
        // do something with error or ignore
      }
    }
        else if (parsedCmd == 9) { // update to gear mode

      if (parsedOption == 0) { // stop the gear
        stopGearFlag = 1;
        // Start the failsafe timer
        timerStart = millis();
        // Add an interrupt to stop the gear if POWERTAILPIN changes
        attachInterrupt(REEDSWITCHPIN, gearOff, FALLING);
      }
      else if (parsedOption == 1) { // start the gear
        stopGearFlag = 0;
        // Remove the interrupt on the reed switch pin
        detachInterrupt(REEDSWITCHPIN); 
        gearOn();
      }
      else {
        // do something with error or ignore
      }
    }
    else {
      // do something with error or ignore
    }
  }

  // Check to see if its time to stop the gear
  if (stopGearFlag && millis() - timerStart >= MAXGEARTIME) {
    detachInterrupt(REEDSWITCHPIN); 
    gearOff();
  }
 
  // Run ongoing effects
  if (ongoingEffectFlag) {
    // TODO
  }
}