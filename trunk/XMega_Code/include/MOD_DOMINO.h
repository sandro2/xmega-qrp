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

#ifndef MOD_DOMINO_H
#define MOD_DOMINO_H 1

#define NUM_TONES 18
#define TONE_SHIFT 36
#define DOMINO_DELAY 1484

void Domino_Setup(uint32_t base_freq, int mode);
static void dominoex_sendtone(uint8_t tone);
static void dominoex_sendsymbol(uint8_t sym);
void transmit_dominoex_character(uint8_t c);
void Domino_TXString(char *string);

#endif