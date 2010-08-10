#ifndef MOD_RTTY_H
#define MOD_RTTY_H 1

void RTTY_Setup(uint32_t base_freq, uint32_t shift_freq, uint16_t baud_rate);
void RTTY_TXBit(int bit_val);
void RTTY_Delay();
void RTTY_TXByte(uint8_t data);
void RTTY_TXString(char *string);

#endif