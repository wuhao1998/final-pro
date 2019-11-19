#ifndef __io_h__
#define __io_h__
void LCD_Command(unsigned char cmnd);
void LCD_Char (unsigned char char_data);
void LCD_Custom_Char (unsigned char loc, unsigned char *msg);
void LCD_init();
void LCD_ClearScreen(void);
void LCD_WriteCommand (unsigned char Command);
void LCD_Cursor (unsigned char column);
void LCD_DisplayString(unsigned char column ,const unsigned char *string);
void delay_ms(int miliSec);
#endif

