/*
    RTTY Transmission Library
    Date: 10-08-2010
    Author: Mark Jessop
    
    This library will transmit strings over RTTY modulation (Binary FSK), at a specified baud rate, carrier frequency and frequency shift, with 1 start bit and 2 stop bits.
    
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
#include "MOD_RTTY.h"

//uint16_t RTTY_Delay_Time = 109; // Delay between bit changes, in ms
double RTTY_Delay_Time = 20;
uint8_t stop_bits = 1;

void RTTY_Setup(uint32_t base_freq, uint32_t shift_freq, uint16_t baud_rate,  uint8_t stops){
   uint32_t LOGICAL_0 = base_freq;
   uint32_t LOGICAL_1 = base_freq + shift_freq;
   //RTTY_Delay_Time = (uint16_t)500000/baud_rate;
   RTTY_Delay_Time = 1000.0 / (double)baud_rate;
   stop_bits = stops;
   
   //Setup32KHzRTC(109);//RTTY_Delay_Time);
   
 //  TCD0.PER = 417;
  // TCD0.CTRLA = TC_CLKSEL_DIV256_gc;

   //AD9835_Setup();
   AD9835_UseFSEL(0);
   AD9835_SelectFREG(0);
   AD9835_SetFreq(LOGICAL_0);
   AD9835_SelectFREG(1);
   AD9835_SetFreq(LOGICAL_1);
   AD9835_UseFSEL(1);
}

void RTTY_TXBit(int bit_val){
    AD9835_SelectFREG(bit_val);
    RTTY_Delay();
}

void RTTY_Delay(){
/*
while (!(TCD0.INTFLAGS & TC0_OVFIF_bm));
  TCD0.INTFLAGS = TC0_OVFIF_bm;
*/
_delay_ms(RTTY_Delay_Time);
}

void RTTY_TXByte(uint8_t data){
    uint8_t i = 0;
    
    // Send Start bit
    RTTY_TXBit(0);
    
    // Send byte bits, LSB first.
    for (i=0; i<8; i++){
        if( data & 1 ) RTTY_TXBit(1);
        else RTTY_TXBit(0);
        data = data >> 1;
    }
    
    // Send Stop bits
    for( i = 0; i < stop_bits; i++){
        RTTY_TXBit(1);
    }
}

void RTTY_TXString(char *string) {
  //  TCD0.CNT = 0;
  //  TCD0.INTFLAGS = TC0_OVFIF_bm;
    
    AD9835_Awake();
    //AD9835_SelectFREG(0);
    //_delay_ms(1000);
    //AD9835_SelectFREG(1);
    _delay_ms(500);
    
    RTTY_Delay();

	for (int i = 0; i < strlen(string); i++) {
		RTTY_TXByte(string[i]);
	}
}




    
    
