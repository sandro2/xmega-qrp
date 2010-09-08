

#include <stdio.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#define F_CPU 32000000UL
#include <util/delay.h>
#include <avr/sleep.h>
#include "include/avr_compiler.h"
#include "include/xmega_lib.h"
#include "include/MOD_RTTY.h"
#include "include/MOD_Morse.h"
#include "include/base64_enc.h"

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
  PORTE.DIRSET = 0xFF;

  CLK.PSCTRL = 0x00; // no division on peripheral clock
  
  // Allocate 1kb for a data buffer
  uint8_t buffer[1024];
  uint16_t buffer_ptr = buffer;
  
 
  //Setup_PortF_Usart();
 
    _delay_ms(500);

    RTTY_Setup(7500000, 200, 100, 1); // 1000Hz Carrier, 170Hz Shift, 50 baud, 1 stop bit
    //Morse_Setup(20, 5000000);
    
   AD9835_Awake();
    
   char *txdata = "abcd";
   unsigned int adc_val = 0;
  
    
   // base64enc(buffer, txdata, strlen(txdata));
    
    uint8_t count = 0;
    
    
    char int_str[16];
    while(1){
        adc_val = DoADC_A(0,0,0,0);
        itoa(adc_val, int_str, 10);
        RTTY_TXString("AD9835 Sig-Gen Test - ADC_B: ");
        RTTY_TXString(int_str);
        RTTY_TXString("\n");
        _delay_ms(500);
        PORTE.OUTCLR = 0xFF;
        PORTE.OUTSET = ~count++;
    
    }
    

 }