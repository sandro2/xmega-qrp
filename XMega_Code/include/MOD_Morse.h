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

#ifndef MOD_MORSE_H
#define MOD_MORSE_H 1

void Morse_Setup(uint16_t WPM, uint32_t carrier_freq);
void Morse_DelayUnit(int number);
void Morse_SendDit();
void Morse_SendDash();
void Morse_SendLetter(char letter);
void Morse_TXString(char text[]);

#endif