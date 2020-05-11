/*
 * LCD functions.
 */
#include "lcd.h"

/*
 * Handles the writing to the LCD through PORTD.
 * The function will write a command to the command register of the LCD screen. 
 * The function takes a character (8-bit number) as an argument. If the argument 
 * is an integer or a long (16 or 32 bit number), only the lower 8 bits will be 
 * used. The function does not return any data.
 */
void lcd_command(char x){
	char temp;  //Temporary variable
    
    //Configure PORTD as output
	TRISDbits.TRISD0 = 0;
	TRISDbits.TRISD1 = 0;
	TRISDbits.TRISD2 = 0;
	TRISDbits.TRISD3 = 0;
	TRISDbits.TRISD4 = 0;
	TRISDbits.TRISD7 = 0;
    
	temp = x;   //Store input variable
	
    //Clear PORTD
    PORTD = 0;
    __delay_ms(5);

	x = x >>4;      //Left shift bit
	x = x & 0xF;    //Random values for the 4 LSB
	x = x | 0x80;   //Enable signal
	PORTD = x;      //Output to PORTD
	__delay_ms(5);
    
	x = x & 0xF;
	PORTD = x;
	__delay_ms(5);
    
	PORTD = 0;  //Clear for next line
	__delay_ms(5);
    
    //Second line input, repeat the process of the first
	x = temp;
	x = x & 0xF;
	x = x | 0x80;
	PORTD = x;
	__delay_ms(5);
    
	x = x & 0xF;
	PORTD = x;
	__delay_ms(5);
}

/*
 * The function will initialize the LCD screen into 4 bit / 2 line mode, turn 
 * off the blinking cursor, clear the screen, and place the cursor at address 
 * 0x00 (first position). The function takes no argument and does not return any 
 * data. 
 */
void lcd_init(void){
	lcd_command(0x33);
	lcd_command(0x32);  //4-bit Mode Enable
	lcd_command(0x2C);  //Enable 2-line mode
	lcd_command(0x0C);  //Turned off blink and cursor, set to 0x0F to turn on
	lcd_command(0x01);  //Clear Home
}

/*
 * The function will write a character to the screen at the current cursor 
 * address. The function takes a character (8-bit number) as an argument. If the 
 * argument is an integer or a long (16 or 32 bit number), only the lower 8 bits 
 * will be used. The function does not return any data
 */
void lcd_char(char x){
	char temp;  //Temporary variable
    
    //Configure PORTD as output
	TRISDbits.TRISD0 = 0;
	TRISDbits.TRISD1 = 0;
	TRISDbits.TRISD2 = 0;
	TRISDbits.TRISD3 = 0;
	TRISDbits.TRISD4 = 0;
	TRISDbits.TRISD7 = 0;
    
	temp = x;   //Store input variable
    
    //Clear PORTD
	PORTD = 0x10;
	__delay_ms(5);
    
	x = x >>4;      //Left shift bit
	x = x & 0xF;    //Random values for the 4 LSB
	x = x | 0x90;   //Enable signal and organize data character
	PORTD = x;
	__delay_ms(5);
    
	x = x & 0x1F;
	PORTD = x;
	__delay_ms(5);
    
	PORTD = 0x10;   //Reset
	__delay_ms(5);
    
    //Second line input, repeat the process of the first
	x = temp;
	x = x & 0xF;
	x = x | 0x90;
	PORTD = x;
	__delay_ms(5);
    
	x = x & 0x1F;
	PORTD = x;
	__delay_ms(5);
}