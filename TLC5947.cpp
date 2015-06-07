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

#include "Arduino.h"
#include "TLC5947.h"

// ------ Pin definitions from "Pin Mappings.txt" ------ //
#define SINPORT		PORTB
#define SCKPORT		PORTB
#define XLATPORT	PORTB
#define BLANKPORT	PORTB

#define SINDDR		DDRB
#define SCKDDR		DDRB
#define XLATDDR		DDRB
#define BLANKDDR	DDRB

// The internal microcontroller label, not the printed Arduino pin number
#define SINPIN		3
#define SCKPIN		4
#define XLATPIN		1
#define BLANKPIN	2

// These are the pre-shifted pin definitions for the optimized code
#define SINPINSHIFT		8
#define SCKPINSHIFT		16
#define XLATPINSHIFT	2
#define BLANKPINSHIFT	4

// Static variable definitions
uint8_t TLC5947::s_nNumChips = 0;
uint16_t** TLC5947::s_pnValues;
uint16_t** TLC5947::s_pnValuesTemp;



TLC5947::TLC5947(uint16_t nInitialValue) {
  // Set all necessary pins to outputs
  SCKDDR |= SCKPINSHIFT;
  BLANKDDR |= (1 << BLANKPIN);
  XLATDDR |= (1 << XLATPIN);
  SINDDR |= (1 << SINPIN);

  BLANKPORT |= 1 << BLANKPIN; // Turn all the outputs off (BLANK pin)
  XLATPORT &= ~(1 << XLATPIN); // Ensure that the Latch Pin is off

  m_nChip = s_nNumChips; // Set current ID based on number of total chips

  embiggen(); // Embiggen the data array

  for (uint8_t i = 0; i < 24; i++) {
    s_pnValues[m_nChip][i] = nInitialValue;
  }

  if (s_nNumChips == 1) {
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2); // Run at 8MHz on 16MHz boards
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0)
  }

  update();
}

TLC5947::~TLC5947() {
  if (s_nNumChips == 1) {
    SPI.end();
  }

  s_nNumChips--;
}

void TLC5947::embiggen(void) {
  if (s_nNumChips) {
    // Allocate a dynamic multidimensional array
    s_pnValuesTemp = new uint16_t*[s_nNumChips + 1];
    for (uint8_t i = 0; i < s_nNumChips + 1; i++) {
      s_pnValuesTemp[i] = new uint16_t[24];
    }

    // Copy the array to the new temporary array
    for (uint8_t i = 0; i < s_nNumChips; i++) {
      for (uint8_t ii = 0; ii < 24; ii++) {
        s_pnValuesTemp[i][ii] = s_pnValues[i][ii];
      }
    }

    // Delete the dynamic multidimensional array
    for (uint8_t i = 0; i < s_nNumChips; i++) {
      delete[] s_pnValues[i];
    }
    delete[] s_pnValues;

    s_pnValues = s_pnValuesTemp; // Point the array at the new array
    s_pnValuesTemp = 0; // Nullify the temporary pointer
  } else {
    // Allocate a dynamic multidimensional array
    s_pnValues = new uint16_t*[s_nNumChips + 1];
    for (uint8_t i = 0; i < s_nNumChips + 1; i++) {
      s_pnValues[i] = new uint16_t[24];
    }
  }

  s_nNumChips++;
}

uint8_t TLC5947::chipID(void) {
  return m_nChip;
}

uint8_t TLC5947::numChips(void) {
  return s_nNumChips;
}

bool TLC5947::set(uint16_t nValue) {
  if (!(nValue & 0xF000)) {
    // Set all values to nValue
    for (uint8_t i = 0; i < 24; i++) {
      s_pnValues[m_nChip][i] = nValue;
    }

    return 0;
  } else {
    return 1;
  }
}

bool TLC5947::set(uint8_t nChannel, uint16_t nValue) {
  if (nChannel < 24 && !(nValue & 0xF000)) {
    s_pnValues[m_nChip][nChannel] = nValue;

    return 0;
  } else {
    return 1;
  }
}

bool TLC5947::set(uint8_t nChip, uint8_t nChannel, uint16_t nValue) {
  if (nChip < s_nNumChips && nChannel < 24 && !(nValue & 0xF000)) {
    s_pnValues[nChip][nChannel] = nValue;

    return 0;
  } else {
    return 1;
  }
}

bool TLC5947::setAll(uint16_t nValue) {
  if (!(nValue & 0xF000)) {
    for (uint8_t i = 0; i < s_nNumChips; i++) {
      for (uint8_t ii = 0; ii < 24; ii++) {
        s_pnValues[i][ii] = nValue;
      }
    }

    return 0;
  } else {
    return 1;
  }
}

bool TLC5947::setAll(uint8_t nChip, uint16_t nValue) {
  if (nChip < s_nNumChips && !(nValue & 0xF000)) {
    // Set all values to nValue
    for (uint8_t i = 0; i < 24; i++) {
      s_pnValues[nChip][i] = nValue;
    }

    return 0;
  } else {
    return 1;
  }
}

uint16_t TLC5947::read(uint8_t nChannel) {
  if (nChannel < 24) {
    return s_pnValues[m_nChip][nChannel]; // Return the given channel
  } else {
    return 0xFFFF;
  }
}

uint16_t TLC5947::read(uint8_t nChip, uint8_t nChannel) {
  if (nChip < s_nNumChips && nChannel < 24) {
    return s_pnValues[nChip][nChannel]; // Return the given chip's channel
  } else {
    return 0xFFFF;
  }
}

void TLC5947::clear(void) {
  // Set all values to zero
  for (uint8_t i = 0; i < 24; i++) {
    s_pnValues[m_nChip][i] = 0;
  }
}

bool TLC5947::clear(uint8_t nChip) {
  if (nChip < s_nNumChips) {
    // Set all values to zero
    for (uint8_t i = 0; i < 24; i++) {
      s_pnValues[nChip][i] = 0;
    }

    return 0;
  } else {
    return 1;
  }
}

void TLC5947::clearAll(void) {
  // Set all chips to zero
  for (uint8_t i = 0; i < s_nNumChips; i++) {
    for (uint8_t ii = 0; ii < 24; ii++) {
      s_pnValues[i][ii] = 0;
    }
  }
}

bool TLC5947::shift(uint16_t nShift, uint16_t nValue) {
  bool bCircular = false;
  if (nValue == 0xFFFF) {
    bCircular = true;
    nValue = TLC5947::read(TLC5947::numChips() - 1, 23);
  }

  if (!(nValue & 0xF000)) {
    if (bCircular()) {

    } else {
      for (uint16_t i = (s_nNumChips * 24) - 1; i >= 0; i--) {
        if (i >= nShift) {
          s_pnValues[(i - (i % 24)) / 24][i % 24] = s_pnValues[
            (i - nShift - ((i - nShift) % 24)) / 24][
            (i - nShift) % 24];
        } else {
          s_pnValues[(i - (i % 24)) / 24][i % 24] = nShift;
        }
      }
    }

    for (uint16_t i = 0; i < nShift; i += 2) {
      if (i == (nShift - 1)) {
        SPI.transfer((uint8_t)((nValue >> 4) & 0x00FF));

        // TODO: figure out how to transfer the last nibble (4 bits)

        /*
        // Send data MSB first
        for (uint8_t ii = 0; ii < 12; ii++) {
          // Send the current bit
          if ((nValue << ii) & 0x0800) {
            SINPORT |= (1 << SINPIN);
          } else {
            SINPORT &= ~(1 << SINPIN);
          }

          // Clock in the current bit (serial clock) [rising edge]
          SCKPORT |= (1 << SCKPIN);
          SCKPORT &= ~(1 << SCKPIN);
        }
        */
      } else {
        // Send the bytes
        SPI.transfer((uint8_t)((nValue >> 4) & 0x00FF));
        SPI.transfer((uint8_t)((nValue << 4) & 0x00F0) | (uint8_t)((nValue >> 8) & 0x000F));
        SPI.transfer((uint8_t)(nValue & 0x00FF));
      }
    }

    return 0;
  } else {
    return 1;
  }
}

void TLC5947::update(void) {
  BLANKPORT |= (1 << BLANKPIN); // Turn all the outputs off (BLANK pin)

  // Send the highest/furthest chip's data first
  for (int8_t i = s_nNumChips - 1; i >= 0; i--) {
    // This chunk of code is the much prettier(read: actually good), 	//
    // but significantly slower implementation.							//

    /*
    // Loop the 24 channels per chip
    for (uint8_t ii = 0; ii < 24; ii++) {
      uint16_t nTemp = s_pnValues[i][23 - ii]; // Current value

      // Send data MSB first
      for (uint8_t iii = 0; iii < 12; iii++) {
        // Send the current bit
        if ((nTemp << iii) & 0x0800) {
          SINPORT |= SINPINSHIFT;
        } else {
          SINPORT &= ~SINPINSHIFT;
        }

        // Clock in the current bit (serial clock) [rising edge]
        SCKPORT |= SCKPINSHIFT;
        SCKPORT &= ~SCKPINSHIFT;
      }
    }
    */



    // The following clusterfuck of code is the actual code to send the //
    // data. It's an unrolled 'for' loop. I've found that doing it this //
    // way (on an Arduino) reduces latency by 75%. That being said, 	//
    // it also adds 4KB to the program size.							//

    uint16_t nTemp = s_pnValues[i][23];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][22];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][21];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][20];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][19];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][18];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][17];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][16];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][15];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][14];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][13];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][12];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][11];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][10];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][9];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][8];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][7];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][6];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][5];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][4];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][3];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][2];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][1];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;



    nTemp = s_pnValues[i][0];

    if (nTemp & 0x0800)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0400)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0200)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0100)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0080)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0040)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0020)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0010)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0008)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0004)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0002)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;

    if (nTemp & 0x0001)
      SINPORT |= SINPINSHIFT;
    else
      SINPORT &= ~SINPINSHIFT;

    SCKPORT |= SCKPINSHIFT;
    SCKPORT &= ~SCKPINSHIFT;
  }

  // Latch the data (send it to the outputs) [rising edge]
  XLATPORT |= (1 << XLATPIN);
  XLATPORT &= ~(1 << XLATPIN);

  BLANKPORT &= ~(1 << BLANKPIN); // Enable the outputs again (BLANK pin)
}
