# TLC5947 Library for AVR-G++

## Features
- Supports up to 255 daisy-chained chips.
- Allows for enabling or disabling all outputs simultaneously.
- Uses hardware SPI so that you can get the most out of 16MHz.

## Physical Constraints
The TLC5947 has 12 bits of resolution for each of its 24 channels. Each channel is a constant-current sink, meaning that it cannot source any current. It also does not matter what voltage is applied to the pins, you can run the LEDS from up to 30V.

If you are daisy-chaining more than one TLC5947, connect the SOUT of the first TLC to the SIN of the next.  All the other pins should just be connected together. The one exception is that each TLC needs it's own resistor between  pin 31 and GND.

- The 1k resistor between TLC pin 31 and GND will let ~30mA through each LED. This is calculated by the equation I = 49.2 / R. This doesn't depend on the LED driving voltage.
- (Optional): put a pull-up resistor (~10k) between BLANK and VCC so that all the LEDs will turn off when the AVR is reset.

### Compatibility
This library uses SPI to communicate, so it may conflict with any other libraries use SPI.

## Functions:

### TLC5947()
The constructor for the TLC5947 library. If no pins are specified, the chip will default to whatever pins you chose for the first one.

### TLC5947(latch, blank)
The constructor for the TLC5947 library. You must specify a latch and blank pin for the first chip that is declared.
#### Arguments
- `latch`: The pin that this chip's latch control is connected to (e.g. PB5).
- `blank`: The pin that this chip's blank control is connected to (e.g. PB3).

### chipID()
Returns the ID number of the current chip.

### read(channel)
Returns the current value of the specified channel.
#### Arguments
- `channel`: Channel to be read.

### set(values[24])
Sets all channels based on the given array.
#### Arguments
- `values[24]`: Brightness values for all channels. Range is [0-4095].

### set(value)
Sets all channels to the same value.
#### Arguments
- `value`: Brightness value for all channels. Range is [0-4095].

### set(channel, value)
Sets one channel to the specified value.
#### Arguments
- `channel`: Channel to be set.
- `value`: Brightness value for the channel. Range is [0-4095].

### clear()
Sets all channels to 0.

### enable()
Enable the chip by pulling the BLANK pin low.

### disable()
Disable the chip by pulling the BLANK pin high.

### latch()
Latches the data to the outputs.

## Static Functions

### numChips()
Returns the total number of chips.

### setAll(value);
Sets all channels on all chips to the specified value.
#### Arguments
- `value`: Brightness value for the channel. Range is [0-4095].

### clearAll()
Sets all channels on all chips to 0.

### enableSPI()
Enable the SPI interface.

### disableSPI()
Disable the SPI interface.

### send()
Shifts the data out to the chips.

### update()
Calls enableSPI() if needed, then send() and latch(). This is all you should use unless your application requires finer control.

### shift(shift, value);
Shifts all data in all chips by the given number of channels. If value is left blank, a circular shift is performed, whereby the data being shifted out of the end gets added back to the beginning.
#### Arguments
- `shift`: Number of channels to shift data by. Defaults to 1.
- `value`: Brightness value to be shifted in. Range is [0-4095].

## TODO
- [x] Allow the user to choose a custom pin for XLAT
- [x] Allow for per-chip blank control
- [ ] Fix the code for shifting an odd number of channels
- [ ] Do a check for duplicate pins when calling `update()`
