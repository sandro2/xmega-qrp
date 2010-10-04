/*
    DominoEX Transmission Library.
    
    Adapted from code written by Daniel Richman
    http://github.com/danielrichman/avr/blob/master/xplain-x128a1/dac_domex.c
    
    Ported to the Atmel XMega by Mark Jessop - Sept 2010.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    For a full copy of the GNU General Public License, 
    see <http://www.gnu.org/licenses/>.
*/

#include "avr_compiler.h"
#include <string.h>
#include "AD9835_Xmega.h"
#include "MOD_DOMINO.h"

static uint8_t varicode[] = 
  "\xf1\x19\xaf\xf1\x1b\xcf\xf1\x1d\xef\xf1\x2f\x88\xc2\x20\x98\x82\x2a"
  "\xb8\x82\x2c\x0d\x82\x2d\xe8\x82\x2f\x89\x92\x29\xa9\x92\x2b\xc9\x92"
  "\x2d\xe9\x92\x2f\x8a\xa2\x29\xaa\xa2\x2b\xca\xa2\x2d\xea\x00\x70\x0b"
  "\x80\x0e\xba\x90\x0a\x99\x80\x7f\x0a\x80\x0c\xb8\x90\x0d\x88\xb2\x70"
  "\x0e\xd7\x00\x98\xf3\x40\x0a\xf4\x50\x09\x86\x50\x0c\xe5\x60\x0c\xb6"
  "\x60\x0e\x80\x0a\xd8\xa0\x78\x0f\x90\x7f\x0c\x90\x38\x09\xe4\x30\x0c"
  "\xe3\x30\x08\xc4\x50\x08\xa5\x30\x0a\x87\x60\x0a\xb4\x40\x08\xd4\x30"
  "\x0b\x94\x60\x0f\xd3\x20\x0f\xe2\x50\x0b\xd6\x50\x0d\xf5\x60\x09\x97"
  "\x00\xea\xa0\x09\xfa\xa0\x0a\xc9\x90\x4b\x00\xb1\x00\x0c\xb0\x10\x00"
  "\xf0\x10\x09\xa0\x50\x00\xa2\x10\x0e\x90\x00\x0e\x06\x30\x00\x81\x20"
  "\x08\x07\x00\x08\x02\x00\x0d\xd1\x10\x0c\xf1\x10\x0a\x92\x00\xca\x90"
  "\x0e\xda\xb0\x28\xfa\xb2\x28\x9b\xb2\x2a\xbb\xb2\x2c\xdb\xb2\x2e\xfb"
  "\xc2\x28\x9c\xc2\x2a\xbc\xc2\x2c\xdc\xc2\x2e\xfc\xd2\x28\x9d\xd2\x2a"
  "\xbd\xd2\x2c\xdd\xd2\x2e\xfd\xe2\x28\x9e\xe2\x2a\xbe\xe2\x2c\xde\xe2"
  "\x2e\xfe\xb0\x09\xab\xb0\x0b\xcb\xb0\x0d\xeb\xb0\x0f\x8c\xc0\x09\xac"
  "\xc0\x0b\xcc\xc0\x0d\xec\xc0\x0f\x8d\xd0\x09\xad\xd0\x0b\xcd\xd0\x0d"
  "\xed\xd0\x0f\x8e\xe0\x09\xae\xe0\x0b\xce\xe0\x0d\xee\xe0\x0f\x8f\xf0"
  "\x09\xaf\xf0\x0b\xcf\xf0\x0d\xef\xf0\x1f\x88\x81\x19\xa8\x81\x1b\xc8"
  "\x81\x1d\xe8\x81\x1f\x89\x91\x19\xa9\x91\x1b\xc9\x91\x1d\xe9\x91\x1f"
  "\x8a\xa1\x19\xaa\xa1\x1b\xca\xa1\x1d\xea\xa1\x1f\x8b\xb1\x19\xab\xb1"
  "\x1b\xcb\xb1\x1d\xeb\xb1\x1f\x8c\xc1\x19\xac\xc1\x1b\xcc\xc1\x1d\xec"
  "\xc1\x1f\x8d\xd1\x19\xad\xd1\x1b\xcd\xd1\x1d\xed\xd1\x1f\x8e\xe1\x19"
  "\xae\xe1\x1b\xce\xe1\x1d\xee\xe1\x1f\x8f\xf6";

static uint16_t domino8_tones[] = 
{0,16,31,47,63,78,94,109,125,141,156,172,188,203,219,234,250,266};

static uint16_t domino11_tones[] = {0,11,22,32,43,54,65,75,86,97,108,118,129,140,151,161,172,183};

static uint16_t domino22_tones[] = 
{0,22,43,65,86,108,129,151,172,194,215,237,258,280,301,323,345};

static uint16_t *tones;
//{0,11,22,32,43,54,65,75,86,97,108,118,129,140,151,161,172,183};

static double domino_delay;

uint32_t DOMINO_BASE = 1000;

static void dominoex_sendtone(uint8_t tone)
{
    _delay_ms(domino_delay);

  AD9835_SetFreq(DOMINO_BASE + (uint32_t)tones[tone]);
}

static void dominoex_sendsymbol(uint8_t sym)
{
  uint8_t tone;  
  static uint8_t txprevtone = 0;

  tone = (txprevtone + 2 + sym) % NUM_TONES;
  txprevtone = tone;
  dominoex_sendtone(tone);
}

void transmit_dominoex_character(uint8_t c)
{

  uint16_t data;

  /* 2 nibbles are packed in 3 bytes; to retrieve the
   * data multiply c by (3/2). See head of file*/
  data = *((uint16_t *) (varicode + ((c * 3) / 2)));

  if (c & 0x01)
  {
    /* If it was odd we need to discard the first nibble */
    data >>= 4;
  }

  /* First nibble will not have the MSB set, but any multi-nibble
   * chars will have 0x08 set in their "continuation nibbles"  */
  do
  {
    dominoex_sendsymbol(data & 0xF);
    data >>= 4;
  }
  while (data & 0x8);
}

void Domino_TXString(char *string) {
    //AD9835_SetFreq(0);
	for (int i = 0; i < strlen(string); i++) {
		transmit_dominoex_character(string[i]);
	}
	//AD9835_SetFreq(0);
}

void Domino_Setup(uint32_t base_freq, int mode){
   DOMINO_BASE = base_freq;
   
   switch(mode){
    case 8:
        tones = domino8_tones;
        domino_delay = 127.8527;
        break;
    case 11:
        tones = domino11_tones;
        domino_delay = 92.885;
        break;
    case 22:
        tones = domino22_tones;
        domino_delay = 46.4403;
        break;
    default:
        tones = domino11_tones;
        domino_delay = 92.885;
        break;
    }
   
   
   AD9835_UseFSEL(0);
   AD9835_SelectFREG(0);
   AD9835_SetFreq(0);
}
