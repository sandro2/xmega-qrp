

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
#include "include/adc_driver.h"
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

// Carrier Frequency Setting
uint32_t CARRIER_FREQ = 7000000;

// Data Mode
#define RTTY_300    0
#define DOMINOEX8   1

int data_mode = 0;


char tx_buffer[256];
volatile int tx_flag = 0;

float lat, lon;
long altitude;
int sats, txCount, intTemp, extTemp, _intTemp, _extTemp, speed;
char latString[12], longString[12],voltString[12];
int time[3];

//gps_type Gps;

// tinyGPS object
TinyGPS gps;

void TX_Setup(){
    switch(data_mode){
        case 0:
            RTTY_Setup(CARRIER_FREQ, 425, 300, 1);
            break;
        case 1:
            Domino_Setup(CARRIER_FREQ,8);
            break;
        default:
            break;
    }
}

void TXString(char *string){
    switch(data_mode){
        case 0:
            RTTY_TXString(string);
            break;
        case 1:
            Domino_TXString(string);
            break;
        default:
            break;
    }
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

uint16_t readADC(){
    uint16_t ADC_result = 0;
    int8_t offset;
 	/* Move stored calibration values to ADC B */
	ADC_CalibrationValues_Load(&ADCA);
   
    /* Set up ADC B to have signed conversion mode and 12 bit resolution. */
  	ADC_ConvMode_and_Resolution_Config(&ADCA, true, ADC_RESOLUTION_12BIT_gc);

    // The ADC has different voltage reference options, controlled by the REFSEL bits in the
    // REFCTRL register. Here the internal reference is selected 
	ADC_Reference_Config(&ADCA, ADC_REFSEL_VCC_gc);

	// The clock into the ADC decide the maximum sample rate and the conversion time, and
    // this is controlled by the PRESCALER bits in the PRESCALER register. Here, the
    // Peripheral Clock is divided by 8 ( gives 250 KSPS with 2Mhz clock )
	ADC_Prescaler_Config(&ADCA, ADC_PRESCALER_DIV8_gc);

    // The used Virtual Channel (CH0) must be set in the correct mode
    // In this task we will use single ended input, so this mode is selected 
  
    /* Setup channel 0 to have single ended input. */
	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH0,
	                                 ADC_CH_INPUTMODE_DIFF_gc,
                                     ADC_CH_GAIN_1X_gc);
     // Setting up the which pins to convert.
    // Note that the negative pin is internally connected to ground  
	ADC_Ch_InputMux_Config(&ADCA.CH0, ADC_CH_MUXPOS_PIN0_gc, ADC_CH_MUXNEG_PIN1_gc);
  

    // Before the ADC can be used it must be enabled 
	ADC_Enable(&ADCA);


    // Wait until the ADC is ready
    ADC_Wait_32MHz(&ADCA);

    // In the while(1) loop, a conversion is started on CH0 and the 8 MSB of the result is
    // ouput on the LEDPORT when the conversion is done   

	/* Get offset value for ADC B.  */
	offset = ADC_Offset_Get_Unsigned(&ADCA, &(ADCA.CH0), true);  
	
	for(int i = 0; i<5; i++){
	
         ADC_Ch_Conversion_Start(&ADCA.CH0);
           
         while(!ADC_Ch_Conversion_Complete(&ADCA.CH0));
                
            //ADCB.INTFLAGS = ADC_CH0IF_bm;     // Clear CH0IF by writing a one to it
         ADC_result += ADCA.CH0RES;// - offset;
    }
    
    return ADC_result/5;
}

int main(void) {    
    
    Config32MHzClock(); // Setup the 32MHz Clock. Should really be using 2MHz...
    
    // Setup output and input ports.
    LEDPORT.DIRSET = 0xFF; 
    LEDPORT.OUT = 0xFF;
    AD9835_PORT.DIRCLR = 0x40;
    PORTC.DIRSET = 0x04;

    // Start up the timer.
    init_timer();
    
    // Wait a bit before starting the AD9835.
    // It seems to take a few hundred ms to 'boot up' once power is applied.
    _delay_ms(500); 
    
    // Configure the AD9835, and start in sleep mode.
    AD9835_Setup();
    AD9835_Sleep();
    
    // Setup the AD9835 for our chosen datamode.
    TX_Setup();
    AD9835_Awake();
    
    // Broadcast a bit of carrier.
    _delay_ms(1000);
    
    TXString("Starting up...\n"); // Kind of like debug lines.
    
    // Start up the GPS RX UART.
    init_gps();
    
    TXString("GPS Active\n");
    
    // Turn Interrupts on.
    PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_LOLVLEN_bm;
    sei();
    
    TXString("Interrupts On.\n");
    
    unsigned int counter = 0; // Init out TX counter.
 
    while(1){
        // Identify every few minutes
        if (counter%200 == 0) TXString("DE VK5VZI Project Horus Launch projecthorus.org \n");
    
    
        // Collect GPS data
        gps.f_get_position(&lat, &lon);
	    sats = gps.sats();
	    altitude = (long)gps.f_altitude();
	    gps.crack_datetime(0, 0, 0, &time[0], &time[1], &time[2]);
	    
	    floatToString(lat, 5, latString);
	    floatToString(lon, 5, longString);
	    
	    
	    // Read ADC PortA pin 0, using differential, signed input mode. Negative input comes from pin 1, which is tied to ground. Use VCC/1.6 as ref.
	    uint16_t temp = readADC(); 
	    float bat_voltage = (float)temp * 0.001007572056668* 8.5;
        floatToString(bat_voltage,1,voltString);
        
        // Construct our Data String
        sprintf(tx_buffer,"$$DARKSIDE,%u,%02d:%02d:%02d,%s,%s,%ld,%d,%s",counter++,time[0], time[1], time[2],latString,longString,altitude,sats,voltString);
        
        // Calculate the CRC-16 Checksum
        char checksum[10];
        snprintf(checksum, sizeof(checksum), "*%04X\n", gps_CRC16_checksum(tx_buffer));
     
        // And copy the checksum onto the end of the string.
        memcpy(tx_buffer + strlen(tx_buffer), checksum, strlen(checksum) + 1);
        
        // Blinky blinky...
        LEDPORT.OUTTGL = 0x01;
        
        // Transmit!
        TXString(tx_buffer);
        
        // And wait a little while before sending the next string.
        _delay_ms(1000);
        
        sendNMEA("$PUBX,00"); // Poll the UBlox5 Chip for data.
    }

    
}
    
    
        
    