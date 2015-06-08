// ======================================================================== //
//  Pin setup:                                                              //
//                                                                          //
// -------                                    __    __                      //
// ARDUINO|                                  |  \__/  |                     //
//      13| SCLK  (pin 3)                GND |1     32| VCC (+5V)           //
//      12|                   BLANK (pin 10) |2     31| IREF (R -> GND)     //
//      11| SIN   (pin 4)      SCLK (pin 13) |3     30| XLAT (pin 9)        //
//      10| BLANK (pin 2)       SIN (pin 11) |4     29| SOUT (next TLC SIN) //
//       9| XLAT  (pin 30)              OUT0 |5     28| OUT23               //
//       8|                             OUT1 |6     27| OUT22               //
//       7|                             OUT2 |7     26| OUT21               //
//       6|                             OUT3 |8     25| OUT20               //
//       5|                             OUT4 |9     24| OUT19               //
//       4|                             OUT5 |10    23| OUT18               //
//       3|                             OUT6 |11    22| OUT17               //
//       2|                             OUT7 |12    21| OUT16               //
//       1|                             OUT8 |13    20| OUT15               //
//       0|                             OUT9 |14    19| OUT14               //
// -------                             OUT10 |15    18| OUT13               //
//                                     OUT11 |16    17| OUT12               //
//                                           |________|                     //
//                                                                          //
//  -  Put the longer leg (anode) of the LEDs in VCC and the shorter leg    //
//          (cathode) in OUT(0-23).                                         //
//                                                                          //
//  -  +5V from Arduino -> TLC pin 32   (VCC)                               //
//  -  GND from Arduino -> TLC pin 1    (GND)                               //
//  -  digital 9        -> TLC pin 30   (XLAT)                              //
//  -  digital 10       -> TLC pin 2    (BLANK)                             //
//  -  digital 11       -> TLC pin 4    (SIN)                               //
//  -  digital 13       -> TLC pin 3    (SCLK)                              //
//                                                                          //
//  -  The 1k resistor between TLC pin 31 and GND will let ~30mA through    //
//      each LED. This is calculated by the equation I = 49.2/R. This       //
//      doesn't depend on the LED driving voltage.                          //
//  - (Optional): put a pull-up resistor (~10k) between BLANK and VCC so    //
//      that all the LEDs will turn off when the Arduino is reset.          //
//                                                                          //
//  If you are daisy-chaining more than one TLC5947, connect the SOUT of    //
//  the first TLC to the SIN of the next. All the other pins should just be //
//  connected together. The one exception is that each TLC needs it's own   //
//  resistor between pin 31 and GND.                                        //
//                                                                          //
//  This library uses pins 9, 10, 11, and 13.                               //
//  Please do not use these pins.                                           //
//                                                                          //
//  This sketch does the Knight Rider strobe across a line of LEDs.         //
//                                                                          //
//  Jordi Orlando, 03/14/14                                                 //
//  For documentation, please visit hexa.io                                 //
// ======================================================================== //

#include <TLC5947.h> // Include statement for the TLC5947 library

TLC5947 TLC; // Declare a new TLC5947 instance (repeat as necessary)

// Create an array to store the LED values
uint16_t *pnArray = new uint16_t[24 * TLC5947::numChips()];

#define SPEED	1 // Change this to any value between 1 and 1000



void setup(){} // We don't need anything to go in setup()

void loop() {
  int8_t nDirection = 1; // Define the direction variable

  // Loop through every LED attached to the TLC5947(s)
  for (uint8_t i = 0; i < 24 * TLC5947::numChips(); i += nDirection) {
    uint32_t nTimer = millis(); // Record the current time
    pnArray[i] = 4095; // Set the current LED to full-on (4095)

    // Fade the correct LEDs by calling updateArray()
    while (millis() < nTimer + (500 / SPEED)) {
      updateArray();
    }

    // If the strobe is at either end of the display, switch directions
    if (((i == 0) && (nDirection == -1)) || (
      (i == 24 * TLC5947::numChips() - 1) && (nDirection == 1))) {
      // Fade the correct LEDs by calling updateArray()
      while (pnArray[i] > 0) {
        updateArray();
      }

      i += nDirection; // Increment/decrement the counter
      nDirection = ~nDirection + 1; // Switch directions
    }
  }
}

void updateArray() {
  // Subtract SPEED from every value in the LED array
  for (uint8_t i = 0; i < 24 * TLC5947::numChips(); i++) {
    if (pnArray[i] >= SPEED) {
      pnArray[i] -= SPEED;
      TLC.set(i, pnArray[i]);
    } else {
      pnArray[i] = 0;
      TLC.set(i, pnArray[i]);
    }
  }

  TLC.update(); // Update the chips (actually send the data)
  delayMicroseconds(250); // Delay for stability
}
