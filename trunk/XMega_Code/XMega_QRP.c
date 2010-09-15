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
#include "include/clksys_driver.h"
#include "include/AD9835_Xmega.h"
#include "include/MOD_RTTY.h"

#define CARRIER_FREQ    1000

#define LEDPORT     PORTE
#define SWITCHPORT  PORTF
#define AD9835_PORT PORTD

#define RTTY_MASK   0x01
#define DOMINO_MASK 0x02
#define MANUAL_MASK 0x04
#define CARRIER_MASK 0x08


int STATE = MANUAL_MASK;
char int_str[16];


ISR(TCC0_OVF_vect)
{
    LEDPORT.OUTTGL = 0x40;
    
    int temp = SWITCHPORT.IN & 0xFF;
    
    if((temp != 0xFF) && (temp != ~STATE)){
        
        if((~temp&0xFF) == CARRIER_MASK){
            //LEDPORT.OUTTGL = 0x10;
            LEDPORT.OUTSET = 0x0F;
            if((STATE&0xFF) == MANUAL_MASK){
                LEDPORT.OUTCLR = (STATE | CARRIER_MASK);
                
                AD9835_SelectFREG(1);
                
                while ((SWITCHPORT.IN & CARRIER_MASK) == 0x00);
                
                AD9835_SelectFREG(0);
                
                LEDPORT.OUTSET = CARRIER_MASK;
            }
        }else{
            //LEDPORT.OUTTGL = 0x20;
            STATE = ~temp;
            LEDPORT.OUTSET = 0x0F;
            LEDPORT.OUTCLR = STATE;
            Set_Mode();
        }
	}
	
        
}


void Set_Mode(){
    switch (STATE&0xFF){
        case RTTY_MASK:
            AD9835_Sleep();
            RTTY_Setup(CARRIER_FREQ, 425, 300, 1);
            AD9835_Awake();
            break;
        case DOMINO_MASK:
            AD9835_Sleep();
            break;
        case MANUAL_MASK:
            AD9835_Sleep();
            AD9835_UseFSEL(0);
            AD9835_SelectFREG(0);
            AD9835_SetFreq(0);
            AD9835_SelectFREG(1);
            AD9835_SetFreq(CARRIER_FREQ);
            AD9835_UseFSEL(1);
            AD9835_SelectFREG(0);
            AD9835_Awake();
            break;
        default:
            break;
    }
}

void TXString(char *string){
    switch (STATE&0xFF){
        case RTTY_MASK:
            RTTY_TXString(string);
            break;
        case DOMINO_MASK:
            break;
        case MANUAL_MASK:
            break;
        default:
            break;
    }    
}           

int main(void)
{
    Config32MHzClock();
  
    LEDPORT.DIRSET = 0xFF; // Setup the LED port for output.
    LEDPORT.OUT = ~STATE;
    //AD9835_PORT.DIRSET=0xFF;
  
    // Setup Buttons for internal pullup, and input.
    PORTCFG.MPCMASK=0xFF;
    SWITCHPORT.PIN0CTRL = PORT_OPC_PULLUP_gc;
    SWITCHPORT.DIRCLR = 0xFF;
    
    AD9835_Setup();
    //RTTY_Setup(CARRIER_FREQ, 425, 300, 1);
    Set_Mode();
    
    // Set up Timer/Counter 0.
	TCC0.PER = 1000; // Approx 1kHz
	TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV64_gc; // Work from CPUCLK/64.
	TCC0.INTCTRLA = ( TCC0.INTCTRLA & ~TC0_OVFINTLVL_gm ) | TC_OVFINTLVL_LO_gc; // Enable overflow interrupt.--
	
	// Enable low interrupt level in PMIC and enable global interrupts.
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	while(1) {
	    TXString("AD9835 TX Test\n");
	    _delay_ms(1000);
	    LEDPORT.OUTTGL = 0x80;
	}
}
  