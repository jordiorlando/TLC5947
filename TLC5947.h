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

#define PA0 (pin){DDA0, PORTA, DDRA}
#define PA1 (pin){DDA1, PORTA, DDRA}
#define PA2 (pin){DDA2, PORTA, DDRA}
#define PA3 (pin){DDA3, PORTA, DDRA}
#define PA4 (pin){DDA4, PORTA, DDRA}
#define PA5 (pin){DDA5, PORTA, DDRA}
#define PA6 (pin){DDA6, PORTA, DDRA}
#define PA7 (pin){DDA7, PORTA, DDRA}

#define PB0 (pin){DDB0, PORTB, DDRB}
#define PB1 (pin){DDB1, PORTB, DDRB}
#define PB2 (pin){DDB2, PORTB, DDRB}
#define PB3 (pin){DDB3, PORTB, DDRB}
#define PB4 (pin){DDB4, PORTB, DDRB}
#define PB5 (pin){DDB5, PORTB, DDRB}
#define PB6 (pin){DDB6, PORTB, DDRB}
#define PB7 (pin){DDB7, PORTB, DDRB}

#define PC0 (pin){DDC0, PORTC, DDRC}
#define PC1 (pin){DDC1, PORTC, DDRC}
#define PC2 (pin){DDC2, PORTC, DDRC}
#define PC3 (pin){DDC3, PORTC, DDRC}
#define PC4 (pin){DDC4, PORTC, DDRC}
#define PC5 (pin){DDC5, PORTC, DDRC}
#define PC6 (pin){DDC6, PORTC, DDRC}
#define PC7 (pin){DDC7, PORTC, DDRC}

#define PD0 (pin){DDD0, PORTD, DDRD}
#define PD1 (pin){DDD1, PORTD, DDRD}
#define PD2 (pin){DDD2, PORTD, DDRD}
#define PD3 (pin){DDD3, PORTD, DDRD}
#define PD4 (pin){DDD4, PORTD, DDRD}
#define PD5 (pin){DDD5, PORTD, DDRD}
#define PD6 (pin){DDD6, PORTD, DDRD}
#define PD7 (pin){DDD7, PORTD, DDRD}

#define PE0 (pin){DDE0, PORTE, DDRE}
#define PE1 (pin){DDE1, PORTE, DDRE}
#define PE2 (pin){DDE2, PORTE, DDRE}
#define PE3 (pin){DDE3, PORTE, DDRE}
#define PE4 (pin){DDE4, PORTE, DDRE}
#define PE5 (pin){DDE5, PORTE, DDRE}
#define PE6 (pin){DDE6, PORTE, DDRE}
#define PE7 (pin){DDE7, PORTE, DDRE}

#define PF0 (pin){DDF0, PORTF, DDRF}
#define PF1 (pin){DDF1, PORTF, DDRF}
#define PF2 (pin){DDF2, PORTF, DDRF}
#define PF3 (pin){DDF3, PORTF, DDRF}
#define PF4 (pin){DDF4, PORTF, DDRF}
#define PF5 (pin){DDF5, PORTF, DDRF}
#define PF6 (pin){DDF6, PORTF, DDRF}
#define PF7 (pin){DDF7, PORTF, DDRF}

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

struct pin {
  uint8_t pin;
  uint8_t port;
  uint8_t ddr;
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
    static void embiggen(void);

    static pin *s_pnLatch;
    static pin *s_pnBlank;

    static uint8_t s_nNumChips;
    static uint16_t **s_pnValues;

    static bool s_bModified;
    static bool s_bSPIenabled;

    uint8_t m_nChip;
};

#endif
