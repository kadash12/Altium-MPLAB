/*
 * DAC functions.
 */
#include "dac.h"
#include "lcd.h"

/*
 * Configures SPI1, uses PortC for the DAC LTC1661.
 * The output rate specified by the frequency.
 */
void spi_init(){    
    //Data sample input for the end of output
    SSP2STATbits.SMP = 1;
    //Transmit
    SSP2STATbits.CKE = 1;
    //Enables the needed pins (master,slave,data,enable)
    SSP2CON1bits.SSPEN = 1;
    //Clk =FOSC/64
    SSP2CON1bits.SSPM = 2;

    TRISBbits.TRISB0 = 0;   //Select
    TRISBbits.TRISB3 = 0;   //CLK
    TRISBbits.TRISB1 = 0;   //Data

     //Set as output pin
    RB1PPS = 0x12;  //PPS SDA
    RB3PPS = 0x11;  //PPS SCL
}

/*
 * SPI1 transmit the data to the DAC.
 */
void spi_write(unsigned char data){
    LATBbits.LATB0 = 0;     //Enable output
    
    uint16_t full = 0xF000;     //Command for DAC
    full = full | (data << 4);  //Shift to DAC place
    
    uint8_t bound = (full >> 8);
    
    SSP2BUF;   //Dummy read to clear flag
    SSP2BUF = bound;     //Upper bound
    while(!SSP2STATbits.BF);    //Wait till it finish writing from buffer
    
    SSP2STATbits.BF = 0;    //Clear flag 
    
    bound = full;    
    SSP2BUF;   //Dummy read to clear flag
    SSP2BUF = bound;    //Lower bound    
    while(!SSP2STATbits.BF);    //Wait till it finish writing from buffer
    
    LATBbits.LATB0 = 1;     //Disable output
}