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
#include "avr_compiler.h"
#include "AD9834_XMega.h"
#include <math.h>


unsigned int AD9834_CONTROL = 0x01C8;

int PINSW = 0;
int FREG = 0;

void AD9834_Use_Pins(int value){
    if(value){
        AD9834_CONTROL |= 0x0100;
        PINSW = 1;
    }else{
        AD9834_CONTROL &= ~0x0100;
        PINSW = 0;
    }
    AD9834_SendWord(AD9834_CONTROL);
}

void AD9834_Sign_Bit_On(int value){
    if(value){
        AD9834_CONTROL |= 0x0020;
    }else{
        AD9834_CONTROL &= ~0x0020;
    }
    AD9834_SendWord(AD9834_CONTROL);
}

void AD9834_DAC_ON(int value){
    if(value){
        AD9834_CONTROL &= ~0x0040; // Clear the SLEEP12 bit.
    }else{
        AD9834_CONTROL |= 0x0040; // Set the SLEEP12 bit.
    }
    AD9834_SendWord(AD9834_CONTROL);
}

void AD9834_SelectFREG(int value){
    if(PINSW){
        if(value)   AD9834_PORT.OUTSET = FSEL_PIN;//digitalWrite(FSEL_PIN,HIGH);
        else        AD9834_PORT.OUTCLR = FSEL_PIN;//digitalWrite(FSEL_PIN,LOW);
    }else{
        if(value)   AD9834_CONTROL |= 0x0800;
        else        AD9834_CONTROL &= ~0x0800;
        AD9834_SendWord(AD9834_CONTROL);
    }
}

void AD9834_SelectPREG(int value){
    if(PINSW){
        if(value)   AD9834_PORT.OUTSET = PSEL_PIN;//digitalWrite(PSEL_PIN,HIGH);
        else        AD9834_PORT.OUTCLR = PSEL_PIN;//digitalWrite(PSEL_PIN,LOW);
    }else{
        if(value)   AD9834_CONTROL |= 0x0400;
        else        AD9834_CONTROL &= ~0x0400;
        AD9834_SendWord(AD9834_CONTROL);
    }
}

void AD9834_Reset(int value){
    if(PINSW){
        if(value)   AD9834_PORT.OUTSET = RESET_PIN;
        else        AD9834_PORT.OUTCLR = RESET_PIN;
    }else{
        if(value)   AD9834_CONTROL |= 0x0100;
        else        AD9834_CONTROL &= ~0x0100;
        AD9834_SendWord(AD9834_CONTROL);
    }
}

void AD9834_Sleep(int value){
    if(PINSW){
        if(value)   AD9834_PORT.OUTSET = SLEEP_PIN;//digitalWrite(SLEEP_PIN,HIGH);
        else        AD9834_PORT.OUTCLR = SLEEP_PIN;//digitalWrite(SLEEP_PIN,LOW);
    }else{
        if(value)   AD9834_CONTROL |= 0x0080;
        else        AD9834_CONTROL &= ~0x0080;
        AD9834_SendWord(AD9834_CONTROL);
    }
}

void AD9834_Triangle_Output(int value){
    if(value)   AD9834_CONTROL |= 0x0002;
    else        AD9834_CONTROL &= ~0x0002;
    AD9834_SendWord(AD9834_CONTROL);
}

// Sets FREG0 to the value required to produce a specified frequency, in Hz.
unsigned long AD9834_SetFreq(int f_reg, unsigned long freq){
	unsigned int data;
	unsigned long temp;
	unsigned int f_LSB;
	unsigned int f_MSB;
	temp=((unsigned long)((float)freq*5.36871))&0x0FFFFFFF;
	if(f_reg==1){
	    f_LSB = (0x8000 | (unsigned int)(temp & 0x00003FFF));
		f_MSB = (0x8000 | (unsigned int)((temp>>14) & 0x3FFF));
	}else{
		f_LSB = (0x4000 | (unsigned int)(temp & 0x00003FFF));
		f_MSB = (0x4000 | ((unsigned int)(temp>>14) & 0x3FFF));
	}
	//Serial.println("Setting Frequency - Sending Control Word");
    AD9834_SendWord(AD9834_CONTROL|0x2000);
   // Serial.println("Setting Frequency - Sending LSB");
    AD9834_SendWord(f_LSB);
    //Serial.println("Setting Frequency - Sending MSB");
    AD9834_SendWord(f_MSB);
    //Serial.println("Setting Frequency - Clearing B28");
    AD9834_SendWord(AD9834_CONTROL);
    return temp;
}

// Initialises the AD9834, clearing all the registers, and putting it into sleep mode.
void AD9834_Setup(){
    AD9834_PORT.DIRSET = SDATA_PIN|SCLK_PIN|FSYNC_PIN|FSEL_PIN|RESET_PIN|SLEEP_PIN;//|PSEL_PIN;
	
	AD9834_PORT.DIRSET = FSYNC_PIN;
	AD9834_PORT.DIRCLR = SLEEP_PIN|RESET_PIN|FSEL_PIN;//|PSEL_PIN;


	//AD9834_SendWord(0xF800); // 0b11111000 - Sleep chip, Reset, and Clear registers.
	//AD9834_UseFSEL(false);
	//AD9835_SendWord(0xB000); // 0b10110000 - Sync reading of FSEL/PSEL0/PSEL1 with
						 // MCLK, and select FSEL/PSEL0/PSEL1 with their bits.
	//AD9834_SendWord(0x5000); // 0b01010000 - Ensure FREG0 is selected.
	//AD9834_SelectFREG(0);
	//AD9835_SendWord(0x4000); // 0b01000000 - Use PREG0.
	//AD9834_SendWord(0x1800); // 0b00011000 - Null the phase register
	//AD9834_SendWord(0x0900); // 0b00001001 - ^
	
	// Set everything to off, basically.
	AD9834_SendWord(AD9834_CONTROL);
	
	// Null the phase Registers.
	AD9834_SendWord(0xC000);
	AD9834_SendWord(0xE000);
//	AD9834_SendWord(0x2248);
}

void AD9834_SINE_ON(){
    AD9834_Sleep(0);
    AD9834_DAC_ON(1);
    AD9834_Reset(0);
}
    

// Send a 16-bit data word to the AD9834, MSB first.
void AD9834_SendWord(unsigned int data){
	uint16_t temp;
	uint16_t bitMask = 0x8000;
	//digitalWrite(SCLK_PIN,HIGH);
	AD9834_PORT.OUTSET = SCLK_PIN;
  
	//digitalWrite(FSYNC_PIN,HIGH);
	AD9834_PORT.OUTSET = FSYNC_PIN;
	//digitalWrite(FSYNC_PIN,LOW); // Set FSYNC low to signify we are sending data.
	AD9834_PORT.OUTCLR = FSYNC_PIN;
  
	for(int i = 0; i<16; i++){
    	temp = data;
    	if((data&bitMask)==0){
    	    //digitalWrite(SDATA_PIN,LOW);
    	    AD9834_PORT.OUTCLR = SDATA_PIN;
    	}
    	else{
    	    //digitalWrite(SDATA_PIN,HIGH);
    	    AD9834_PORT.OUTSET = SDATA_PIN;
    	}
    	bitMask = bitMask>>1;
    
		//digitalWrite(SCLK_PIN,HIGH); // Pulse the clock
  	  	//digitalWrite(SCLK_PIN,LOW);
  	  	AD9834_PORT.OUTSET = SCLK_PIN;
  	  	AD9834_PORT.OUTCLR = SCLK_PIN;
  	  	AD9834_PORT.OUTSET = SCLK_PIN;
   	 	//digitalWrite(SCLK_PIN,HIGH);
    
	}
	//digitalWrite(FSYNC_PIN,HIGH); // Set FSYNC high to signify we have finished.
	AD9834_PORT.OUTSET = FSYNC_PIN;
}




