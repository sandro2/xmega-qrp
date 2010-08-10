#ifndef MOD_MORSE_H
#define MOD_MORSE_H 1

void Morse_Setup(uint16_t WPM, uint32_t carrier_freq);
void Morse_DelayUnit(int number);
void Morse_SendDit();
void Morse_SendDash();
void Morse_SendLetter(char letter);
void Morse_TXString(char text[]);

#endif