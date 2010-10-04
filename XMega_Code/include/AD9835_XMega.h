/*
	2010-07-30
	AD9835 Control Library
	
	Author: Mark Jessop

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

#ifndef AD9835_XMEGA_H
#define AD9835_XMEGA_H 1

#define SPI_NUM_BYTES     2

#define SDATA   PIN0_bm
#define SCLK    PIN1_bm
#define FSYNC 	PIN2_bm
#define FSEL 	PIN3_bm

void AD9835_SendWord(uint16_t data_word);
void AD9835_Setup( );
void AD9835_SetFreq(uint32_t freq);
void AD9835_Sleep();
void AD9835_Awake();
void AD9835_UseFSEL(int c);
void AD9835_SelectFREG(int reg_number);

#endif