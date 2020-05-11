/* 
 * Header for timer functions.
 */

#ifndef TIMER_H
#define	TIMER_H

#define RTC 104     //I2C slave address.

void rtc_init();
unsigned char get_seconds();
void set_seconds(unsigned char sec);
unsigned char get_minutes();
void set_minutes(unsigned char min);
unsigned char get_hours();
void set_hours(unsigned char hour);
unsigned char get_dayname();
void set_dayname(unsigned char name);
unsigned char get_months();
void set_months(unsigned char month);
unsigned char get_days();
void set_days(unsigned char day);
unsigned char get_years();
void set_years(unsigned char year);
unsigned char get_Aseconds();
void set_Aseconds(unsigned char Asec);
unsigned char get_Aminutes();
void set_Aminutes(unsigned char Amin);
unsigned char get_Ahours();
void set_Ahours(unsigned char Ahour);

#endif	/* TIMER_H */

