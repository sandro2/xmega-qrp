

#include <stdio.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#define F_CPU 32000000UL
#include <util/delay.h>
#include <avr/sleep.h>
#include "avr_compiler.h"
#include "xmega_lib.h"
#include "MOD_RTTY.h"
#include "MOD_Morse.h"
#include "base64_enc.h"
#include "crc16.h"

unsigned int checksum(unsigned char* data, unsigned int len){
    unsigned int buff = 0;
    unsigned int i = 0;
    
    for(i = 0; i<len; i++){
        buff += (unsigned int)data[i];
    }
    return buff;
}



int main(void)
{
  Config32MHzClock();
  PORTF.DIRSET = 0xFF;

  CLK.PSCTRL = 0x00; // no division on peripheral clock
  
  // Allocate 1kb for a data buffer
  uint8_t buffer[1024];
  uint16_t buffer_ptr = buffer;
  
 
  Setup_PortF_Usart();
 
    _delay_ms(500);

    RTTY_Setup(500, 850, 300, 1); // 1000Hz Carrier, 170Hz Shift, 50 baud, 1 stop bit
    //Morse_Setup(20, 5000000);
    
   AD9835_Awake();
    
   char *txdata = "abcd";
   unsigned int crc_val = checksum(txdata, 4);
  
    
   // base64enc(buffer, txdata, strlen(txdata));
    
    
    
    
    int adcb_val = 0;
    char adcb_str[16];
    while(1){
   
        adcb_val = DoADC_B(0,0,0,0);
        itoa(crc_val, adcb_str, 10);
        RTTY_TXString("CRC: ");
        RTTY_TXString(adcb_str);
        RTTY_TXString("\n");
        UsartWriteString("\n\r\n\rTesting 12345\n ");
        _delay_ms(500);
    
    }
    

 }