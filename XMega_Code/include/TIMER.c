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