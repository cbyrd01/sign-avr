# Protocol for communicating to the Arduino over I2C
I2C all communication is initiated by i2c-master (RPI/Node) to the i2c-slave (Arduino).

This uses a three byte message:

|cmd|option|value|
|---|------|-----|

For ease of troubleshooting, cmd and option are numeric digits 0-9. Value is a binary number from 0-255. All three bytes are always present (fixed length message), when not needed they are set to zero.

## Commands

|number|meaning|
|------|-------|
|0-7|Set first-eigth strand color|
|8|Set gear|
|9|Special mode|
|0|future use|

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
|0|0|255|Set the first LED strand to red|
|8|0|0|Turn off the gear|
|8|1|0|Turn on the gear|
|9|0|0|All LED off|
|9|1|0|All LED on|
|9|2|100|Blink LED with 100ms transition|