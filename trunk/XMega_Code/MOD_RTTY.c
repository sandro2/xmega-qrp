/*
    RTTY Transmission Library
    Date: 10-08-2010
    Author: Mark Jessop
    
    This library will transmit strings over RTTY modulation (Binary FSK), at a specified baud rate, carrier frequency and frequency shift, with 1 start bit and 2 stop bits.
*/

#include "avr_compiler.h"
#include "AD9835_Xmega.h"
#include "MOD_RTTY.h"

double RTTY_Delay_Time = 20; // Delay between bit changes, in ms

void RTTY_Setup(uint32_t base_freq, uint32_t shift_freq, uint16_t baud_rate){
   uint32_t LOGICAL_0 = base_freq;
   uint32_t LOGICAL_1 = base_freq + shift_freq;
   RTTY_Delay_Time = 1000.0 / (double)baud_rate;

   AD9835_Setup();
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
   _delay_ms(RTTY_Delay_Time);
  //  _delay_ms(500);
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
    RTTY_TXBit(1);
    RTTY_TXBit(1);
}

void RTTY_TXString(char *string) {

	for (int i = 0; i < strlen(string); i++) {
		RTTY_TXByte(string[i]);
	}
}


    
    
