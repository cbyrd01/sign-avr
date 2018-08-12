#include <Arduino.h> // Base Arduino
#include <Wire.h> // Wire library for I2C
#include <Adafruit_NeoPXL8.h> // Adafruit NeoPXL8 LED library

#define SLAVE_ADDRESS 0x08 // I2C slave address

#define REEDSWITCHPIN 11 // Digitial input pin for sensing when to stop the gear
#define POWERTAILPIN 12 // Digital output pin which controls the gear

const int NUM_PIXELS  = 120; // 120 pixels per strip
const int NUM_STRIPS  = 5;   // one strip per letter
const int FULL_BRIGHT = 255; // 0-255
const int OFF         = 0;   // 0-255

// Shared state variables
int newCommand = 0; // flag if we get commands from I2C
char command[3]; // store what we got from I2C
int gearStop = 0;
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
  int receiveByte = 0; // set index to 0
  while(Wire.available()) // loop through all incoming bytes
  {
   command[receiveByte] = Wire.read(); // receive byte as a character
   receiveByte++; // increase index by 1
  }
    Serial.print("Did you hear something? I saw: "); // Debugging to serial
    Serial.println(String(command[0])); // Debugging to serial
    newCommand = 1;
}

// The following function sets the LED strps to a specified RGB color
void setStripColor(int stripNumber, int redValue, int greenValue, int blueValue) {
  int startPixel = stripNumber*NUM_PIXELS;
  for(int p=0+startPixel; p<startPixel+NUM_PIXELS; p++) {
    leds.setPixelColor(p, redValue, greenValue, blueValue);
  }
  leds.show();
}

// This function turns on all of the strips at full bright (white)
void allOn() {
  for(int i=0; i<NUM_STRIPS;i++) {
    setStripColor(i, FULL_BRIGHT, FULL_BRIGHT, FULL_BRIGHT);  
  }
}

// This function turns off all of the strips at once
void allOff() {
  for(int i=0; i<NUM_STRIPS;i++) {
    setStripColor(i, OFF, OFF, OFF);  
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

void doGear(int turnOn) {
  if(turnOn) {
    gearStop = 0;
    gearOn();
  }
  else {
    // turn off
    gearStop = 1;
  }
}

void setup()
{
  // Start serial at 9600 baud
  Serial.begin(9600);
  
  // Set up NeoPXL8 LEDs
  leds.begin();
  leds.setBrightness(FULL_BRIGHT);

  // Set up pins for Gear
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
  if(gearStop == 1) {
    gearOff();
  }
  else {
    gearOn();
  }

}

