/*
	2010-07-30
	AD9835 Control Library
	
	Author: Mark Jessop
*/

#include "avr_compiler.h"
#include "spi_driver.h"
#include "AD9835_XMega.h"
#include <math.h>



int FREG = 0;
int SELSRC = 0;

//SPI_Master_t spiMasterC;
//PORT_t *ssPort = &PORTC;


void AD9835_SelectFREG(int reg_number){
	FREG = reg_number;
	if(SELSRC){
		if(reg_number == 0){
			AD9835_SendWord(0x6000); // FREG0
		}
		else{
			AD9835_SendWord(0x6800); // FREG1
		}
	}
	else{
		if(reg_number == 0){
			PORTD.OUTCLR = FSEL; // FREG0
		}
		else{
			PORTD.OUTSET = FSEL; // FREG1
		}
	}
}


void AD9835_Sleep(){
	AD9835_SendWord(0xE000);
}

void AD9835_Awake(){
	AD9835_SendWord(0xC000);
}
/*
void AD9835_SendWord(uint16_t data_word){
	//SPI_MasterSSLow(ssPort, FSYNC);
	PORTC.OUTCLR = FSYNC;
	uint8_t word1 = (uint8_t)(data_word>>8);
	uint8_t word2 = (uint8_t)(data_word&0xFF);
	SPI_MasterTransceiveByte(&spiMasterC, word1);
	SPI_MasterTransceiveByte(&spiMasterC, word2);
	//SPI_MasterSSHigh(ssPort, FSYNC);
	PORTC.OUTSET = FSYNC;
}
*/
void AD9835_SendWord(uint16_t data){
	uint16_t temp;
	uint16_t bitMask = 0x8000;
	//digitalWrite(SCLK_PIN,HIGH);
	PORTD.OUTSET = SCLK;
  
	//digitalWrite(FSYNC_PIN,HIGH);
	PORTD.OUTSET = FSYNC;
	//digitalWrite(FSYNC_PIN,LOW); // Set FSYNC low to signify we are sending data.
	PORTD.OUTCLR = FSYNC;
  
	for(int i = 0; i<16; i++){
    	temp = data;
    	if((data&bitMask)==0){
    	    //digitalWrite(SDATA_PIN,LOW);
    	    PORTD.OUTCLR = SDATA;
    	}
    	else{
    	    //digitalWrite(SDATA_PIN,HIGH);
    	    PORTD.OUTSET = SDATA;
    	}
    	bitMask = bitMask>>1;
    
		//digitalWrite(SCLK_PIN,HIGH); // Pulse the clock
  	  	//digitalWrite(SCLK_PIN,LOW);
  	  	PORTD.OUTSET = SCLK;
  	  	PORTD.OUTCLR = SCLK;
  	  	if(F_CPU>2000000){
  	  	    _delay_us(1);
  	  	}
  	  	PORTD.OUTSET = SCLK;
   	 	//digitalWrite(SCLK_PIN,HIGH);
    
	}
	//digitalWrite(FSYNC_PIN,HIGH); // Set FSYNC high to signify we have finished.
	PORTD.OUTSET = FSYNC;
}

void AD9835_SetFreq(uint32_t freq){
	uint16_t data;
	uint32_t temp;
	freq=(uint32_t)(freq/11.64153218e-3);
	if(FREG == 0){
		temp=freq;
		temp=temp&0x000000ff;
		data=(uint16_t)(0x3000+(temp));        //get 8L LSB with mask
		AD9835_SendWord(data);
		temp=freq;
		temp=temp&0x0000ff00;
		data=(uint16_t)(0x2100+(temp>>=8));    //get 8H LSB with mask
		AD9835_SendWord(data);
		temp=freq;
		temp=temp&0x00ff0000;
		data=(uint16_t)(0x3200+(temp>>=16));   //get 8L MSB with mask
		AD9835_SendWord(data);
		temp=freq;
		temp=temp&0xff000000;
		data=(uint16_t)(0x2300+(temp>>=24));   //get 8H MSB with mask
		AD9835_SendWord(data);
	}
	if(FREG == 1){
		temp=freq;
		temp=temp&0x000000ff;
		data=(uint16_t)(0x3400+(temp));        //get 8L LSB with mask
		AD9835_SendWord(data);
		temp=freq;
		temp=temp&0x0000ff00;
		data=(uint16_t)(0x2500+(temp>>=8));    //get 8H LSB with mask
		AD9835_SendWord(data);
		temp=freq;
		temp=temp&0x00ff0000;
		data=(uint16_t)(0x3600+(temp>>=16));   //get 8L MSB with mask
		AD9835_SendWord(data);
		temp=freq;
		temp=temp&0xff000000;
		data=(uint16_t)(0x2700+(temp>>=24));   //get 8H MSB with mask
		AD9835_SendWord(data);
	}
}


void AD9835_UseFSEL(int c){
	if(c){
		AD9835_SendWord(0xA000);
		SELSRC = 0;
	}
	else{
		AD9835_SendWord(0xB000);
		SELSRC = 1;
	}
}

/*	Setup a SPI Master to control the AD9834 on Port C
	Pin 0 = RESET
	Pin 1 = SLEEP
	Pin 2 = PSEL
	Pin 3 = FSEL
	Pin 4 = FSYNC
	Pin 5 = MOSI (Master Out)
	Pin 6 = MISO (Not Used)
	Pin 7 = SPI Clock
*/
void AD9835_Setup(){
	/* Set Pins to be outputs */
	PORTD.DIRSET = FSYNC | FSEL | PSEL | SDATA | SCLK;
	
	/* Setup the SS (FSYNC) Pin */
//	PORTC.DIRSET = FSYNC;
//	PORTC.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc;
	/* Setup the FSEL Pin */
//	PORTC.DIRSET = FSEL;
//	PORTC.PIN3CTRL = PORT_OPC_WIREDANDPULL_gc;

	
	/* Set Output Pins */
    PORTD.OUTSET = FSYNC;
    PORTD.OUTCLR = FSEL;

	/* Instantiate pointer to ssPort. */
	//*ssPort = &PORTC;
	
/*
	SPI_MasterInit(&spiMasterC,
	               &SPIC,
	               &PORTC,
	               false,
	               SPI_MODE_0_gc,
	               SPI_INTLVL_OFF_gc,
	               false,
	               SPI_PRESCALER_DIV4_gc);
	               */
	               
	AD9835_SendWord(0xF800); // 0b11111000 - Sleep chip, Reset, and Clear registers.
	AD9835_UseFSEL(0);
	//AD9835_SendWord(0xB000); // 0b10110000 - Sync reading of FSEL/PSEL0/PSEL1 with
						 // MCLK, and select FSEL/PSEL0/PSEL1 with their bits.
	AD9835_SendWord(0x5000); // 0b01010000 - Ensure FREG0 is selected.
	AD9835_SelectFREG(0);
	//AD9835_SendWord(0x4000); // 0b01000000 - Use PREG0.
	AD9835_SendWord(0x1800); // 0b00011000 - Null the phase register
	AD9835_SendWord(0x0900); // 0b00001001 - ^
}



/*
2308 0b0000 0011 0000 1000 // 
599A 0b0101 1001 1001 1010
4666 0b0100 0110 0110 0110
A62F 0b1010011000101111
8666 0b1000011001100110
C000 0b1100000000000000
E800 0b1110100000000000
2208 0b0010001000001000
*/
	