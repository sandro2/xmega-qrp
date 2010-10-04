/*
    XMega-QRP - Software to drive the XMega-QRP HF Telemetry transmitter.
    The purpose of this code is to work as a telemetry transmitter for a High 
    Altitude Ballooning project - Project Horus. The associated libraries 
    provide the framework to use the transmitter for many other purposes.
    

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
#include "include/MOD_Morse.h"
#include "include/MOD_RTTY.h"
#include "include/MOD_DOMINO.h"
#include "include/TIMER.h"
#include "include/TinyGPS.h"
#include "include/GPS2.h"
#include "include/OneWire.h"
#include "include/DallasTemperature.h"



#define LEDPORT     PORTE
#define SWITCHPORT  PORTF
#define AD9835_PORT PORTD

// Carrier Frequency Setting
uint32_t CARRIER_FREQ = 1000;//7025000;

// Data Modes
#define RTTY_300    0
#define DOMINOEX8   1
#define QRSS        2 
#define RELIABLE_MODE    1 // DominoEX8 is our 'reliable' mode. Better than RTTY300 at least.
#define FALLBACK    2   // Our QRSS mode, in case the battery really goes to hell.
int data_mode = 0;

#define BATT_THRESHOLD  9.0
#define BATT_THRESHOLD_2  7.0


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

// Temperature Sensor Stuff
OneWire oneWire;
DallasTemperature sensors(&oneWire);

// Internal sensor on prototype board
uint8_t internal[] = {0x28, 0xE4, 0xEF, 0x54, 0x02, 0x00, 0x00, 0xC7};



void TX_Setup(){
    switch(data_mode){
        case 0:
            RTTY_Setup(CARRIER_FREQ, 425, 300, 1);
            break;
        case 1:
            Domino_Setup(CARRIER_FREQ,8);
            break;
        case 2:
            Morse_Setup(5,CARRIER_FREQ); // Sloooooooooq QRSS fallback mode.
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
            Domino_TXString("$$$"); // Try and help sync the receiver.
            Domino_TXString(string);
            break;
        case 2:
            // Probably shouldn't try and TX long strings in this mode...
            Morse_TXString(string);
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
    
    sensors.begin();
    sensors.requestTemperatures();
    
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
    
    TXString("Booting up...\n"); // Kind of like debug lines.
    
    // Start up the GPS RX UART.
    init_gps();
    
    // Turn Interrupts on.
    PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_LOLVLEN_bm;
    sei();
    
    sendNMEA("$PUBX,00"); // Poll the UBlox5 Chip for data.
    
    TXString("GPS Active, Interrupts On.\n");
    
    int found_sensors = sensors.getDeviceCount();
    
    sprintf(tx_buffer,"Found %u sensors.\n",found_sensors);
    TXString(tx_buffer);
    unsigned int counter = 0; // Init out TX counter.
    
    int new_mode = -1;
 
    while(1){
        // Identify every few minutes
        if (counter%10 == 0) TXString("DE VK5VZI Project Horus Telemetry Test \n");
    
	    // Read ADC PortA pin 0, using differential, signed input mode. Negative input comes from pin 1, which is tied to ground. Use VCC/1.6 as ref.
	    uint16_t temp = readADC(); 
	    float bat_voltage = (float)temp * 0.001007572056668* 8.5;
        floatToString(bat_voltage,1,voltString);
   
    
    
        // Collect GPS data
        
        // NOTE - NEED TO ADD IN SPEED.
        gps.f_get_position(&lat, &lon);
	    sats = gps.sats();
	    altitude = (long)gps.f_altitude();
	    gps.crack_datetime(0, 0, 0, &time[0], &time[1], &time[2]);
	    
	    floatToString(lat, 5, latString);
	    floatToString(lon, 5, longString);
	    
        sensors.requestTemperatures();
        _intTemp = sensors.getTempC(internal);
        //_extTemp = sensors.getTempC(external);
        if (_intTemp!=85 && _intTemp!=127 && _intTemp!=-127 && _intTemp!=999) intTemp = _intTemp;
        //if (_extTemp!=85 && _extTemp!=127 && _extTemp!=-127 && _extTemp!=999) extTemp = _extTemp;
	    
	    if(data_mode != FALLBACK){
	    
            // Construct our Data String
            sprintf(tx_buffer,"$$DARKSIDE,%u,%02d:%02d:%02d,%s,%s,%ld,%d,%d,%s",counter++,time[0], time[1], time[2],latString,longString,altitude,sats,intTemp,voltString);
            
            
            // Calculate the CRC-16 Checksum
            char checksum[10];
            snprintf(checksum, sizeof(checksum), "*%04X\n", gps_CRC16_checksum(tx_buffer));
         
            // And copy the checksum onto the end of the string.
            memcpy(tx_buffer + strlen(tx_buffer), checksum, strlen(checksum) + 1);
        }else{
            // If our battery is really low, we don't want to transmit much data, so limit what we TX to just an identifier, battery voltage, and our position.
            
            sprintf(tx_buffer, "DARKSIDE %s %s %s %ld", bat_voltage, latString, longString,altitude);
        }
        
        // Blinky blinky...
        LEDPORT.OUTTGL = 0x01;
        
        // Transmit!
        TXString(tx_buffer);
       
        
        sendNMEA("$PUBX,00"); // Poll the UBlox5 Chip for data again.
        
        /*
        // Check the battery voltage. If low, switch to a more reliable mode.
        if((bat_voltage < BATT_THRESHOLD) && (data_mode != RELIABLE_MODE)){
            new_mode = RELIABLE_MODE;
            // This string should be changed if the 'reliable' mode is changed.
            TXString("Battery Voltage Below 9V. Switching to DominoEX8.\n");
        }
        */
        // Perform a mode switch, if required. 
        // Done here to allow for mode changes to occur elsewhere.
        if(new_mode != -1){
            data_mode = new_mode;
            TX_Setup();
            new_mode = -1;
        }
        
        // And wait a little while before sending the next string.
        _delay_ms(1000);
        
        
    }

    
}
