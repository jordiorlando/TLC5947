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

#define PA0 0x00
#define PA1 0x01
#define PA2 0x02
#define PA3 0x03
#define PA4 0x04
#define PA5 0x05
#define PA6 0x06
#define PA7 0x07

#define PB0 0x08
#define PB1 0x09
#define PB2 0x0A
#define PB3 0x0B
#define PB4 0x0C
#define PB5 0x0D
#define PB6 0x0E
#define PB7 0x0F

#define PC0 0x10
#define PC1 0x11
#define PC2 0x12
#define PC3 0x13
#define PC4 0x14
#define PC5 0x15
#define PC6 0x16
#define PC7 0x17

#define PD0 0x18
#define PD1 0x19
#define PD2 0x1A
#define PD3 0x1B
#define PD4 0x1C
#define PD5 0x1D
#define PD6 0x1E
#define PD7 0x1F

#define PE0 0x20
#define PE1 0x21
#define PE2 0x22
#define PE3 0x23
#define PE4 0x24
#define PE5 0x25
#define PE6 0x26
#define PE7 0x27

#define PF0 0x28
#define PF1 0x29
#define PF2 0x2A
#define PF3 0x2B
#define PF4 0x2C
#define PF5 0x2D
#define PF6 0x2E
#define PF7 0x2F

// Pin definitions
#if defined (__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
  // SCK
  #define SCKPIN    DDB5
  #define SCKPORT   PORTB
  #define SCKDDR    DDRB
  // MOSI
  #define SINPIN    DDB3
  #define SINPORT   PORTB
  #define SINDDR    DDRB
#elif defined (__AVR_ATmega16U4__) || defined (__AVR_ATmega32U4__)
  // SCK
  #define SCKPIN    DDB1
  #define SCKPORT   PORTB
  #define SCKDDR    DDRB
  // MOSI
  #define SINPIN    DDB2
  #define SINPORT   PORTB
  #define SINDDR    DDRB
#elif defined (__AVR_ATmega2560__) // TODO: test this and support ATmega1280
  // SCK
  #define SCKPIN    DDB1
  #define SCKPORT   PORTB
  #define SCKDDR    DDRB
  // MOSI
  #define SINPIN    DDB2
  #define SINPORT   PORTB
  #define SINDDR    DDRB
#endif

// Declare TLC5947 class and its member functions
class TLC5947 {
  public:
    TLC5947();
    TLC5947(uint8_t nLatch, uint8_t nBlank);
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

    static void shift(uint16_t nShift = 1, uint16_t nValue = 0xFFFF);

    static void enableSPI(void);
    static void disableSPI(void);
    void enable(void);
    static void enable(uint8_t nChip);
    void disable(void);
    static void disable(uint8_t nChip);
    static void send(void);
    void latch(void);
    static void latch(uint8_t nChip);
    static void update(void);

  private:
    static void ddr(uint8_t nPin, bool bState);
    static void out(uint8_t nPin, bool bState);
    
    static void embiggen(void);

    static uint8_t *s_pnLatch;
    static uint8_t *s_pnBlank;

    static uint8_t s_nNumChips;
    static uint16_t **s_pnValues;

    static bool s_bModified;
    static bool s_bSPIenabled;

    uint8_t m_nChip;
};

#endif
