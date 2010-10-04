/*
    Morse Transmission Library
    Date: 10-08-2010
    Author: Mark Jessop

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
#include <string.h>
#include "AD9835_Xmega.h"
#include "MOD_Morse.h"

int MORSE_WPM = 20;

char* morseCode[] = {  // 0 = dit, 1 = dash
  "2",     // Dummy entry for non-ascii characters
  "01",    // A     1
  "1000",  // B
  "1010",  // C
  "100",   // D
  "0",     // E
  "0010",  // F
  "110",   // G
  "0000",  // H
  "00",    // I
  "0111",  // J
  "101",   // K
  "0100",  // L
  "11",    // M
  "10",    // N
  "111",   // O
  "0110",  // P
  "1101",  // Q
  "010",   // R
  "000",   // S
  "1"  ,   // T
  "001",   // U
  "0001",  // V
  "011" ,  // W
  "1001",  // X
  "1011",  // Y
  "1100",  // Z  26
  "11111", // 0
  "01111", // 1
  "00111", // 2
  "00011", // 3
  "00001", // 4
  "00000", // 5
  "10000", // 6
  "11000", // 7
  "11100", // 8
  "11110",  // 9  36
  "010101", // .  37
  "110011", // ,
  "001100", // ?
  "011110", // '
  "101011", // !
  "10010", // /
  "10110", // (
  "101101", // )
  "01000", // &
  "111000", // :
  "101010", // ;
  "10001", // =
  "01010", // +
  "100001", // - (minus)
  "001101", // _ (underscore)
  "010010", // "
  "0001001", // $
  "011010" // @
};

void Morse_Setup(uint16_t WPM, uint32_t carrier_freq){
    MORSE_WPM = WPM;

   AD9835_Setup();
   AD9835_UseFSEL(0);
   AD9835_SelectFREG(1);
   AD9835_SetFreq(carrier_freq);
   AD9835_SelectFREG(0);
   AD9835_SetFreq(0); // Using '0'Hz makes the AD9835's output voltage sit around 1.5V, instead of 0V.
   AD9835_UseFSEL(1);
     
}

void Morse_DelayUnit(int number){
  _delay_ms(number*1200/MORSE_WPM);
}

void Morse_SendDit(){
  AD9835_SelectFREG(0);
  AD9835_SelectFREG(1);
  Morse_DelayUnit(1);
  AD9835_SelectFREG(0);
}

void Morse_SendDash(){
  AD9835_SelectFREG(0);
  AD9835_SelectFREG(1);
  Morse_DelayUnit(3);
  AD9835_SelectFREG(0);
}

void Morse_SendLetter(char letter){
  int morse_index = 0;
  // Work out if the character is a letter, a number, or a symbol character.
  
  // Numbers
  if((letter>=48) && (letter<=57)){morse_index = letter - 21;}
  // Uppercase Letters
  else if((letter>=65) && (letter<=90)){morse_index = letter - 64;}
  // Lowercase Letters
  else if((letter>=97) && (letter<=122)){morse_index = letter - 96;}
  // Symbols
  else if(letter == '.'){ morse_index = 37;}
  else if(letter == ','){ morse_index = 38;}
  else if(letter == '?'){ morse_index = 39;}
  else if(letter == '\''){ morse_index = 40;}
  else if(letter == '!'){ morse_index = 41;}
  else if(letter == '/'){ morse_index = 42;}
  else if(letter == '('){ morse_index = 43;}
  else if(letter == ')'){ morse_index = 44;}
  else if(letter == '&'){ morse_index = 45;}
  else if(letter == ':'){ morse_index = 46;}
  else if(letter == ';'){ morse_index = 47;}
  else if(letter == '='){ morse_index = 48;}
  else if(letter == '+'){ morse_index = 49;}
  else if(letter == '-'){ morse_index = 50;}
  else if(letter == '_'){ morse_index = 51;}
  else if(letter == '"'){ morse_index = 52;}
  else if(letter == '$'){ morse_index = 53;}
  else if(letter == '@'){ morse_index = 54;}
  // And everything else is treated as a space
  else{morse_index = 0;}
  
  // Now we have the index to the morse conversion array, we can send the char.
  //char temp[] = morseCode[morse_index];
  int index = 0;
  while(morseCode[morse_index][index] != 0){
    if(morseCode[morse_index][index] == '0'){
        Morse_SendDit();
    }
    else if(morseCode[morse_index][index] == '1'){Morse_SendDash();}
    else if(morseCode[morse_index][index] == '2'){Morse_DelayUnit(6);}
    Morse_DelayUnit(1);
    index++;
  }
}

void Morse_TXString(char *string){

	for (int i = 0; i < strlen(string); i++) {
        Morse_SendLetter(string[i]);
        Morse_DelayUnit(3);
    }
}