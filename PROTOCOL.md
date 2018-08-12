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
|8|Set gear|
|9|Special mode|
|10-15|future use|

## Options
### Command
When cmd is 0-7 (set color), option controls which color is being set

|number|meaning|
|------|-------|
|0|red|
|1|green|
|2|blue|

### Gear
When cmd is 8, option controls what to do with the gear

|number|meaning|
|------|-------|
|0|Off|
|1|On|

### Special mode
When the cmd is 9, option controls what special mode should be used

|number|meaning|
|------|-------|
|0|All off|
|1|All on|
|2|blink|

## Value
Value provides what value (from 0-255) to use and is dependant on the option selected.

### Strand color
When cmd is 0-7 (set color), the value is the intensity of the color (0-255). 

### Set gear
Value is not used when cmd is 8, and should be set to 0

### Special mode
When cmd is 9 (special mode) the value depends on the special mode used.

|option|meaning|value|
|------|-------|-----|
|0|All off|n/a (0)|
|1|All on|n/a (0)|
|2|blink|transition speed ms (0-255)|

## Examples


|cmd|option|value|meaning|
|---|------|-----|-------|
|0|0|255|Set the first LED strand red to full intensity|
|3|2|128|Set the fourth LED strand blue to half intensity|
|8|0|0|Turn off the gear|
|8|1|0|Turn on the gear|
|9|0|0|All LED strips off|
|9|1|0|All LED strips on full white|
|9|2|100|Blink LED with 100ms transition|