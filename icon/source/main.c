/*
 * GccApplication1.c
 *
 * Created: 7/6/2019 6:36:32 PM
 * Author : Xia Hua
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <stdlib.h>
#include "bit.h"
#include "timer.h"
#include "scheduler.h"
#include "keypad.h"
#include "io.h"
#include "io.c"
//#include <stdio.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


void customLCD()//custom character
{
	char i;
	//Custom character dictionary
	unsigned char Character1[8] = { 4, 10, 5, 31, 20, 6, 10, 18 };  //the character icon
	unsigned char Character2[8] = { 31, 31, 31, 31, 31, 31, 31, 31 }; //the block icon
	unsigned char Character3[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; //undefined
	unsigned char Character4[8] = { 0x01, 0x03, 0x07, 0x1F, 0x1F, 0x07, 0x03, 0x01 }; //undefined
	unsigned char Character5[8] = { 4, 4, 14, 21, 31, 14, 10, 21}; //the boss icon
	unsigned char Character6[8] = { 31, 21, 21, 31, 17, 21, 31,0 }; // the monster icon 
	unsigned char Character7[8] = { 0, 0, 10, 0, 4, 17, 14, 0 }; // the smile icon
	unsigned char Character8[8] = { 0, 10, 31, 31, 14, 4, 0, 0 }; // the heart icon

	LCD_Init();
	
	LCD_Custom_Char(0, Character1);  /* Build Character1 at position 0 */
	LCD_Custom_Char(1, Character2);  /* Build Character2 at position 1 */
       	LCD_Custom_Char(2, Character2);  /* Build Character3 at position 2 */ //changed to undefined 
	LCD_Custom_Char(3, Character2);  /* Build Character4 at position 3 */ //changed to undefined
	LCD_Custom_Char(4, Character5);  /* Build Character5 at position 4 */
	LCD_Custom_Char(5, Character6);  /* Build Character6 at position 5 */
	LCD_Custom_Char(6, Character7);  /* Build Character6 at position 6 */
	LCD_Custom_Char(7, Character8);  /* Build Character6 at position 7 */
}


int main() {
	/* Configure the ports as output */
	DDRA = 0xF0; PORTA = 0x0F; // KeyPad Entry
	DDRB = 0xFF; PORTB = 0x00; // 7 Segment Matrix
	DDRC = 0xFF; PORTC = 0x00; // LCD Screen Control 
	DDRD = 0xFF; PORTD = 0x00; // LCD Control Signal for PD5,6,7. Also, 7 Segment Matrix digit control. Maybe Data Output?	
	
	customLCD(); // This is the Welcome screen.

	
	unsigned short i;
	
	while (1)
	{

	}
	return 1;
}
