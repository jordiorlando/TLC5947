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

// Declare TLC5947 class and its member functions
class TLC5947 {
  public:
    TLC5947(uint16_t nInitialValue = 0);
    ~TLC5947();

    uint8_t chipID(void);
    static uint8_t numChips(void);

    bool set(uint16_t nValue);
    bool set(uint8_t nChannel, uint16_t nValue);
    static bool set(uint8_t nChip, uint8_t nChannel, uint16_t nValue);
    static bool setAll(uint16_t nValue);
    static bool setAll(uint8_t nChip, uint16_t nValue);

    //void write(uint16_t anValues[24]);
    //static void write(uint8_t nChip, uint16_t anValues[24]);

    uint16_t read(uint8_t nChannel);
    static uint16_t read(uint8_t nChip, uint8_t nChannel);

    void clear(void);
    static bool clear(uint8_t nChip);
    static void clearAll(void);

    static bool shift(uint16_t nShift = 1, uint16_t nValue = 0xFFFF);

    static void update(void);

  private:
    static void embiggen(void);
    static uint8_t s_nNumChips;
    static uint16_t **s_pnValues;
    static uint16_t **s_pnValuesTemp;
    uint8_t m_nChip;
};



#endif
