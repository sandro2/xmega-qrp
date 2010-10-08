/*
    Timer Interrupt Helper Library
    
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
#include "TIMER.h"

uint32_t millis_counter = 0;
uint32_t second_counter = 0;
uint32_t minute_counter = 0;

ISR(TCC0_OVF_vect){
    millis_counter++;
    
    if(millis_counter%1000 == 0){
    
        second_counter++;
        
        PORTE.OUTTGL = 0x10; // Flash a LED every second.
        
        if(second_counter%60 == 0){
            minute_counter++;
        }
    }
    /*
    int temp = SWITCHPORT.IN & 0xFF;
    
    switch(~temp&0xFF){
        case RTTY_MASK:
            new_mode = RTTY_300;
            LEDPORT.OUTSET = 0x0F;
            LEDPORT.OUTCLR = RTTY_MASK;
            
            break;
        case DOMINO_MASK:
            new_mode = DOMINOEX8;
            LEDPORT.OUTSET = 0x0F;
            LEDPORT.OUTCLR = DOMINO_MASK;
            break;
        case CW_MASK:
            new_mode = QRSS;
            LEDPORT.OUTSET = 0x0F;
            LEDPORT.OUTCLR = CW_MASK;
            break;
        default:
            break; // Don't change anything!
    }
    */
}

void init_timer(){
    TCC0.PER = 125;
    TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV256_gc;
    TCC0.INTCTRLA = ( TCC0.INTCTRLA & ~TC0_OVFINTLVL_gm ) | TC_OVFINTLVL_LO_gc;
}

uint32_t seconds(){
    return second_counter;
}

// Hack for arduino code compatability
uint32_t millis(){
    return millis_counter;
}