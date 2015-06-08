# TLC5947 Library for Arduino
For documentation, please visit [hexa.io](http://hexa.io)

## Features
- Supports up to 255 daisy-chained chips.

## Physical Constraints
The TLC5947 has 12 bits of resolution for each of its 24 channels. Each channel is a constant-current sink, meaning that it cannot source any current. It also does not matter what voltage is applied to the pins, you can run the LEDS from up to 30V.

If you are daisy-chaining more than one TLC5947, connect the SOUT of the first TLC to the SIN of the next.  All the other pins should just be connected together. The one exception is that each TLC needs it's own resistor between  pin 31 and GND.

- The 1k resistor between TLC pin 31 and GND will let ~30mA through each LED. This is calculated by the equation I = 49.2 / R. This doesn't depend on the LED driving voltage.
- (Optional): put a pull-up resistor (~10k) between BLANK and VCC so that all the LEDs will turn off when the Arduino is reset.

### Compatibility
This library uses pins 9, 10, 11, and 13 on the Arduino Uno. Please do not use these pins.

## Functions:

### TLC5947(nInitialValue)
The constructor for the TLC5947 library.
#### Arguments
- `nInitialValue`: Optional argument for the initial value assigned to every channel. Range is [0-4095]. Defaults to 0.

### chipID()
Returns the ID number of the current chip.

### set(nValue)
Sets all channels on all chips to the same value.
#### Arguments
- `nValue`: Brightness value for all channels. Range is [0-4095].

### set(nChannel, nValue)
Sets one channel to the specified value.
#### Arguments
- `nChannel`: Channel to be set.
- `nValue`: Brightness value for the channel. Range is [0-4095].

### read(nChannel)
Returns the current value of the specified channel.
#### Arguments
- `nChannel`: Channel to be read.

### clear()
Sets all channels to 0.

## Static Functions

### numChips()
Returns the total number of chips.

### set(nChip, nChannel, nValue);
Sets one channel on a specific chip to the specified value.
#### Arguments
- `nChip`: Chip to be set.
- `nChannel`: Channel to be set.
- `nValue`: Brightness value for the channel. Range is [0-4095].

### read(nChip, nChannel)
Returns the current value of the specified channel.
#### Arguments
- `nChip`: Chip to be read.
- `nChannel`: Channel to be read.

### clear(nChip)
Sets all channels on the given chip to 0.
#### Arguments
- `nChip`: Chip to be cleared.

### clearAll()
Sets all channels on all chips to 0.

### shift(nShift, nValue);
Shifts all data in all chips by the given number of channels.
#### Arguments
- `nShift`: Number of channels to shift data by. Defaults to 1.
- `nValue`: Brightness value to be shifted in. Range is [0-4095]. Defaults to 4095.

### update()
Sends the data out to the chips and displays it.

## TODO
- Make set() and setAll() more concise
- Improve documentation
- Provide some form of BLANK control
- Check compatibility with the new Arduino IDE
- Allow the user to choose a custom pin for XLAT
