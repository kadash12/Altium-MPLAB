/* 
 * Header for I2C functions.
 */

#ifndef I2C_H
#define	I2C_H

#include <xc.h>     //Contain the PIC C commands
#include <stdio.h>
#include <stdlib.h>

void i2c_init();  
unsigned char i2c_read(unsigned char address, unsigned char registers);
void i2c_write(unsigned char address, unsigned char registers, unsigned char data);

#endif	/* I2C_H */

