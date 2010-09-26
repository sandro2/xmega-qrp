#ifndef MOD_DOMINO_H
#define MOD_DOMINO_H 1

#define NUM_TONES 18
#define TONE_SHIFT 36
#define DOMINO_DELAY 1484

void Domino_Setup(uint32_t base_freq, int mode);
static void dominoex_sendtone(uint8_t tone);
static void dominoex_sendsymbol(uint8_t sym);
void transmit_dominoex_character(uint8_t c);
void Domino_TXString(char *string);

#endif