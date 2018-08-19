# Protocol for communicating to the Arduino over I2C
I2C all communication is initiated by i2c-master (RPI/Node) to the i2c-slave (Arduino).

This uses a two byte message:

|cmd|option|value|
|---|------|-----|
|4 bit|4 bit| 8 bit|

Cmd and option are 4 bit integers from 0-15. Value is a 8 bit integer from 0-255. Two bytes are always present (fixed length message), when value is not needed it is set to zero.

## Commands

|number|meaning|
|------|-------|
|0-7|Set first-eigth strand color|
|8|Lighting effect|
|9|Set gear|
|10-15|future use|

## Options
### Command
When cmd is 0-7 (set color), option controls which color is being set

|number|meaning|
|------|-------|
|0|red|
|1|green|
|2|blue|

### Lighting effect mode
When the cmd is 8, option controls what lighting effect mode should be used

|number|meaning|
|------|-------|
|0|Lighting effect off|
|1|Preset color|
|2|blink|

### Gear
When cmd is 9, option controls what to do with the gear

|number|meaning|
|------|-------|
|0|Off|
|1|On|

## Value
Value provides what value (from 0-255) to use and is dependant on the option selected.

### Strand color
When cmd is 0-7 (set color), the value is the intensity of the color (0-255). 

### Special mode
When cmd is 8 (lighting effect mode) the value depends on the lighting effect used.

|option|meaning|value|
|------|-------|-----|
|0|Lighting effects off|n/a (0)|
|1|Preset color|preset color (0-9)|
|2|blink|transition speed ms (0-255)|

### Set gear
Value is not used when cmd is 9, and should be set to 0

## Examples

|cmd|option|value|meaning|
|---|------|-----|-------|
|0|0|255|Set the first LED strand red to full intensity|
|3|2|128|Set the fourth LED strand blue to half intensity|
|8|0|0|All lighting effects off|
|8|1|2|LED color preset 2 (red)|
|8|2|100|Blink LED with 100ms transition|
|9|0|0|Turn off the gear|
|9|1|0|Turn on the gear|