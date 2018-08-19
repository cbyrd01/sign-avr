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
const int FULL_BRIGHT = 255;   // 0-255
const int OFF         = 0;     // 0-255
const int BRIGHTNESS = 255;    // Set strip brightness
const int MAXGEARTIME = 30000; // Time in mills until the gear stops without seeing the reed switch

// Shared state variables
int newCommandFlag = 0;        // flag if we get commands from I2C
int ongoingEffectFlag = 0;     // flag if we need to process an ongoing lighting or gear effect
byte receivedCommand[2];       // store what we got from I2C
int parsedCmd = 0;             // store the result of parsing the cmd from receivedCommand
int parsedOption = 0;          // store the result of parsing the option from receivedCommand
int parsedValue = 0;           // store the result of parsing the value from receivedCommand
int stopGearFlag = 0;          // flag that we want to stop the gear from turning
unsigned long currentMillis = millis();
// Matrix to hold each LED strip and rgb values
int stripMatrix[8][3] = {
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0}
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
  digitalWrite(13, HIGH);
}

void gearOff() {
    if(digitalRead(REEDSWITCHPIN) == LOW) {
      digitalWrite(POWERTAILPIN, LOW);
      digitalWrite(13, LOW);
    }
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
  pinMode(13, OUTPUT);
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
    if (parsedCmd < 8) {
      // cmd 0-7 means an update to an LED strip
      stripMatrix[parsedCmd][parsedOption] = parsedValue;
      setStripColor(parsedCmd, stripMatrix[parsedCmd][0],stripMatrix[parsedCmd][1],stripMatrix[parsedCmd][2]);
      leds.show();
    }
    else if (parsedCmd == 8) {
      // cmd 8 means an update to gear mode
      if (parsedOption == 0) {
        // stop the gear
        stopGearFlag = 1;
      }
      else if (parsedOption == 1) {
        // start the gear
        stopGearFlag = 0;
        gearOn();
      }
      else {
        // do something with error or ignore
      }
    }
    else if (parsedCmd = 9) {
      // cmd 9 means a special command
      if (parsedOption == 0) {
        // all LEDs off
        for(int s=0; s<NUM_STRIPS; s++){
          for(int p=0; p<NUM_PIXELS; p++) {
          stripMatrix[s][p] = OFF;
          }
        }
        setAllStripColor(OFF, OFF, OFF);
        leds.show();
      }
      else if (parsedOption == 1) {
        // all LEDs on
        for(int s=0; s<NUM_STRIPS; s++){
          for(int p=0; p<NUM_PIXELS; p++) {
          stripMatrix[s][p] = FULL_BRIGHT;
          }
        }
        setAllStripColor(FULL_BRIGHT, FULL_BRIGHT, FULL_BRIGHT);
        leds.show();
      }
      else if (parsedOption == 2) {
        // blink LEDs
      }
      else {
        // do something with error or ignore
      }
    }
    else {
      // do something with error or ignore
    }
  }

  // Run ongoing effects
  if (stopGearFlag) {
    gearOff();
  }

  if (ongoingEffectFlag) {
    // Do stuff
  }
}