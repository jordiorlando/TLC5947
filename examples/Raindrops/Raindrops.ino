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
//  This sketch simulates a random raindrop effect on a group of RGB LEDs.  //
//                                                                          //
//  Jordi Pakey-Rodriguez, 2015-06-10                                       //
//  For documentation, please visit https://github.com/D1SC0tech/TLC5947    //
// ======================================================================== //

#include <TLC5947.h> // Include statement for the TLC5947 library
#include <avr/pgmspace.h> // Include the pgmspace library for PROGMEM support

TLC5947 TLC; // Declare a new TLC5947 instance (repeat as necessary)

// Wolfram Alpha:
// Table[floor(Re(ln(x))*255/Re(ln(-0.015625))), {x, -1, -0.015625, 0.015625}]
static const uint8_t raindropTable[64] PROGMEM = {
  0, 0, 1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16,
  17, 18, 20, 21, 22, 24, 25, 27, 28, 30, 31, 33, 35, 37, 38, 40,
  42, 44, 46, 48, 50, 52, 55, 57, 60, 62, 65, 68, 71, 74, 77, 81,
  85, 88, 93, 97, 102, 107, 113, 120, 127, 135, 145, 156, 170, 187, 212, 255
};
/*// Table[floor(-(1/x+1) * 255/63), {x, -1, -0.015625, 0.015625}]
static const uint8_t raindropTable[64] PROGMEM = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3,
  4, 4, 4, 4, 5, 5, 5, 6, 6, 7, 7, 8, 8, 9, 10, 11,
  12, 13, 14, 15, 17, 19, 21, 24, 28, 32, 39, 47, 60, 82, 125, 255
};
// Table[floor(-(1/x+1) * 3145/21), {x, -1, -0.37, 0.01}]
static const uint8_t raindropTable[64] PROGMEM = {
  0, 1, 3, 4, 6, 7, 9, 11, 13, 14, 16, 18, 20, 22, 24, 26,
  28, 30, 32, 35, 37, 39, 42, 44, 47, 49, 52, 55, 58, 61, 64, 67,
  70, 73, 77, 80, 84, 87, 91, 95, 99, 104, 108, 112, 117, 122, 127, 132,
  138, 143, 149, 155, 162, 168, 175, 183, 190, 198, 206, 215, 224, 234, 244, 254
};*/

// Create an array to track the frames for each LED
uint8_t *pnArray = new uint8_t[TLC5947::numChips() * 8];

// Frame counter
uint8_t nCounter = 0;
// Brightness value (1 to 16)
#define BRIGHTNESS  16
// Change this to any value between 1 and 1000
#define SPEED       2



void setup() {
  // Initialize our frame array
  for (uint8_t i = 0; i < TLC5947::numChips() * 8; i++) {
    pnArray[i] = 255;
  }
}

void loop() {
  for (uint8_t i = 0; i < TLC5947::numChips() * 8; i++) {
    if (pnArray[i] < 255) {
      // Set green
      TLC.set(i * 3, (uint16_t)pgm_read_byte_near(raindropTable + pnArray[i]) * BRIGHTNESS);
      // Set red
      TLC.set((i * 3) + 1, (uint16_t)pgm_read_byte_near(raindropTable + pnArray[i]) * BRIGHTNESS);
      // Set blue
      TLC.set((i * 3) + 2, (uint16_t)pgm_read_byte_near(raindropTable + pnArray[i]) * BRIGHTNESS);
      pnArray[i]--;
    }
  }

  // Send the data to the chips
  TLC5947::update();

  delay(SPEED);
  nCounter++;

  // Wait a random amount of time between raindrops
  if (nCounter > random(10, 10000)) {
    raindrop();
    nCounter = 0;
  }
}

void raindrop() {
  // Pick a random channel that is currently empty
  while (true) {
    uint8_t i = random(TLC5947::numChips() * 8);

    if (pnArray[i] > 63) {
      pnArray[i] = 63;
      break;
    }
  }
}
