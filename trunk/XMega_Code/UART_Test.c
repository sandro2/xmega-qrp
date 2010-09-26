

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#define F_CPU 32000000UL
#include <util/delay.h>
#include <avr/sleep.h>
#include <util/crc16.h>
#include "include/avr_compiler.h"
#include "include/xmega_lib.h"
//#include "include/clksys_driver.h"
#include "include/AD9835_Xmega.h"
#include "include/MOD_RTTY.h"
#include "include/MOD_DOMINO.h"
#include "include/TIMER.h"

#include "include/TinyGPS.h"


#include "include/GPS2.h"



#define LEDPORT     PORTE
#define SWITCHPORT  PORTF
#define AD9835_PORT PORTD

uint32_t CARRIER_FREQ = 1000;


char tx_buffer[256];
volatile int tx_flag = 0;

float lat, lon;
long altitude;
int sats, txCount, intTemp, extTemp, _intTemp, _extTemp, speed;
char latString[12], longString[12];
int time[3];

//gps_type Gps;

// tinyGPS object
TinyGPS gps;

void TX_Setup(){
    //Domino_Setup(CARRIER_FREQ,8);
    RTTY_Setup(CARRIER_FREQ, 425, 300, 1);
}

void TXString(char *string){
    //Domino_TXString(string);
    RTTY_TXString(string);
}

void floatToString(float val, int precision, char *string){

    // Print the int part
    sprintf(string, "%d", (long)(val));
    if(precision > 0) {
        // Print the decimal point
        strcat(string, ".");
        unsigned long frac;
        unsigned long mult = 1;
        int padding = precision -1;
        while (precision--) { mult *=10; }
        if (val >= 0)
            frac = (val - (long)(val)) * mult;
        else
            frac = ((long)(val)- val ) * mult;
        unsigned long frac1 = frac;
        while (frac1 /= 10) { padding--; }
        while (padding--) { strcat(string, "0"); }

        // Convert and print the fraction part
        sprintf(string+strlen(string), "%ld", frac);
    }
} 
 
uint16_t gps_CRC16_checksum (char *string)
{
	size_t i;
	uint16_t crc;
	uint8_t c;
 
	crc = 0xFFFF;
 
	// Calculate checksum ignoring the first two $s
	for (i = 2; i < strlen(string); i++)
	{
		c = string[i];
		crc = _crc_xmodem_update (crc, c);
	}
 
	return crc;
}

ISR(USARTD1_RXC_vect){
    gps.encode(USARTD1.DATA);
    PORTE.OUTTGL = 0x08;
}

int main(void) {    
    Config32MHzClock();
    
    LEDPORT.DIRSET = 0xFF; // Setup the LED port for output.
    LEDPORT.OUT = 0xFF;
    
    AD9835_PORT.DIRCLR = 0x40;
    PORTC.DIRSET = 0x04;
    
    _delay_ms(500);
    
    init_timer();
    
    AD9835_Setup();
    AD9835_Sleep();
    TX_Setup();
    AD9835_Awake();
    _delay_ms(1000);
    TXString("Starting up...\n");
    
    //Gps.packetflag = 0;
    
    init_gps();
    
    TXString("GPS Active\n");
    // Set up USART0 on Port C (USARTC0)
    //Setup_PortC_Usart();
    
    
    PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_LOLVLEN_bm;
    sei();
    
    TXString("Interrupts On.\n");
    
    unsigned int counter = 0;
 
    while(1){
    
        gps.f_get_position(&lat, &lon);
	    sats = gps.sats();
	    altitude = (long)gps.f_altitude();
	    gps.crack_datetime(0, 0, 0, &time[0], &time[1], &time[2]);
	    
	    floatToString(lat, 4, latString);
	    floatToString(lon, 4, longString);
        
        sprintf(tx_buffer,"$$DARKSIDE,%u,%02d:%02d:%02d,%s,%s,%ld,%d",counter++,time[0], time[1], time[2],latString,longString,altitude,sats);
        
        char checksum[10];
        
        snprintf(checksum, sizeof(checksum), "*%04X\n", gps_CRC16_checksum(tx_buffer));
     
        // It would be much more efficient to use the return value of snprintf here, rather than strlen
     
        // Also copy checksum's terminating \0 (hence the +1).
        memcpy(tx_buffer + strlen(tx_buffer), checksum, strlen(checksum) + 1);
        
        
        //sprintf(tx_buffer,"%u %u %g %s \n\r",(unsigned int)seconds(),counter++,Gps.,nmeabuffer);
        LEDPORT.OUTTGL = 0x01;
        
        
        TXString(tx_buffer);
        
        _delay_ms(1000);
        
        sendNMEA("$PUBX,00");
    }

    
}
    
    
        
    