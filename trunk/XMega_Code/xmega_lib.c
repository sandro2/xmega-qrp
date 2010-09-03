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
  USARTF0.BAUDCTRLA = 207; // 9600b  (BSCALE=207,BSEL=0)
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
    USARTF0.BAUDCTRLA = 12; // 9600b  (BSCALE=13,BSEL=0)
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

void Setup_PortF_Usart(){
    PORTF.DIR |= (1<<3) | (1<<0); // set PORTF:3 transmit pin as output
    PORTF.OUT |= (1<<3);          // set PORTF:3 hi 
    USARTF0.BAUDCTRLA = 207; // 9600b  (BSCALE=207,BSEL=0)
    
    USARTF0.CTRLB = USART_TXEN_bm | USART_RXEN_bm; // enable tx and rx on USART
    
};

void UsartWriteChar(unsigned char data)
{
    USARTF0.DATA = data; // transmit ascii 3 over and over
	if(!(USARTF0.STATUS&USART_DREIF_bm))
		while(!(USARTF0.STATUS & USART_TXCIF_bm)); // wait for TX complete
  	USARTF0.STATUS |= USART_TXCIF_bm;  // clear TX interrupt flag
};

unsigned char UsartReadChar(void)
{
	while(!(USARTF0.STATUS&USART_RXCIF_bm));  // wait for RX complete

  	return USARTF0.DATA;
};

// write out a simple '\0' terminated string
void UsartWriteString(char *string)
{
    while(*string != 0)
	  UsartWriteChar(*string++);
};

// write out a simple '\0' terminated string and print "\n\r" at end
void UsartWriteLine(char *string)
{
   UsartWriteString(string);
   UsartWriteString("\n\r");

};