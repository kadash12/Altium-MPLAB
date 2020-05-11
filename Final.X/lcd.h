/*
 * Header for lcd functions.
 */
#ifndef LCD_H
#define	LCD_H

#include <xc.h>     //Contain the PIC C commands
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define _XTAL_FREQ 4000000  //The default clock is 4MHz so set delay clock by 
                            //same frequency.

void lcd_init(void);
void lcd_command(char);
void lcd_char(char);

#endif	/* LCD_H */

