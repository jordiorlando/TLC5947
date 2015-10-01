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

#ifndef PINDEFS_H
#define PINDEFS_H

struct pin {
  uint8_t pin;
  volatile uint8_t *port;
  volatile uint8_t *ddr;
};

#define PA0 {0, &PORTA, &DDRA}
#define PA1 {1, &PORTA, &DDRA}
#define PA2 {2, &PORTA, &DDRA}
#define PA3 {3, &PORTA, &DDRA}
#define PA4 {4, &PORTA, &DDRA}
#define PA5 {5, &PORTA, &DDRA}
#define PA6 {6, &PORTA, &DDRA}
#define PA7 {7, &PORTA, &DDRA}

#define PB0 {0, &PORTB, &DDRB}
#define PB1 {1, &PORTB, &DDRB}
#define PB2 {2, &PORTB, &DDRB}
#define PB3 {3, &PORTB, &DDRB}
#define PB4 {4, &PORTB, &DDRB}
#define PB5 {5, &PORTB, &DDRB}
#define PB6 {6, &PORTB, &DDRB}
#define PB7 {7, &PORTB, &DDRB}

#define PC0 {0, &PORTC, &DDRC}
#define PC1 {1, &PORTC, &DDRC}
#define PC2 {2, &PORTC, &DDRC}
#define PC3 {3, &PORTC, &DDRC}
#define PC4 {4, &PORTC, &DDRC}
#define PC5 {5, &PORTC, &DDRC}
#define PC6 {6, &PORTC, &DDRC}
#define PC7 {7, &PORTC, &DDRC}

#define PD0 {0, &PORTD, &DDRD}
#define PD1 {1, &PORTD, &DDRD}
#define PD2 {2, &PORTD, &DDRD}
#define PD3 {3, &PORTD, &DDRD}
#define PD4 {4, &PORTD, &DDRD}
#define PD5 {5, &PORTD, &DDRD}
#define PD6 {6, &PORTD, &DDRD}
#define PD7 {7, &PORTD, &DDRD}

#define PE0 {0, &PORTE, &DDRE}
#define PE1 {1, &PORTE, &DDRE}
#define PE2 {2, &PORTE, &DDRE}
#define PE3 {3, &PORTE, &DDRE}
#define PE4 {4, &PORTE, &DDRE}
#define PE5 {5, &PORTE, &DDRE}
#define PE6 {6, &PORTE, &DDRE}
#define PE7 {7, &PORTE, &DDRE}

#define PF0 {0, &PORTF, &DDRF}
#define PF1 {1, &PORTF, &DDRF}
#define PF2 {2, &PORTF, &DDRF}
#define PF3 {3, &PORTF, &DDRF}
#define PF4 {4, &PORTF, &DDRF}
#define PF5 {5, &PORTF, &DDRF}
#define PF6 {6, &PORTF, &DDRF}
#define PF7 {7, &PORTF, &DDRF}

#define PG0 {0, &PORTG, &DDRG}
#define PG1 {1, &PORTG, &DDRG}
#define PG2 {2, &PORTG, &DDRG}
#define PG3 {3, &PORTG, &DDRG}
#define PG4 {4, &PORTG, &DDRG}
#define PG5 {5, &PORTG, &DDRG}
#define PG6 {6, &PORTG, &DDRG}
#define PG7 {7, &PORTG, &DDRG}

#define PH0 {0, &PORTH, &DDRH}
#define PH1 {1, &PORTH, &DDRH}
#define PH2 {2, &PORTH, &DDRH}
#define PH3 {3, &PORTH, &DDRH}
#define PH4 {4, &PORTH, &DDRH}
#define PH5 {5, &PORTH, &DDRH}
#define PH6 {6, &PORTH, &DDRH}
#define PH7 {7, &PORTH, &DDRH}

#define PJ0 {0, &PORTJ, &DDRJ}
#define PJ1 {1, &PORTJ, &DDRJ}
#define PJ2 {2, &PORTJ, &DDRJ}
#define PJ3 {3, &PORTJ, &DDRJ}
#define PJ4 {4, &PORTJ, &DDRJ}
#define PJ5 {5, &PORTJ, &DDRJ}
#define PJ6 {6, &PORTJ, &DDRJ}
#define PJ7 {7, &PORTJ, &DDRJ}

#define PK0 {0, &PORTK, &DDRK}
#define PK1 {1, &PORTK, &DDRK}
#define PK2 {2, &PORTK, &DDRK}
#define PK3 {3, &PORTK, &DDRK}
#define PK4 {4, &PORTK, &DDRK}
#define PK5 {5, &PORTK, &DDRK}
#define PK6 {6, &PORTK, &DDRK}
#define PK7 {7, &PORTK, &DDRK}

#define PL0 {0, &PORTL, &DDRL}
#define PL1 {1, &PORTL, &DDRL}
#define PL2 {2, &PORTL, &DDRL}
#define PL3 {3, &PORTL, &DDRL}
#define PL4 {4, &PORTL, &DDRL}
#define PL5 {5, &PORTL, &DDRL}
#define PL6 {6, &PORTL, &DDRL}
#define PL7 {7, &PORTL, &DDRL}

// SPI definitions
#if defined (__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
#  define SPI_SCK   {5, &PORTB, &DDRB}
#  define SPI_MOSI  {3, &PORTB, &DDRB}
#elif defined (__AVR_ATmega16U4__) || defined (__AVR_ATmega32U4__)
#  define SPI_SCK   {1, &PORTB, &DDRB}
#  define SPI_MOSI  {2, &PORTB, &DDRB}
#elif defined (__AVR_ATmega2560__) // TODO: test this and support ATmega1280
#  define SPI_SCK   {1, &PORTB, &DDRB}
#  define SPI_MOSI  {2, &PORTB, &DDRB}
#endif

#endif
