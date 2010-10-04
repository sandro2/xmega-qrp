/*
    Atmel XMega Helper Library
    
    Adapted from code written for Boston Android's XMega Boards.
    
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


#include "xmega_lib.h"

/*
 Outputs a 12-bit value on the PORT C DAC
 ch = DAC Channel (0 or 1)
 ref = Voltage Reference (0=1V,1=AVCC,2=AREFA,3=AREFB)
*/
void DoDAC_B(unsigned int ch, unsigned int value, unsigned int ref)
{
  if(ref<4)
    DACB.CTRLC = (ref << 3);           // select reference 
  else return; // early return for bad parameter

  DACB.CTRLB = DAC_CHSEL_DUAL_gc;      // select dual output DAC
  if(ch==0)
  {
	  DACB.CTRLA = (1<<2) | (1<<0);    // enable DACB, CH0
	  DACB.CH0DATA = value;
  }
  if(ch==1)
  {
      DACB.CTRLA |= (1<<3) | (1<<0);   // enable DACB, CH1
      DACB.CH1DATA = value;
  };

};

void Config32MHzClock(void)
{
  CCP = CCP_IOREG_gc; //Security Signature to modify clock 
  // initialize clock source to be 32MHz internal oscillator (no PLL)
  OSC.CTRL = OSC_RC32MEN_bm; // enable internal 32MHz oscillator
  while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator ready
  CCP = CCP_IOREG_gc; //Security Signature to modify clock 
  CLK.CTRL = CLK_SCLKSEL_RC32M_gc; //select sysclock 32MHz osc
// update baud rate control to match new clk
  //USARTF0.BAUDCTRLA = 207; // 9600b  (BSCALE=207,BSEL=0)
};

void Config2MHzClock(void)
{
  CCP = CCP_IOREG_gc; //Security Signature to modify clock 
  // initialize clock source to be 32MHz internal oscillator (no PLL)
  OSC.CTRL = OSC_RC2MEN_bm; // enable internal 32MHz oscillator
  while(!(OSC.STATUS & OSC_RC2MRDY_bm)); // wait for oscillator ready
  CCP = CCP_IOREG_gc; //Security Signature to modify clock 
  CLK.CTRL = CLK_SCLKSEL_RC2M_gc; //select sysclock 32MHz osc
// update baud rate control to match new clk
    //USARTF0.BAUDCTRLA = 12; // 9600b  (BSCALE=13,BSEL=0)
};

void Config32KHzClock(void)
{
  CCP = CCP_IOREG_gc; //Security Signature to modify clock 
  // initialize clock source to be 32KHz internal oscillator (no PLL)
  OSC.CTRL = OSC_RC32KEN_bm; // enable internal 32MHz oscillator
  while(!(OSC.STATUS & OSC_RC32KRDY_bm)); // wait for oscillator ready
  CCP = CCP_IOREG_gc; //Security Signature to modify clock 
  CLK.CTRL = CLK_SCLKSEL_RC32K_gc; //select sysclock 32MHz osc
// serial port doesn't work at this clk speed so demo program will stop
};
/*
void Setup32KHzRTC(int DELAY){
    // Configure RTC
	// Set Internal 32kHz oscillator prescaled to 1kHz as source for RTC
	CLKSYS_RTC_ClockSource_Enable( CLK_RTCSRC_TOSC32_gc );
	// Enable Low level interrupts for the RTC Overflow
	RTC.INTCTRL = ( RTC.INTCTRL & ~RTC_OVFINTLVL_gm) | RTC_OVFINTLVL_LO_gc;

	// Enable low level interrupts in the programmable interrupt controller
	PMIC.CTRL |= PMIC_LOLVLEN_bm;

	// Set initial period for RTC and start the RTC with no prescaler
	RTC.PER = DELAY;
	RTC.CTRL |= RTC_PRESCALER_DIV1_gc;
}
*/ 

int DoOutput(char port, unsigned int pin, unsigned int val)
{
  PORT_t *Port;

  switch(port)
  {
    case 'a':
	  Port = (PORT_t*)_SFR_IO_ADDR(PORTA);
	  break;
    case 'b':
	  Port = (PORT_t*)_SFR_IO_ADDR(PORTB);
	  break;
    case 'c':
	  Port = (PORT_t*)_SFR_IO_ADDR(PORTC);
	  break;
    case 'd':
	  Port = (PORT_t*)_SFR_IO_ADDR(PORTD);
	  break;
    case 'e':
	  Port = (PORT_t*)_SFR_IO_ADDR(PORTE);
	  break;
    case 'f':
	  Port = (PORT_t*)_SFR_IO_ADDR(PORTF);
	  break;
    default:
	  return -1; // if no valid port, return
  };
};
  
  int DoInput(char port, unsigned int pin)
{
  PORT_t *Port;

  switch(port)
  {
    case 'a':
	  Port = (PORT_t*)_SFR_IO_ADDR(PORTA);
	  break;
    case 'b':
	  Port = (PORT_t*)_SFR_IO_ADDR(PORTB);
	  break;
    case 'c':
	  Port = (PORT_t*)_SFR_IO_ADDR(PORTC);
	  break;
    case 'd':
	  Port = (PORT_t*)_SFR_IO_ADDR(PORTD);
	  break;
    case 'e':
	  Port = (PORT_t*)_SFR_IO_ADDR(PORTE);
	  break;
    case 'f':
	  Port = (PORT_t*)_SFR_IO_ADDR(PORTF);
	  break;
    default:
	  return -1; // if no valid port, return
  };

  if(pin > 7) { return -1; };

  Port->DIR &= ~(1<<pin);  // configure port as input
  if(Port->IN & (1<<pin))
    return 1;
  else
    return 0;

};

int DoADC_A(unsigned int pos, unsigned int neg, unsigned int sign, unsigned int ref)
{
  int result;

  ADCA.CTRLA = 0x01; // enable ADC circuit
  ADCA.PRESCALER = ADC_PRESCALER_DIV128_gc;
  ADCA.CH0.MUXCTRL = (pos << ADC_CH_MUXPOS_gp) |  (neg << ADC_CH_MUXNEG_gp);

  if(sign==1) // implicit differential selection
  {
    ADCA.CTRLB = 0x10; // set conversion mode signed
    ADCA.CH0.CTRL = ADC_CH_INPUTMODE_DIFF_gc;
  }
  else
  {
    ADCA.CTRLB = 0x00; // set conversion mode unsigned
    ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;

  };

  // clear any old interrupt flags
  ADCA.CH0.INTFLAGS |= 1;

  ADCA.CTRLA |= 0x4;   // start conversion on channel 0

  while(!ADCA.CH0.INTFLAGS);  // wait for conversion complete
  
  result = ADCA.CH0RES;

  return result;
};

int DoADC_B(unsigned int pos, unsigned int neg, unsigned int sign, unsigned int ref)
{
  int result;

  ADCB.CTRLA = 0x01; // enable ADC circuit
  ADCB.PRESCALER = ADC_PRESCALER_DIV128_gc;
  ADCB.CH0.MUXCTRL = (pos << ADC_CH_MUXPOS_gp) |  (neg << ADC_CH_MUXNEG_gp);

  if(sign==1) // implicit differential selection
  {
    ADCB.CTRLB = 0x10; // set conversion mode signed
    ADCB.CH0.CTRL = ADC_CH_INPUTMODE_DIFF_gc;
  }
  else
  {
    ADCB.CTRLB = 0x00; // set conversion mode unsigned
    ADCB.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;

  };

  // clear any old interrupt flags
  ADCB.CH0.INTFLAGS |= 1;

  ADCB.CTRLA |= 0x4;   // start conversion on channel 0

  while(!ADCB.CH0.INTFLAGS);  // wait for conversion complete
  
  result = ADCB.CH0RES;

  return result;
};

void Setup_PortC_Usart(){
    PORTC.DIR |= (1<<3) | (1<<0); // set PORTF:3 transmit pin as output
    PORTC.OUT |= (1<<3);          // set PORTF:3 hi 
    USARTC0.BAUDCTRLA = 207; // 9600b  (BSCALE=207,BSEL=0)
    
    USARTC0.CTRLB = USART_TXEN_bm | USART_RXEN_bm; // enable tx and rx on USART 
};

void UsartCWriteChar(unsigned char data)
{
    USARTC0.DATA = data; // transmit ascii 3 over and over
	if(!(USARTC0.STATUS&USART_DREIF_bm))
		while(!(USARTC0.STATUS & USART_TXCIF_bm)); // wait for TX complete
  	USARTC0.STATUS |= USART_TXCIF_bm;  // clear TX interrupt flag
};

unsigned char UsartCReadChar(void)
{
	while(!(USARTC0.STATUS&USART_RXCIF_bm));  // wait for RX complete

  	return USARTC0.DATA;
};

// write out a simple '\0' terminated string
void UsartCWriteString(char *string)
{
    while(*string != 0)
	  UsartCWriteChar(*string++);
};

// write out a simple '\0' terminated string and print "\n\r" at end
void UsartCWriteLine(char *string)
{
   UsartCWriteString(string);
   UsartCWriteString("\n\r");

};

uint8_t ReadCalibrationByte( uint8_t index ) 
{ 
uint8_t result; 

/* Load the NVM Command register to read the calibration row. */ 
NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc; 
result = pgm_read_byte(index); 

/* Clean up NVM Command register. */ 
NVM_CMD = NVM_CMD_NO_OPERATION_gc; 

return( result ); 
} 