/*
Copyright 2015 Jordi Pakey-Rodriguez <jordi.orlando@hexa.io>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TLC5947_H
#define TLC5947_H

#include <avr/io.h>
#include "pindefs.h"
#include "new.h"

// Declare TLC5947 class and its member functions
class TLC5947 {
  public:
    TLC5947();
    TLC5947(pin latch, pin blank);
    ~TLC5947();

    uint8_t chipID(void);
    static uint8_t numChips(void);

    uint16_t read(uint8_t channel);

    void set(uint16_t values[24]);
    void set(uint16_t value);
    void set(uint8_t channel, uint16_t value);
    static void setAll(uint16_t value);

    void clear(void);
    static void clearAll(void);

    static void enableSPI(void);
    static void disableSPI(void);

    void enable(void);
    void disable(void);
    void latch(void);

    static void send(void);
    static void update(void);

    static void shift(uint16_t shift = 1, uint16_t value = 0xFFFF);

  private:
    static void embiggen(void);

    static void enable(uint8_t chip);
    static void disable(uint8_t chip);
    static void latch(uint8_t chip);

    static const pin s_SCK;
    static const pin s_MOSI;
    static pin *s_latch;
    static pin *s_blank;

    static bool s_modified;
    static bool s_SPIenabled;

    static uint8_t s_numChips;
    static uint16_t **s_values;

    uint8_t m_chip;
};

#endif
