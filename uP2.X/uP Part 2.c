/* 
 * File: uP2.c
 * Author: Johnny Li
 * Created on February 9, 2020
 * Description:
 * A microcontroller based system that measures the resistance of a resistor
 * displaying the resistance in Ohms on an LCD display. The resistance ranges 
 * from 500Ohm to 2MOhm but only display the value at a range of 1kOhm to 1MOhm.
 * If resistance exceeds 1MOhms or is less than 1kOhms, the LCD will display 
 * "Out of Range".
 * Output Format: R=XXXXXXX Ohms
 */
/*
 * The following portion is taken from the student guide of 
 * Microprocessor & Microcontroller I/O, A/D and LCD.
 * http://www.add.ece.ufl.edu/4924/
 * void lcd_init(void);
 * void lcd_command(char);
 * void lcd_char(char);
 */
/*
 * PIN1:  Vss = GND	    
 * PIN2:  Vdd = +5V
 * PIN3:  Vo = Pot driven by +5V
 * PIN4:  RS (H: Data R/W  L:Instruction W) = RC4
 * PIN5:  R/W = GND (Always write mode)
 * PIN6:  Enable = RC7
 * PIN7:  DB4 = RC0
 * PIN8:  DB5 = RC1
 * PIN9:  DB6 = RC2
 * PIN10: DB7 = RC3
 * 
 * Note: RA6 and RA7 need to be configured because they share 
 * pins with external oscillators so PORTA was abandon and PORTC was used 
 * instead.
 */

//Includes files
#include <xc.h>     //Contain the PIC C commands
#include <stdio.h>
#include <stdlib.h>

//Configuration
#pragma config WDTE = OFF   //Disable watch dog timer
#pragma config LVP = ON      //Enable low voltage programming mode
#define _XTAL_FREQ 4000000  //The default clock is 4MHz so set delay clock by 
                            //same frequency.

//Declare methods
void lcd_init(void);
void lcd_command(char);
void lcd_char(char);
void adc_init(void);
unsigned long adcNum();

//Global Variable
unsigned long value; //Where to store the ADC result in
unsigned long R;  //Resistance
unsigned long r1 = 33000;   //Resistor value
int volt = 5; //Voltage

/*
 * Handles the writing to the LCD through PORTC.
 * The function will write a command to the command register of the LCD screen. 
 * The function takes a character (8-bit number) as an argument. If the argument 
 * is an integer or a long (16 or 32 bit number), only the lower 8 bits will be 
 * used. The function does not return any data.
 */
void lcd_command(char x){
	char temp;  //Temporary variable
    
    //Configure PORTC as output
	TRISCbits.TRISC0 = 0;
	TRISCbits.TRISC1 = 0;
	TRISCbits.TRISC2 = 0;
	TRISCbits.TRISC3 = 0;
	TRISCbits.TRISC4 = 0;
	TRISCbits.TRISC7 = 0;
    
	temp = x;   //Store input variable
	
    //Clear PORTC
    PORTC = 0;
    __delay_ms(5);

	x = x >>4;      //Left shift bit
	x = x & 0xF;    //Random values for the 4 LSB
	x = x | 0x80;   //Enable signal
	PORTC = x;      //Output to PORTC
	__delay_ms(5);
    
	x = x & 0xF;
	PORTC = x;
	__delay_ms(5);
    
	PORTC = 0;  //Clear for next line
	__delay_ms(5);
    
    //Second line input, repeat the process of the first
	x = temp;
	x = x & 0xF;
	x = x | 0x80;
	PORTC = x;
	__delay_ms(5);
    
	x = x & 0xF;
	PORTC = x;
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
    
    //Configure PORTC as output
	TRISCbits.TRISC0 = 0;
	TRISCbits.TRISC1 = 0;
	TRISCbits.TRISC2 = 0;
	TRISCbits.TRISC3 = 0;
	TRISCbits.TRISC4 = 0;
	TRISCbits.TRISC7 = 0;
    
	temp = x;   //Store input variable
    
    //Clear PORTC
	PORTC = 0x10;
	__delay_ms(5);
    
	x = x >>4;      //Left shift bit
	x = x & 0xF;    //Random values for the 4 LSB
	x = x | 0x90;   //Enable signal and organize data character
	PORTC = x;
	__delay_ms(5);
    
	x = x & 0x1F;
	PORTC = x;
	__delay_ms(5);
    
	PORTC = 0x10;   //Reset
	__delay_ms(5);
    
    //Second line input, repeat the process of the first
	x = temp;
	x = x & 0xF;
	x = x | 0x90;
	PORTC = x;
	__delay_ms(5);
    
	x = x & 0x1F;
	PORTC = x;
	__delay_ms(5);
}

/*
 * The function will initialize the ADC on PORTA pin 0. 
 * ADCON0- Disable ADC for now. 
 * ADCON1- Two conversions are processed as a pair, performed after every second 
 * conversion.
 * ADCON2- ADFLTR is transferred to ADPREV at start-of-conversion and ADACC, 
 * ADAOV and ADCNT registers are cleared.
 */
void adc_init(){
    TRISAbits.TRISA0 = 1;  //Configure PORTA pin 0 as input
    ADCON0 = 0x0;   //Disabled ADC and select RA0
}

/*
 * Get the ADC value and convert it to a char.
 */
unsigned long adcNum() {
    ADCON0bits.ADON = 1;    //Enable ADC
    ADCON0bits.GO = 1;  //Set go bit
    __delay_ms(5);
    
    while(ADCON0bits.GO){};  //Wait til conversion is complete 
    
    ADCON0bits.ADON = 0;    //Disable ADC
    
    //Shift bits of output
    return (ADRES >> 6);
}

/*
 * Initialize ADC and LCD.
 * Infinite loop to print the resistance of the resistor.
 * Get adc value -> voltage -> resistance.
 * Check if resistance meet the requirements and print out the correct output.
 */
void main() {
    adc_init();     //Initialized ADC
    lcd_init();     //Initialize LCD screen, note this takes care of PORTC I/0 
                    //direction  
   
    /* Test LCD code
     * Infinite loop to output 'X'
    while (1){  
        lcd_char('X');
    }*/
    
    //Infinite loop to print the resistance of the resistor. 
    while (1){
        //Load adc value
        value = adcNum()-2;
        
        //Calculate resistance 
        unsigned long Vout = (value*volt*1000)/1023;   //Vout = (adc val*Vin)/1023
        R = (Vout*r1)/((volt*1000)-Vout);   //R2 = (Vout*R1)/(Vin-Vout)
        
       char array[7]; //Storage array
       //Clear array
       for(int i=0; i<7; i++){
           array[i]=0;
       }
       //Covert to char array
       sprintf(array, "%lu", R);
       
        //Out of bound range
        if ((R<1000) || (R>1000000)){
            //Print out of bound
            lcd_char('O');
            lcd_char('u');
            lcd_char('t');
            lcd_char(' ');
            lcd_char('o');
            lcd_char('f');
            lcd_char(' ');
            lcd_char('B');
            lcd_char('o');
            lcd_char('u');
            lcd_char('n');
            lcd_char('d');
        }
        else{
            //Print format R=X XXX XXX Ohms
            lcd_char('R');
            lcd_char('=');
            lcd_char(' ');
            
            //Print number
            for(int i=0; i<7; i++){
                char temp = array[i];   //Temporary storage
                if(temp != 0){  //Check if not null
                    lcd_char(temp); //Print digit
                }
                else{
                    break;
                }
            }
            lcd_char(' ');
            lcd_char('O');
            lcd_char('h');
            lcd_char('m');
            lcd_char('s');
        }
        __delay_ms(2000); //Static view
        
        //Clear display
        lcd_command(0x01);  //Clear Home
    }
    
    return;  //Never to be reached.
}