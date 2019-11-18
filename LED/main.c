/*
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

/*
Scheduler module
*/
static task task1, task2,task3, task4;
task *tasks[] = {&task1, &task2, &task3, &task4};

KeyPad Values
*/
unsigned int keyPressed = '\0';
unsigned char playCrusor = 0;
unsigned char mapTravel = 0;

char upperMap[10] = {2,3,4,8,12,20,25,27,29,30};
char lowerMap[5] = {10,15,17,18,22};
char upperMapReset[10] = {2,3,4,8,12,20,25,27,29,30};
char lowerMapReset[5] = {10,15,17,18,22};

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

void crusorAt(unsigned char row, unsigned char col)//element to find the cursor
{
	if(row == 1){
		if(col <= 16){
			LCD_Cursor(col+1);
		}
	}
	else if(row == 2){
		if(col <= 16){
			LCD_Cursor(col+17);
		}
	}
}

enum gameStage {start, wait, gamePress, gameRelease, win, lose}; //This is the main game function
int gameTick(int state)
{
	unsigned char i = 0;
	int previousState = state;
	switch(state)
	{
		case start:
			state = wait;
			break;
		case wait:
			if (keyPressed == '#'){
				state = gamePress;
			}
			else{
				state = wait;
			}
			break;
		case gamePress:
			if (keyPressed != '#'){
				state = gameRelease;
			}
			else{
				state = gamePress;
			}
			break;
		case gameRelease:
			if (keyPressed == '#'){
				state = wait;
			}
			else{
				if(mapTravel >= 30){
					state = win;
				}
				else{
					for(i=0 ; i<10 ;i++){
						if(playCrusor == 1 && upperMap[i]+1==1){
							state = lose;
						}
					}
					for (i =0; i<5;i++){
						if(playCrusor == 17 && lowerMap[i]+17==17){
							state = lose;
						}
					}
				}
			}
			break;
		case win:
			if(keyPressed == '#'){
				state = wait;
			}
			else{
				state = win;
			}
			break;
		case lose:
			if(keyPressed == '#'){
				state = wait;
			}
			else{
				state =lose;
			}
			break;
		default:
			break;
	}

	switch(state){
		case start:
			break;
		case wait:
			playCrusor = 17;
			memcpy(upperMap, upperMapReset, sizeof (upperMap));
			memcpy(lowerMap, lowerMapReset, sizeof (lowerMap));
			if(previousState != wait){
				LCD_DisplayString(1, "Press # START!  Use CD to move");
				//LCD_Command(0xC0);
				LCD_Char(0);LCD_Char(2);
				//LCD_Char(2);LCD_Char(4);LCD_Char(5);LCD_Char(4);LCD_Char(5);LCD_Char(4);
				//LCD_Char(5);LCD_Char(4);LCD_Char(7);LCD_Char(7);LCD_Char(7);LCD_Char(7);LCD_Char(7);LCD_Char(7);
			}
			//EEPROM USEAGE
			char highest = eeprom_read_byte((uint8_t*)1)+eeprom_read_byte((uint8_t*)10)*10;
			for(int c = 0; c < 250;c++){
				PORTB = seg_code[highest/10];
				PORTD = ~SegThree;
				_delay_ms(1);
				PORTB = seg_code[highest%10];
				PORTD = ~SegFour;
			}
			PORTB = 0;
			break;
		case gamePress:
			break;
		case gameRelease:
			break;

		case win:
			if(previousState != win){
				LCD_DisplayString(1, "YOU WIN!        Your score is:");
				LCD_Cursor(31);
				LCD_WriteData(mapTravel/10+'0');
				LCD_Cursor(32);
				LCD_WriteData(mapTravel%10+'0');
				//Seven Segment Display
				for(int p = 0 ; p <2000; p++){
					PORTB = seg_code[mapTravel/10];
					PORTD = ~SegThree;
					_delay_ms(1);
					PORTB = seg_code[mapTravel%10];
					PORTD = ~SegFour;
				}
				PORTB = 0;
				//EEPROM WRITE
				if(mapTravel > highest)
				{
					eeprom_write_byte((uint8_t*)10,mapTravel/10);
					eeprom_write_byte((uint8_t*)1,mapTravel%10);
				}
				 //  write the byte 64 to location 1 of the EEPROM
			}
			break;

		case lose:
			if(previousState != lose){
				LCD_DisplayString(1, "GAME OVER!      Your score is:");
				LCD_Cursor(31);
				LCD_WriteData(mapTravel/10+'0');
				LCD_Cursor(32);
				LCD_WriteData(mapTravel%10+'0');
				for(int q = 0 ; q <2000; q++){
					PORTB = seg_code[mapTravel/10];
					PORTD = ~SegThree;
					_delay_ms(1);
					PORTB = seg_code[mapTravel%10];
					PORTD = ~SegFour;
				}
				PORTB = 0;
				if(mapTravel > highest)
				{
					eeprom_write_byte((uint8_t*)10,mapTravel/10);
					eeprom_write_byte((uint8_t*)1,mapTravel%10);
				} //  write the byte 64 to location 1 of the EEPROM
			}
			break;
		default:
			break;
	}
	return state;
}

int main() {
	/* Configure the ports as output */
	DDRA = 0xF0; PORTA = 0x0F; // KeyPad Entry
	DDRB = 0xFF; PORTB = 0x00; // 7 Segment Matrix
	DDRC = 0xFF; PORTC = 0x00; // LCD Screen Control
	DDRD = 0xFF; PORTD = 0x00; // LCD Control Signal for PD5,6,7. Also, 7 Segment Matrix digit control. Maybe Data Output?

	customLCD(); // This is the Welcome screen.
	LCD_init();
	LCD_ClearScreen();
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	/*
	Task doing state
	*/
	task1.state =  start;
	task1.period = 10;
	task1.elapsedTime = task1.period;
	task1.TickFct = &gameTick;

	task2.state = mapwait;
	task2.period = 300;
	task2.elapsedTime = task2.period;
	task2.TickFct = &mapTick;

	task3.state = keypadwait;
	task3.period = 10;
	task3.elapsedTime = task3.period;
	task3.TickFct = &keypadTick;

	task4.state = finalwait;
	task4.period = 10;
	task4.elapsedTime = task4.period;
	task4.TickFct = &finalTick;

	/*
	task5.state = fourdigitwait;
	task5.period = 10;
	task5.elapsedTime = task5.period;
	task5.TickFct = &fourdigitTick;
	*/

	/*
	GCD function
	*/
	unsigned long GCD = tasks[0]->period;
	for(int i = 1;i <numTasks; i++){
		GCD = findGCD(GCD,tasks[i]->period);
	}
	TimerSet(1);
	TimerOn();

	unsigned short i;

	while (1)
	{
		//sevenSeg();
		//lcdMatrix();
		/*
		Something Common
		*/

		for(i = 0; i < numTasks; i++){
			if(tasks[i]->elapsedTime == tasks[i]->period)
			{
				tasks[i]->state= tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			else tasks[i]->elapsedTime += GCD;
		}
			while(!TimerFlag);
			TimerFlag = 0;

	}
	return 1;
}
