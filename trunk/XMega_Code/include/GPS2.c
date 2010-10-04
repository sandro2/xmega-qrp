/*
    GPS Helper Library
    Adapted from code written by Terry Baume for Project Horus.
    
    
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
#include "GPS2.h"
#include <string.h>
#include <stdio.h> 

//extern gps_type Gps;

void init_gps(){
    GPSPORT.DIRSET = 0x80; // Set PD7 as an output, for talking to the GPS
    GPSPORT.OUTSET = 0x80; // And set it high.
    
    // Configure the GPS's USART
    GPSUART.CTRLA = USART_RXCINTLVL_HI_gc;
    GPSUART.CTRLC = USART_CHSIZE_8BIT_gc;
    GPSUART.BAUDCTRLA = 207; // 9600 Baud when using a 32MHz Clock.
    
    GPSUART.CTRLB = USART_TXEN_bm;
    
    // Set the UBlox5 Chip into flight mode.
    uint8_t setNav[] = {0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xDC};
	sendUBX(setNav, sizeof(setNav)/sizeof(uint8_t));
    
    // Disable un-neccesary NMEA sentences (all of them!)
    sendNMEA("$PUBX,40,RMC,0,0,0,0");
    sendNMEA("$PUBX,40,VTG,0,0,0,0");
    sendNMEA("$PUBX,40,GGA,0,0,0,0");
    sendNMEA("$PUBX,40,GSA,0,0,0,0");
    sendNMEA("$PUBX,40,GSV,0,0,0,0");
    sendNMEA("$PUBX,40,ZDA,0,0,0,0");
    sendNMEA("$PUBX,40,GLL,0,0,0,0");
    
    
    // Enable UART RX
    GPSUART.CTRLB = USART_RXEN_bm|USART_TXEN_bm;
}

uint8_t gps_xor_checksum(char *string)
{
	uint8_t i;
	uint8_t XOR;
	uint8_t c;
 
	XOR = 0;
 
	// Calculate checksum ignoring the first two $s
	for (i = 1; i < strlen(string); i++)
	{
		c = string[i];
		XOR ^= c;
	}
 
	return XOR;
}

void sendNMEA(char *string){

char checksum[10];
sprintf(checksum,"*%02X",gps_xor_checksum(string));

char output[128];

sprintf(output,"%s%s\n\r",string,checksum);

GPSWriteString(output);

//RTTY_TXString(output);


}

void sendUBX(uint8_t *MSG, uint8_t len) {
	for(int i=0; i<len; i++) {
		GPSWriteChar(MSG[i]);
	}
}
/*
ISR(USARTD1_RXC_vect)				//UART interrupt on mega xx8 series
{
	static char buffer[6];
	static u08 GGA;
	static u08 RMC;
	static u08 stage;
	static u08 commacount;
	static u08 bufferindex;
	static u08 pointcount;
	static gps_type gps;
	char c=GPSUART.DATA;
	PORTE.OUTTGL = 0x08;
	switch(c)
	{
		case '$':			//start of a packet
		  commacount=0;
		  GGA=FALSE;
		  RMC=FALSE;			//we dont know what sort of packet it is yet
		case ',':			//we dont break as the code is shared
		  commacount++;			//note this means packet header is 1
		  bufferindex=0;		//wipe all these so they can be reused
		  pointcount=0;
		  stage=FALSE;
		  memset(buffer,' ',6);
		break;
		case '.':
		  pointcount++;			//we need to be able to detect number of points in the case of altitude
		break;                      
		default:			//we have some of the CSV data
		  if(bufferindex<6)		//dont mess up ! Dont overflow
		  {									
		    	buffer[bufferindex]=c;	//stick the character in our buffer		
		  }
		  if(GGA)
		  {
			switch(commacount)
			{			
				case 3:		//the latitude from the GGA
				  if( (bufferindex<1  && !stage) || bufferindex<5)
				  {
					bufferindex++;
				  }
				  else
				  {
					if(!stage)
					{				
						gps.latitude=(float)atoi(buffer);		// degrees
						stage=TRUE;
					}
					else
					{
						gps.latitude+=minutes*(float)atoi(buffer);	// minutes
					}
				  }
				break;
				case 4:
				  if(c=='S')
				  {
					gps.latitude=-gps.latitude;
				  }
				break;
				case 5:
				  if( (bufferindex<1 && !stage) || bufferindex<5)
				  {
					bufferindex++;
				  }
				  else
				  {
					if(!stage)
					{				
						gps.longitude=(float)atoi(buffer);		// degrees
						stage=TRUE;
					}
					else
					{
						gps.longitude+=minutes*(float)atoi(buffer);	// minutes
					}
				  }
				break;
				case 6:
				  if(c=='W')
				  {
					gps.longitude=-gps.longitude;
				  }
				break;
				case 7:
				  gps.status=atoi(&c);
				break;
				case 10:
				  if(!pointcount)				//wait until we get to a decimal point		
				  {
					bufferindex++;
				  }
				  else
				  {
					gps.altitude=(float)atoi(buffer)*0.1;	//last char in buffer will be after dp
				  }
			}
		  }
		  else if(RMC)
		  {
			if(commacount==8)		//speed in knots
			{		
				if(!pointcount)
				{
					bufferindex++;
				}
				else
				{
					gps.speed=(float)atoi(buffer)*0.1;
				}
			}	
			if(commacount==9)		//the heading
			{
 
				if(!pointcount)
				{
					bufferindex++;
				}
				else
				{
					gps.heading=(float)atoi(buffer)*0.1;
					//toggle_pin;		//toggles pin D5 - flashing LED
					if(!Gps.packetflag)	//main has unlocked the data
					{
						Gps.packetflag=TRUE;	//this is usually the last interesting part of the fix info to come through
						Gps=gps;		//copy into the global variable
						PORTE.OUTTGL = 0x04;
					}
				}
			}
		  }
		  else if(!commacount)			//the header
		  {		
			if(bufferindex<4)
			{
				bufferindex++;		//increase the position in the buffer
			}
			else
			{
				if(strcmp(buffer,"GPGGA ")==0)	//the last character will be a space
				{
					GGA=TRUE;
					PORTE.OUTTGL = 0x02;
				}
				if(strcmp(buffer,"GPRMC ")==0)
				{
					RMC=TRUE;
				}
			}
		 }	
	}
}
*/
void GPSWriteChar(unsigned char data)
{
    USARTD1.DATA = data;
	if(!(USARTD1.STATUS&USART_DREIF_bm))
		while(!(USARTD1.STATUS & USART_TXCIF_bm)); // wait for TX complete
  	USARTD1.STATUS |= USART_TXCIF_bm;  // clear TX interrupt flag
};

// write out a simple '\0' terminated string
void GPSWriteString(char *string)
{
    while(*string != 0)
	  GPSWriteChar(*string++);
};

// write out a simple '\0' terminated string and print "\n\r" at end
void GPSWriteLine(char *string)
{
   GPSUART.CTRLB = USART_TXEN_bm;
   GPSWriteString(string);
   GPSWriteString("\n\r");
   GPSUART.CTRLB = ~USART_TXEN_bm;
};
