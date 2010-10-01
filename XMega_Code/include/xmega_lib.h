#ifndef XMEGA_LIB_H
#define XMEGA_LIB_H 1

#include <stdio.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/sleep.h>


void DoDAC_B(unsigned int ch, unsigned int value, unsigned int ref);

void Config32MHzClock(void);
void Config2MHzClock(void);
void Config32KHzClock(void);
int DoOutput(char port, unsigned int pin, unsigned int val);
int DoInput(char port, unsigned int pin);
int DoADC_A(unsigned int pos, unsigned int neg, unsigned int sign, unsigned int ref);
int DoADC_B(unsigned int pos, unsigned int neg, unsigned int sign, unsigned int ref);
void Setup_PortC_Usart();
void UsartCWriteChar(unsigned char data);
unsigned char UsartCReadChar(void);
void UsartCWriteString(char *string);
void UsartCWriteLine(char *string);
void Setup32KHzRTC(int DELAY);
uint8_t ReadCalibrationByte( uint8_t index );

#endif