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

// Pin definitions
#if defined (__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
  // PB1
  #define XLATPORT  PORTB
  #define XLATDDR   DDRB
  #define XLATPIN   DDB1
  // SS
  #define BLANKPORT PORTB
  #define BLANKDDR  DDRB
  #define BLANKPIN  DDB2
  // SCK
  #define SCKPORT   PORTB
  #define SCKDDR    DDRB
  #define SCKPIN    DDB5
  // MOSI
  #define SINPORT   PORTB
  #define SINDDR    DDRB
  #define SINPIN    DDB3
#elif defined (__AVR_ATmega16U4__) || defined (__AVR_ATmega32U4__)
  // PB7
  #define XLATPORT  PORTB
  #define XLATDDR   DDRB
  #define XLATPIN   DDB7 // TODO: change this to the correct pin for ATmega32U4
  // SS
  #define BLANKPORT PORTB
  #define BLANKDDR  DDRB
  #define BLANKPIN  DDB0
  // SCK
  #define SCKPORT   PORTB
  #define SCKDDR    DDRB
  #define SCKPIN    DDB1
  // MOSI
  #define SINPORT   PORTB
  #define SINDDR    DDRB
  #define SINPIN    DDB2
#elif defined (__AVR_ATmega2560__) // TODO: test this and support ATmega1280
  // PD7
  #define XLATPORT  PORTD
  #define XLATDDR   DDRD
  #define XLATPIN   DDD7
  // SS
  #define BLANKPORT PORTB
  #define BLANKDDR  DDRB
  #define BLANKPIN  DDB0
  // SCK
  #define SCKPORT   PORTB
  #define SCKDDR    DDRB
  #define SCKPIN    DDB1
  // MOSI
  #define SINPORT   PORTB
  #define SINDDR    DDRB
  #define SINPIN    DDB2
#endif

// Declare TLC5947 class and its member functions
class TLC5947 {
  public:
    TLC5947(uint16_t nInitialValue = 0);
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
    static void enable(void);
    static void disable(void);
    static void send(void);
    static void latch(void);
    static void update(void);

  private:
    static void embiggen(void);

    static uint8_t s_nNumChips;
    static uint16_t **s_pnValues;
    static uint16_t **s_pnValuesTemp;

    static bool s_bModified;
    static bool s_bSPIenabled;

    uint8_t m_nChip;
};



#endif
