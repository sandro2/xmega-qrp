/*
	2010-10-12
	AD9834 Control Library
	
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

#ifndef AD9834_XMEGA_H
#define AD9834_XMEGA_H 1

#define AD9835_PORT PORTD

#define SDATA_PIN	PIN0_bm
#define SCLK_PIN	PIN1_bm
#define	FSYNC_PIN	PIN2_bm
#define FSEL_PIN	PIN3_bm
#define RESET_PIN   PIN4_bm
#define SLEEP_PIN   PIN5_bm
#define PSEL_PIN    PIN6_bm



// Function Definitions
void AD9834_SendWord(unsigned int data);
void AD9834_Use_Pins(int value);
void AD9834_Sign_Bit_On(int value);
void AD9834_DAC_ON(int value);
void AD9834_SelectFREG(int value);
void AD9834_SelectPREG(int value);
void AD9834_Reset(int value);
void AD9834_Sleep(int value);
void AD9834_Triangle_Output(int value);
unsigned long AD9834_SetFreq(int f_reg, unsigned long freq);
void AD9834_Setup();
void AD9834_SINE_ON();

#endif