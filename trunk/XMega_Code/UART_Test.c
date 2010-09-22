#define F_CPU 32000000UL

#include <stdio.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include "include/avr_compiler.h"
#include "include/xmega_lib.h"
#include "include/clksys_driver.h"
#include "include/AD9835_Xmega.h"
#include "include/MOD_RTTY.h"
#include "include/MOD_DOMINO.h"

#define LEDPORT     PORTE
#define SWITCHPORT  PORTF
#define AD9835_PORT PORTD

uint32_t CARRIER_FREQ = 7030000;


char usart_buffer[1024];
int usart_buffer_ptr = 0;


char tx_buffer[1024];
volatile int tx_flag = 0;


ISR(USARTC0_RXC_vect)
{
    char temp = USARTC0.DATA;
    usart_buffer[usart_buffer_ptr++] = temp;
    
    if((temp == 10) || (temp == 13) || (usart_buffer_ptr == 1023)) {
        strncpy(usart_buffer, tx_buffer, 1024);
        tx_buffer[usart_buffer_ptr] = 0;
        usart_buffer_ptr = 0;
        LEDPORT.OUTTGL = 0x04;
        tx_flag = 1;
    }
    
    LEDPORT.OUTTGL = 0x02;
}

int main(void) {    
    Config32MHzClock();
    
    LEDPORT.DIRSET = 0xFF; // Setup the LED port for output.
    LEDPORT.OUT = 0xFF;
    
    // Set up the AD9835
    _delay_ms(500);
    AD9835_Setup();
    LEDPORT.OUTCLR = 0x01;
    RTTY_Setup(CARRIER_FREQ, 425, 300, 1);
    
    // Set up USART0 on Port C (USARTC0)
    USARTC0.CTRLA = USART_RXCINTLVL_HI_gc;
    USARTC0.CTRLC = USART_CHSIZE_8BIT_gc;
    USARTC0.BAUDCTRLA = 207; // 9600 Baud when using a 2MHz Clock.
    USARTC0.CTRLB = USART_RXEN_bm;
    
    PMIC.CTRL = PMIC_HILVLEN_bm;
    sei();
    
    //AD9835_Awake();
 
    while(1){
        LEDPORT.OUTTGL = tx_flag<<3;
        if(tx_flag == 1){
            USARTC0.CTRLB = ~USART_RXEN_bm;
            LEDPORT.OUTSET = 0x01;
            
            AD9835_Awake();
            RTTY_TXString("$$TESTING,0,00:00:00,0.0000,0.0000,0,0,0;20;19*\n");//tx_buffer);
            //AD9835_Sleep();
            
            tx_flag = 0;
            
            
            LEDPORT.OUTCLR = 0x01;
            USARTC0.CTRLB = USART_RXEN_bm;
        }
    }
}
    
    
        
    