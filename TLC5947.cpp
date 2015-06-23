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

#include "TLC5947.h"

// Static variable definitions
const pin TLC5947::s_nSCK = SPI_SCK;
const pin TLC5947::s_nMOSI = SPI_MOSI;
pin* TLC5947::s_pnLatch;
pin* TLC5947::s_pnBlank;
uint8_t TLC5947::s_nNumChips = 0;
uint16_t** TLC5947::s_pnValues;
bool TLC5947::s_bModified = true;
bool TLC5947::s_bSPIenabled = false;

TLC5947::TLC5947() : TLC5947(s_pnLatch[0], s_pnBlank[0]) {
  // TODO: warn the user if they don't initialize the first chip
  //#if (s_nNumChips == 0)
  //#  error "You must define pins for the first chip"
  //#endif
}

TLC5947::TLC5947(pin nLatch, pin nBlank) {
  // Set current ID based on number of total chips
  m_nChip = s_nNumChips;
  // Embiggen the data array
  embiggen();

  s_pnLatch[m_nChip] = nLatch;
  s_pnBlank[m_nChip] = nBlank;

  // Set latch and blank (SS) to outputs
  *s_pnLatch[m_nChip].ddr |= _BV(s_pnLatch[m_nChip].pin);
  *s_pnBlank[m_nChip].ddr |= _BV(s_pnBlank[m_nChip].pin);

  // Set the BLANK pin high
  disable();
  // Ensure that the Latch Pin is off
  *s_pnLatch[m_nChip].port &= ~(_BV(s_pnLatch[m_nChip].pin));

  if (!m_nChip) {
    // Enable the SPI interface
    enableSPI();

    // Send a blank byte so that the SPIF bit is set
    SPDR = 0;
  }

  // Set all channels to start at 0
  clear();
  update();
}

TLC5947::~TLC5947() {
  // TODO: properly shrink the arrays
  if (!m_nChip) {
    // Disable the SPI interface
    disableSPI();
  }
}

void TLC5947::embiggen(void) {
  if (s_nNumChips) {
    // TODO: add a buffer in here so that we don't waste any space in RAM
    // Allocate a dynamic multidimensional array
    uint16_t **s_pnValuesTemp = new uint16_t*[s_nNumChips + 1];
    for (uint8_t i = 0; i < s_nNumChips + 1; i++) {
      s_pnValuesTemp[i] = new uint16_t[24];
    }
    pin *s_pnLatchTemp = new pin[s_nNumChips + 1];
    pin *s_pnBlankTemp = new pin[s_nNumChips + 1];

    // Copy the array to the new temporary array
    for (uint8_t i = 0; i < s_nNumChips; i++) {
      for (uint8_t ii = 0; ii < 24; ii++) {
        s_pnValuesTemp[i][ii] = s_pnValues[i][ii];
      }

      s_pnLatchTemp[i] = s_pnLatch[i];
      s_pnBlankTemp[i] = s_pnBlank[i];
    }

    // Delete the dynamic multidimensional array
    for (uint8_t i = 0; i < s_nNumChips; i++) {
      delete[] s_pnValues[i];
    }
    delete[] s_pnValues;
    delete[] s_pnLatch;
    delete[] s_pnBlank;

    // Point the array at the new array location
    s_pnValues = s_pnValuesTemp;
    s_pnLatch = s_pnLatchTemp;
    s_pnBlank = s_pnBlankTemp;
    // Nullify the temporary pointer
    s_pnValuesTemp = 0;
    s_pnLatchTemp = 0;
    s_pnBlankTemp = 0;
  } else {
    // Allocate a dynamic multidimensional array
    s_pnValues = new uint16_t*[1];
    for (uint8_t i = 0; i < s_nNumChips + 1; i++) {
      s_pnValues[i] = new uint16_t[24];
    }

    s_pnLatch = new pin;
    s_pnBlank = new pin;
  }

  s_nNumChips++;
}

uint8_t TLC5947::chipID(void) {
  return m_nChip;
}

uint8_t TLC5947::numChips(void) {
  return s_nNumChips;
}

uint16_t TLC5947::read(uint8_t nChannel) {
  // Return the given channel
  if (nChannel < 24) {
    return s_pnValues[m_nChip][nChannel];
  } else {
    return s_pnValues[(nChannel - (nChannel % 24)) / 24][nChannel % 24];
  }
}

void TLC5947::set(uint16_t anValues[24]) {
  for (uint8_t i = 0; i < 24; i++) {
    anValues[i] &= 0x0FFF;

    if (s_pnValues[m_nChip][i] != anValues[i]) {
      s_pnValues[m_nChip][i] = anValues[i];
      s_bModified = true;
    }
  }
}

void TLC5947::set(uint16_t nValue) {
  nValue &= 0x0FFF;

  // Set all values to nValue
  for (uint8_t i = 0; i < 24; i++) {
    if (s_pnValues[m_nChip][i] != nValue) {
      s_pnValues[m_nChip][i] = nValue;
      s_bModified = true;
    }
  }
}

void TLC5947::set(uint8_t nChannel, uint16_t nValue) {
  nValue &= 0x0FFF;

  uint8_t i;
  if (nChannel < 24) {
    i = m_nChip;
  } else {
    i = (nChannel - (nChannel % 24)) / 24;
  }

  if (s_pnValues[i][nChannel % 24] != nValue) {
    s_pnValues[i][nChannel % 24] = nValue;
    s_bModified = true;
  }
}

void TLC5947::setAll(uint16_t nValue) {
  nValue &= 0x0FFF;

  for (uint8_t i = 0; i < s_nNumChips; i++) {
    for (uint8_t ii = 0; ii < 24; ii++) {
      if (s_pnValues[i][ii] != nValue) {
        s_pnValues[i][ii] = nValue;
        s_bModified = true;
      }
    }
  }
}

void TLC5947::clear(void) {
  // Set all values to zero
  for (uint8_t i = 0; i < 24; i++) {
    if (s_pnValues[m_nChip][i]) {
      s_pnValues[m_nChip][i] = 0;
      s_bModified = true;
    }
  }
}

void TLC5947::clearAll(void) {
  // Set all chips to zero
  for (uint8_t i = 0; i < s_nNumChips; i++) {
    for (uint8_t ii = 0; ii < 24; ii++) {
      if (s_pnValues[i][ii]) {
        s_pnValues[i][ii] = 0;
        s_bModified = true;
      }
    }
  }
}

void TLC5947::enableSPI() {
  // Set MOSI and SCK as outputs
  *s_nSCK.ddr |= _BV(s_nSCK.pin);
  *s_nMOSI.ddr |= _BV(s_nMOSI.pin);

  // Enable SPI, master
  SPCR = (1<<SPE) | (1<<MSTR);

  // Set clock rate fck/2
  SPSR |= (1<<SPI2X);

  s_bSPIenabled = true;
}

void TLC5947::disableSPI() {
  // Disable SPI
  SPCR = 0;

  // Reset clock rate
  SPSR &= ~(1<<SPI2X);

  s_bSPIenabled = false;
}

void TLC5947::enable(void) {
  // Enable all outputs (BLANK low)
  *s_pnBlank[m_nChip].port &= ~(_BV(s_pnBlank[m_nChip].pin));
}

void TLC5947::enable(uint8_t nChip) {
  // Enable all outputs (BLANK low)
  *s_pnBlank[nChip].port &= ~(_BV(s_pnBlank[nChip].pin));
}

void TLC5947::disable(void) {
  // Disable all outputs (BLANK high)
  *s_pnBlank[m_nChip].port |= _BV(s_pnBlank[m_nChip].pin);
}

void TLC5947::disable(uint8_t nChip) {
  // Disable all outputs (BLANK high)
  *s_pnBlank[nChip].port |= _BV(s_pnBlank[nChip].pin);
}

void TLC5947::latch(void) {
  // Latch the data to the outputs (rising edge of XLAT)
  *s_pnLatch[m_nChip].port |= _BV(s_pnLatch[m_nChip].pin);
  *s_pnLatch[m_nChip].port &= ~(_BV(s_pnLatch[m_nChip].pin));
}

void TLC5947::latch(uint8_t nChip) {
  // Latch the data to the outputs (rising edge of XLAT)
  *s_pnLatch[nChip].port |= _BV(s_pnLatch[nChip].pin);
  *s_pnLatch[nChip].port &= ~(_BV(s_pnLatch[nChip].pin));
}

void TLC5947::send(void) {
  // Shift the data out to the chips
  for (int16_t i = (s_nNumChips * 24) - 1; i >= 0; i -= 2) {
    // Break every two channels into 3 bytes and send them
    while(!(SPSR & (1<<SPIF)));
    SPDR = (uint8_t)((s_pnValues[(i - (i % 24)) / 24][i % 24] >> 4) & 0x00FF);
    while(!(SPSR & (1<<SPIF)));
    SPDR = (uint8_t)((s_pnValues[(i - (i % 24)) / 24][i % 24] << 4) & 0x00F0) | (uint8_t)((s_pnValues[((i - 1) - ((i - 1) % 24)) / 24][(i - 1) % 24] >> 8) & 0x000F);
    while(!(SPSR & (1<<SPIF)));
    SPDR = (uint8_t)(s_pnValues[((i - 1) - ((i - 1) % 24)) / 24][(i - 1) % 24] & 0x00FF);
  }
}

void TLC5947::update(void) {
  // TODO: weed out duplicate calls to disable(), enable(), and latch()
  if (s_bModified) {
    // Enable SPI if it isn't already on
    if (!s_bSPIenabled) {
      enableSPI();
    }
    // Disable the outputs
    for (uint8_t i = 0; i < s_nNumChips; i++) {
      disable(i);
    }
    // Shift the data out to the chips
    send();
    // Latch the data to the outputs
    for (uint8_t i = 0; i < s_nNumChips; i++) {
      latch(i);
    }
    // Enable the outputs
    for (uint8_t i = 0; i < s_nNumChips; i++) {
      enable(i);
    }

    // Clear the modified flag
    s_bModified = false;
  }
}

// TODO: warn the user if they have modified anything before calling shift(). In
// this situation, the un-sent data will be lost.
void TLC5947::shift(uint16_t nShift, uint16_t nValue) {
  if (nShift >= (s_nNumChips * 24)) {
    nShift %= (s_nNumChips * 24);
  }

  // Initialize an array to store the overflow data
  uint16_t *p_nValues = new uint16_t[nShift];

  // Save the overflow data to the array
  for (int16_t i = (s_nNumChips * 24) - 1; i >= (s_nNumChips * 24) - nShift; i--) {
    if (nValue == 0xFFFF) {
      // If nValue is unchanged from the default, we are doing a circular
      // rotation. This means that no data should be lost.
      p_nValues[i + nShift - (s_nNumChips * 24)] = s_pnValues[(i - (i % 24)) / 24][i % 24];
    } else {
      // If not, set all the data to the same given value
      p_nValues[i + nShift - (s_nNumChips * 24)] = nValue;
    }
  }

  // Rotate the data by the required number of channels and update the array
  for (int16_t i = (s_nNumChips * 24) - 1; i >= 0; i--) {
    if (i >= nShift) {
      s_pnValues[(i - (i % 24)) / 24][i % 24] = s_pnValues[
        (i - nShift - ((i - nShift) % 24)) / 24][
        (i - nShift) % 24];
    } else {
      // Restore the overflow data
      s_pnValues[(i - (i % 24)) / 24][i % 24] = p_nValues[i];
    }
  }

  // Enable SPI if it isn't already on
  if (!s_bSPIenabled) {
    enableSPI();
  }

  // Disable the outputs
  for (uint8_t i = 0; i < s_nNumChips; i++) {
    disable(i);
  }

  // Actually shift the data out to the chips
  for (int16_t i = nShift - 1; i >= 0; i -= 2) {
    if (i == 0) {
      // If we are shifting an odd number of channels, we no longer have a
      // nice whole number of bytes. For the last channel, we have to send
      // a byte and a nibble.
      while(!(SPSR & (1<<SPIF)));
      SPDR = (uint8_t)((p_nValues[i] >> 4) & 0x00FF);

      // TODO: figure out how to properly transfer the last nibble (4 bits)
      while(!(SPSR & (1<<SPIF)));
      for (uint8_t ii = 0; ii < 4; ii++) {
        // Send the current bit
        if (((p_nValues[i] & 0x000F)<<ii) & (1<<3)) {
          *s_nMOSI.port |= _BV(s_nMOSI.pin);
        } else {
          *s_nMOSI.port &= ~(_BV(s_nMOSI.pin));
        }

        // Clock in the current bit (serial clock) [rising edge]
        *s_nSCK.port |= _BV(s_nSCK.pin);
        *s_nSCK.port &= ~(_BV(s_nSCK.pin));
      }
    } else {
      // Break every two channels into 3 bytes and send them
      while(!(SPSR & (1<<SPIF)));
      SPDR = (uint8_t)((p_nValues[i] >> 4) & 0x00FF);
      while(!(SPSR & (1<<SPIF)));
      SPDR = (uint8_t)((p_nValues[i] << 4) & 0x00F0) | (uint8_t)((p_nValues[i - 1] >> 8) & 0x000F);
      while(!(SPSR & (1<<SPIF)));
      SPDR = (uint8_t)(p_nValues[i - 1] & 0x00FF);
    }
  }

  // Latch the data (send it to the outputs)
  for (uint8_t i = 0; i < s_nNumChips; i++) {
    latch(i);
  }
  // Enable the outputs
  for (uint8_t i = 0; i < s_nNumChips; i++) {
    enable(i);
  }
  // Clear the modified flag
  s_bModified = false;

  // No memory leaks here!
  delete[] p_nValues;
}
