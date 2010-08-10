

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



int main(void)
{
  Config32MHzClock();
  PORTF.DIRSET = 0xFF;

  CLK.PSCTRL = 0x00; // no division on peripheral clock
 
  Setup_PortF_Usart();
 
    _delay_ms(500);

    //RTTY_Setup(1000, 850, 300); // 1000Hz Carrier, 170Hz Shift, 50 baud
    Morse_Setup(50, 1000);
  //  AD9835_Awake();
    
    while(1){
    
        Morse_TXString("TRANSMISSION START: AD9835 Transmitter Test! 1234567890 abcdefghijklmnopqrstuvwxyz\n");
        UsartWriteString("\n\r\n\rTesting 12345\n ");
        //_delay_ms(500);
    }
    

 }