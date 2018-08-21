# sign-avr
Arduino code for Inventor Forge sign. The Arduino (an Adafruit M0 Feather Express is connected to a Raspberry Pi running Node.js. The protocol is defined in [the protocol document](PROTOCOL.md). The Adafruit M0 Feather Express is connected to a NeoPXL8 FeatherWing that controls each NeoPixel strip.

## Installation

1. Install platformio: [Installation
   Instructions](http://docs.platformio.org/en/stable/ide/atom.html#installation)
2. Start / upload, see: [CLI
   tools](http://docs.platformio.org/en/stable/userguide/index.html)

# TODO
1. Look into verifying received command before processing
1. Fix serial for monitor/debug
1. Add new modes