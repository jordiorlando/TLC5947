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
//  This sketch does                                                        //
//                                                                          //
//  Jordi Orlando, 03/14/14                                                 //
//  For documentation, please visit hexa.io                                 //
// ======================================================================== //

#include <TLC5947.h>

TLC5947 TLC;

#define INPUT_PIN	A0
#define CLEAR_PIN	13

#define SPEED		50



void setup() {
  pinMode(INPUT_PIN, INPUT);
  pinMode(CLEAR_PIN, INPUT);
  digitalWrite(CLEAR_PIN, HIGH);
}

void loop() {
  uint16_t nSum = TLC5947::read(TLC5947::numChips() - 1, 23) + (analogRead(INPUT_PIN) << 2);

  if (!digitalRead(CLEAR_PIN) || nSum > 4095) {
    nSum = 0;
  }

  TLC5947::shift(1, nSum);

  delay(SPEED);
}
