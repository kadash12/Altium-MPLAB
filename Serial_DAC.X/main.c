/* 
 * File:   main.c
 * Author: Johnny Li
 * Created on February 17, 2020
 * Description:
 * Design a function generator that outputs a sine wave, a square wave, a 
 * triangle wave and a saw tooth wave from 10 Hz to 100Hz and a 1V peak to 5V 
 * peak. The waveform will be selected through the use of two switches where 
 * each of the four combinations of switch positions will correspond to one of 
 * the four waveforms. There will be two rotary controls, one controlling the 
 * frequency and the other controlling the output voltage amplitude. The output 
 * waveforms must be comprised of at least 50 samples per cycle. The frequency 
 * and output amplitude must not change when switching between the four waveform
 * types. The output voltage will always have a minimum voltage of 0 V and a 
 * variable maximum value from 1V to 5V. Must use the SPI interface built into 
 * the CPU to connect to and communicate with an external digital to analog 
 * integrated circuit to generate the waveforms.
 */

//Includes files
#include <xc.h>     //Contain the PIC C commands
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//Configuration
#pragma config WDTE = OFF   //Disable watch dog timer
#pragma config LVP = ON      //Enable low voltage programming mode
#define _XTAL_FREQ 4000000  //The default clock is 4MHz so set delay clock by 
                            //same frequency.

//Declare methods
void adc_init();    //Initialize PortA
unsigned long adcNum0();    //Get ADC voltage
void spi_init();    //SPI initialization
void spi_write(unsigned char data);     //SPI write

//Global Variable
unsigned long value0 = 0;   //Where to store the ADC result in (Voltage)
unsigned long F = 0;  //Frequency
float temp = 0.0;   //Temporary Variable
int volt = 5;   //Voltage reference

//Sine Wave
char sin[50] = {
    0x80,0x8f,0x9f,0xae,0xbd,0xca,0xd7,0xe2,0xeb,0xf3,
    0xf9,0xfd,0xff,0xff,0xfd,0xf9,0xf3,0xeb,0xe2,0xd7,
    0xca,0xbd,0xae,0x9f,0x8f,0x80,0x70,0x60,0x51,0x42,
    0x35,0x28,0x1d,0x14,0xc,0x6,0x2,0x0,0x0,0x2,
    0x6,0xc,0x14,0x1d,0x28,0x35,0x42,0x51,0x60,0x70
};
//Square Wave
char square[50] = {
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01
};
//Triangle Wave
char triangle[50] = {
    0xa,0x14,0x1f,0x29,0x33,0x3d,0x47,0x52,0x5c,0x66,
    0x70,0x7a,0x85,0x8f,0x99,0xa3,0xad,0xb8,0xc2,0xcc,
    0xd6,0xe0,0xeb,0xf5,0xff,0xf5,0xeb,0xe0,0xd6,0xcc,
    0xc2,0xb8,0xad,0xa3,0x99,0x8f,0x85,0x7a,0x70,0x66,
    0x5c,0x52,0x47,0x3d,0x33,0x29,0x1f,0x14,0xa,0x0
};
//Sawtooth Wave
char sawtooth[50] = {
    0x5,0xa,0xf,0x14,0x1a,0x1f,0x24,0x29,0x2e,0x33,
    0x38,0x3d,0x42,0x47,0x4d,0x52,0x57,0x5c,0x61,0x66,
    0x6b,0x70,0x75,0x7a,0x80,0x85,0x8a,0x8f,0x94,0x99,
    0x9e,0xa3,0xa8,0xad,0xb3,0xb8,0xbd,0xc2,0xc7,0xcc,
    0xd1,0xd6,0xdb,0xe0,0xe6,0xeb,0xf0,0xf5,0xfa,0xff
};

/*
 * The function will initialize the PORTA pin 0 to be input.
 * PORTB pin 0 as input. 
 */
void adc_init(){
    TRISAbits.TRISA0 = 1;  //Configure PORTA pin 0 as input
   
    //Configure PORTB as input
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
        
    //Clear and Enable
    ANSELC = 0x0;
    ANSELB = 0x0;
    
    //ADC Clock
    ADCLK = 0b00111111;
}

/*
 * Get the ADC value from PortA pin 0 (frequency) and convert it to a char.
 */
unsigned long adcNum0() {
    ADCON0 = 0x00;   //Select RA0
    ADPCH = 0;
    ADCON0bits.ADON = 1;    //Enable ADC
    ADCON0bits.GO = 1;  //Set go bit
    
    while(ADCON0bits.GO){};  //Wait til conversion is complete 
    
    ADCON0bits.ADON = 0;    //Disable ADC
    
    //Shift bits of output
    return (ADRES >> 6);
}

//Bit-banging DAC write
//Used for testing
//Reference: http://www.add.ece.ufl.edu/4924/
//Note that the LTC1661 is a 10-bit DAC, and the function only uses 8-bits. The 
//code left shifts two bits and the 2 least significant bits are 0.
void dac_write(char data){
	unsigned int output = 0x0000;
	unsigned char temp;
	signed char x;

	TRISDbits.TRISD0 = 0; // set direction register
	TRISDbits.TRISD1 = 0;
	TRISDbits.TRISD2 = 0;
	
	PORTDbits.RD0 = 0;   // data to 0
	PORTDbits.RD1 = 0;   // clock to 0
	PORTDbits.RD2 = 1;   // enable to 1

	temp = data;         // format data
	data >>= 4;
	data |= 0xF0;
	output = data;
	output <<= 8;
	data = temp;
	data <<= 4;
	output |= (data & 0x00FF);

	PORTDbits.RD2 = 0;  //enable low
	for (x = 15; x > -1; x--){   // send 16 bits of data
		PORTDbits.RD0 = (output >> x) & 0x01;
		PORTDbits.RD1 = 1;
		PORTDbits.RD1 = 0;
		PORTDbits.RD0 = 0;
	}
	PORTDbits.RD2 = 1;  //enable high
}

/*
 * Configures SPI1, uses PortC for the DAC LTC1661.
 * The output rate specified by the frequency.
 */
void spi_init(){    
    //Data sample input for the end of output
    SSP1STATbits.SMP = 1;
    //Transmit
    SSP1STATbits.CKE = 1;
    //Enables the needed pins (master,slave,data,enable)
    SSP1CON1bits.SSPEN = 1;
    //Clk =FOSC/64
    SSP1CON1bits.SSPM = 2;

    TRISCbits.TRISC0 = 0;   //Select
    TRISCbits.TRISC3 = 0;   //CLK
    TRISCbits.TRISC5 = 0;   //Data

     //Set as output pin
    RC5PPS = 0x10;  //PPS SDA
    RC3PPS = 0x0F;  //PPS SCL
}

/*
 * SPI1 transmit the data to the DAC.
 */
void spi_write(unsigned char data){
    LATCbits.LATC0 = 0;     //Enable output
    
    uint16_t full = 0xF000;     //Command for DAC
    full = full | (data << 4);  //Shift to DAC place
    
    uint8_t bound = (full >> 8);
    
    SSP1BUF;   //Dummy read to clear flag
    SSP1BUF = bound;     //Upper bound
    while(!SSP1STATbits.BF);    //Wait till it finish writing from buffer
    
    SSP1STATbits.BF = 0;    //Clear flag 
    
    bound = full;    
    SSP1BUF;   //Dummy read to clear flag
    SSP1BUF = bound;    //Lower bound    
    while(!SSP1STATbits.BF);    //Wait till it finish writing from buffer
    
    LATCbits.LATC0 = 1;     //Disable output
}

/*
 * 
 */
void main() {         
    OSCFRQ = 0x07;  //48 MHz
    
    adc_init();     //Initialized ADC ports
    spi_init();    //Initialized spi1
    
    //Infinite loop to generate the function. 
    while(1){
        //Load adc values
        if(value0 == 0){    //Average out value
            value0 = adcNum0();
        }
        else{
            value0 = ((value0+adcNum0())/2)+2;   //Frequency ADC
        }
        
        //Port B pin 0 and pin 1
        //(0,0) sin
        if(PORTBbits.RB0==0 && PORTBbits.RB1==0){
            //Testing
            /*for (int x = 0; x < 50; x++){
                int volatile outv = (int)(sin[x]/temp);     //Adjust to voltage
                dac_write(outv);
            }*/
            for (int x = 0; x < 50; x++){
                unsigned char volatile outv = (int)(sin[x]);     //Adjust to voltage
                spi_write(outv);
                for(int i=0; i<(unsigned int)((value0)/8); i++){
                    __delay_ms(1);
                }
            }
        }
        //(1,0) square
        else if(PORTBbits.RB0==1 && PORTBbits.RB1==0){
            for (int x = 0; x < 50; x++){
                unsigned char volatile outv = (int)(square[x]);     //Adjust to voltage
                spi_write(outv);
                 for(int i=0; i<(unsigned int)((value0)/8); i++){
                    __delay_ms(1);
                }
            }
        }
        //(0,1) triangle
        else if(PORTBbits.RB0==0 && PORTBbits.RB1==1){
            for (int x = 0; x < 50; x++){
                unsigned char volatile outv = (int)(triangle[x]);     //Adjust to voltage
                spi_write(outv);
                 for(int i=0; i<(unsigned int)((value0)/8); i++){
                    __delay_ms(1);
                }
            }
        }
        //(1,1) sawtooth
        else if(PORTBbits.RB0==1 && PORTBbits.RB1==1){
           for (int x = 0; x < 50; x++){
                unsigned char volatile outv = (int)(sawtooth[x]);     //Adjust to voltage
                spi_write(outv);
                 for(int i=0; i<(unsigned int)((value0)/8); i++){
                    __delay_ms(1);
                }
            }
        }
    }
    return;
}