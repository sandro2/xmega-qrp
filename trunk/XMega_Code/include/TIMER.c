/*
    Timer Interrupt
    
*/

#include "avr_compiler.h"
#include "TIMER.h"

uint32_t second_counter = 0;
uint32_t minute_counter = 0;

ISR(TCC0_OVF_vect){
    second_counter++;
    
    PORTE.OUTTGL = 0x10; // Flash a LED every second.
    
    if(second_counter%60 == 0){
        minute_counter++;
    }

}

void init_timer(){
    TCC0.PER = 31250;
    TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV1024_gc;
    TCC0.INTCTRLA = ( TCC0.INTCTRLA & ~TC0_OVFINTLVL_gm ) | TC_OVFINTLVL_LO_gc;
}

uint32_t seconds(){
    return second_counter;
}

// Hack for arduino code compatability
uint32_t millis(){
    return second_counter;
}