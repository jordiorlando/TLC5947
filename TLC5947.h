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

struct pin {
  uint8_t pin;
  volatile uint8_t *port;
  volatile uint8_t *ddr;
};

// Declare TLC5947 class and its member functions
class TLC5947 {
  public:
    TLC5947();
    TLC5947(pin nLatch, pin nBlank);
    ~TLC5947();

    uint8_t chipID(void);
    static uint8_t numChips(void);

    uint16_t read(uint8_t nChannel);

    void set(uint16_t anValues[24]);
    void set(uint16_t nValue);
    void set(uint8_t nChannel, uint16_t nValue);
    static void setAll(uint16_t nValue);

    void clear(void);
    static void clearAll(void);

    static void enableSPI(void);
    static void disableSPI(void);

    void enable(void);
    static void enable(uint8_t nChip);
    void disable(void);
    static void disable(uint8_t nChip);
    void latch(void);
    static void latch(uint8_t nChip);

    static void send(void);
    static void update(void);

    static void shift(uint16_t nShift = 1, uint16_t nValue = 0xFFFF);

  private:
    static void embiggen(void);

    static const pin s_nSCK;
    static const pin s_nMOSI;
    static pin *s_pnLatch;
    static pin *s_pnBlank;

    static uint8_t s_nNumChips;
    static uint16_t **s_pnValues;

    static bool s_bModified;
    static bool s_bSPIenabled;

    uint8_t m_nChip;
};

#endif
