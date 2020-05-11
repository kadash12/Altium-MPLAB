/*
 * I2C functions.
 * Code reference: https://tutorial.cytron.io/2013/10/28/interface-ds3231-rtc-module-pic-arduino/
 */
#include "i2c.h"

/*
 * Initialization of I2C through PortC for the DS3231.
 */
void i2c_init(){        
    //Set as input pin
    TRISCbits.TRISC3 = 1;   //CLK
    TRISCbits.TRISC4 = 1;   //Data
   
    //Setup PPS, I2C uses the same values as SPI
    //Inputs PPS
    SSP1CLKPPS = 0x13;
    SSP1DATPPS = 0x14;
    //Output PPS
    RC4PPS = 0x10;  //PPS SDA
    RC3PPS = 0x0F;  //PPS SCL
    
    //Setup the I2C master clock.
    SSP1ADD = 12;   //MSSP Baud Rate Divider    SCL=((n + 1) *4)/FOSC

    SSP1CON1bits.SSPM = 8;  //Clock=FOSC/(4*(SSPxADD+1))
    
    SSP1CON1bits.WCOL = 0;  //Clear the write collision detect bit.
    SSP1CON1bits.SSPOV = 0; //Clear the receive overflow indicator bit.
    SSP1CON1bits.SSPEN = 1; //Enables the needed pins (master,slave,data,enable)
}

/*
 * I2C protocol to read data from a given address and register. 
 */
unsigned char i2c_read(unsigned char address, unsigned char registers){        
	SSP1CON2bits.SEN = 1;   //Enable start bit.
	while (SSP1CON2bits.SEN == 1);      //Wait till process is complete 
	
	SSP1BUF = (address << 1) & 0xfe;   //Load buffer with address and write command.
	
    while (SSP1STATbits.RW == 1);   //Wait till transmit is complete
    
	//Check if slave fail acknowledge
	if (SSP1CON2bits.ACKSTAT == 1) {
		SSP1CON2bits.PEN = 1;    //Set stop bit.
		while (SSP1CON2bits.PEN == 1);   //Wait till process is complete 
		return 0;
	}
	
	SSP1BUF = registers;    //Load registers
	
    //Repeat process
    while (SSP1STATbits.RW == 1);
    
	if (SSP1CON2bits.ACKSTAT == 1) {
		SSP1CON2bits.PEN = 1;
		while (SSP1CON2bits.PEN == 1);
		return 0;
	}
	
	//Repeated start bit.
	SSP1CON2bits.RSEN = 1;
	while (SSP1CON2bits.RSEN == 1);
	
	SSP1BUF = (address << 1) | 0x01;    //Load buffer with address to read.
    
    while (SSP1STATbits.RW == 1);
    
	if (SSP1CON2bits.ACKSTAT == 1) {
		SSP1CON2bits.PEN = 1;
		while (SSP1CON2bits.PEN == 1);
		return 0;
	}
	
	SSP1CON2bits.RCEN = 1;  //Enable receive.
	
	unsigned long num = 10000L;   //Delay
	while (SSP1STATbits.BF == 0) {  //Read buffer
        if (--num == 0) {
            SSP1CON2bits.PEN = 1;
            while (SSP1CON2bits.PEN == 1);
            return 0;
        }
    }
	
	unsigned char read = SSP1BUF;  //Load read with buffer data.
	
	//Check acknowledge
	SSP1CON2bits.ACKDT = 1;
	SSP1CON2bits.ACKEN = 1;
	while (SSP1CON2bits.ACKEN == 1);
	
	SSP1CON2bits.PEN = 1;   //Set stop bit.
	while (SSP1CON2bits.PEN == 1);  //Wait till process is complete 
	
	return read;
}

/*
 *  I2C protocol to write data to a given address and register of some data. 
 */
void i2c_write(unsigned char address, unsigned char registers, unsigned char data){    
	SSP1CON2bits.SEN = 1;   //Set start bit.
	while (SSP1CON2bits.SEN == 1);      //Wait till process is complete 
	
	SSP1BUF = (address << 1) & 0xfe;   //Load buffer with address and write command.
    
    while (SSP1STATbits.RW == 1);
    
    //Slave fail to acknowledge
	if (SSP1CON2bits.ACKSTAT == 1) {
		SSP1CON2bits.PEN = 1;    //Set stop bit.
		while (SSP1CON2bits.PEN == 1);   //Wait till process is complete 
		return;
	}
	
	SSP1BUF = registers;    //Load register to buffer
	
    //Repeat process
    while (SSP1STATbits.RW == 1);
    
	if (SSP1CON2bits.ACKSTAT == 1) {
		SSP1CON2bits.PEN = 1;
		while (SSP1CON2bits.PEN == 1);
		return;
	}

	SSP1BUF = data;     //Load buffer with data
	
    while (SSP1STATbits.RW == 1);
    
	if (SSP1CON2bits.ACKSTAT == 1) {
		SSP1CON2bits.PEN = 1;
		while (SSP1CON2bits.PEN == 1);
		return;
	}
	
    SSP1CON2bits.PEN = 1;   //Set stop bit.
	while (SSP1CON2bits.PEN == 1);  //Wait till process is complete 
}
