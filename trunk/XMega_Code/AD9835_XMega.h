// AD9835 Driver Includes

#ifndef AD9835_XMEGA_H
#define AD9835_XMEGA_H 1

#define SPI_NUM_BYTES     2

#define SDATA   PIN5_bm
#define SCLK    PIN7_bm
#define FSYNC 	PIN4_bm
#define FSEL 	PIN3_bm
#define PSEL	PIN2_bm
#define AD_SLEEP	PIN1_bm
#define AD_RESET	PIN0_bm

void AD9835_SendWord(uint16_t data_word);
void AD9835_Setup( );
void AD9835_SetFreq(uint32_t freq);
void AD9835_Sleep();
void AD9835_Awake();
void AD9835_UseFSEL(int c);
void AD9835_SelectFREG(int reg_number);

#endif