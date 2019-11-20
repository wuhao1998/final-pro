/*	Author: hwu901
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include"io.c"
#include"io.h"
#define A0 (~PINA & 0x01)
#define A1 (PINA & 0x02)
#define A2 (PINA & 0x04)
#define A3 (PINA & 0x08)
#define A4 (PINA & 0x10)
#define A5 (PINA & 0x20)
#define A6 (PINA & 0x40)

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter

		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		// prevents OCR0A from underflowing, using prescaler 64     // 31250 is largest frequency that will not result in underflow
		
		else if (frequency > 31250) { OCR3A = 0x0000; }

		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
		
		TCNT3 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum States{init, wait, on, off, endsong, wait2, lost, win} state;

// twinkle twinkle little star, how I wonder what you are.
//G, G, D, D, E, E, D, C, C, B, B, A, A, G
//int button[14]= {A1, A1, A5, A5, A6, A6, A5, A4, A4, A3, A5, A2, A2, A1};
int notes[14] = {392, 392, 587, 587, 659, 659, 587, 523, 523, 493, 493, 440, 440, 392};
unsigned short timeHeld[14] = {25, 25, 25, 25, 25, 25, 50, 25, 25, 25, 25, 25, 25, 50};
unsigned short timeBetween[14] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};


unsigned char input;
unsigned char n;	// index for notes
unsigned char i;	// tick counter
unsigned char counter;	// counter2 for finish

void tick(){
  
 
	
	switch(state){
		case init:
		  
		state = wait;
		break;
		
		case wait:
		   
		if(A0){
			state = on;
			LCD_DisplayString(1,"Try To Memorize the Notes");
		}
		else{
			state = wait;
		}
		break;
		
		case on:
		  
		if(i <= timeHeld[n]){
			state = on;
		}
		else if(i > timeHeld[n]){
			counter++;
			if(counter > 13){
				state = endsong;
				
			}
			else{
				state = off;
				
				i = 0;
			}
		}
		break;
		
		case off:
		if(i <= timeBetween[counter]){
			state = off;
		}
		else if(i > timeBetween[counter]){
			state = on;
			n++;
			i = 0;
		}
		break;
		
		case endsong:
		  LCD_DisplayString(1,"Try To Repeat   the Notes");
		state = wait2;
		break;
		
		case wait2:
		if(!A0){
			state = wait2;
		}
		else{
			state = wait;
		}
		break;
		
		default:
		break;
	}
	
	switch(state){
		case init:
		break;
		
		case wait:
		  
		n = 0;
		i = 0;
		counter = 0;
		break;
		
		case on:
		 
		set_PWM(notes[n]);	//index++ every timeHeld
		switch(notes[n]){
		case 392:
		  PORTA |= (1<<1);
		  break;
		case 440:
		  PORTA |= (1<<2);
		  break;
		case 493:
		  PORTA |= (1<<3);
		  break;
		case 523:
		  PORTA |= (1<<4);
		  break;
		case 587:
		  PORTA |= (1<<5);
		  break;
		case 659:
		  PORTA |= (1<<6);
		  break;
		}

		i++;
		break;
		
		case off:
		  
		set_PWM(0);
		PORTA = 0x01;
		i++;
		break;
		
		case endsong:
		set_PWM(0);
		PORTA = 0x01;
		break;
		
		case wait2:
		  if (A1) {
			set_PWM(392);
		}
		else if (A2) {
			set_PWM(440);
		}
		else if (A3) {
			set_PWM(493);
			
		}
               	else if (A4) {
			set_PWM(523);
		}
		else if (A5) {
			set_PWM(587);
			
		}
             	else if (A6) {
			set_PWM(659);
		}
		else {
			set_PWM(0);//do nothing
		}
		break;
	}
}
        

int main(void)
{
	DDRA = 0x00; PORTA = 0x01;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00; 
	DDRD = 0xFF; PORTD = 0x00;

	state = init;
	LCD_init();
	LCD_DisplayString(1,"Push Button To  Start Game");
	TimerSet(10);
	TimerOn();
	
	PWM_on();
	
	while (1)
	{
		tick();
		while(!TimerFlag);
		TimerFlag = 0;
	}
}
