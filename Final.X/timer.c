/*
 * I2C functions.
 * Code reference: https://tutorial.cytron.io/2013/10/28/interface-ds3231-rtc-module-pic-arduino/
 */
#include "timer.h"
#include "i2c.h"
#include "lcd.h"

/*
 * The function will start the time on the RTC.
 */
void rtc_init(void){
	//Read seconds
	unsigned char sec = i2c_read(RTC, 0x00);
	
	//Clear CH bit to enable the oscillator.
	if ((sec&0x80)!= 0) {
		i2c_write(RTC, 0x00, sec & 0x7f);
		//Delay
		for (unsigned int i = 0; i < 500; i++) {
			__delay_ms(1);
		}	
	}
	
	//Read hours
	unsigned char hour = i2c_read(RTC, 0x02);
	
	//Enable 24-hour mode.
	if ((hour & 0x40) != 0) {
		i2c_write(RTC, 0x02, hour & 0xbf);
	}
}	

/*
 * Getter for the seconds
 */
unsigned char get_seconds(void){
	unsigned char sec = i2c_read(RTC, 0x00);
	
	//Return a convert of BCD to binary
	return (sec&0x0f)+(((sec>>4)&0x07)*10);
}	

/*
 * Setter for the seconds
 */
void set_seconds(unsigned char sec){
    //Check if less than 60
	if (sec<60) {
		//Change the sec into BCD.
		sec=((sec/10)<<4)+(sec%10);
		
		//Get current seconds
		unsigned char curr_sec = i2c_read(RTC, 0x00);
		
		//Set CH bit
		curr_sec &= 0x80;
		
		//Write seconds out
		i2c_write(RTC, 0x00, curr_sec | sec);
	}	
}	

/*
 * Getter for the minutes
 */
unsigned char get_minutes(void){
	unsigned char min = i2c_read(RTC, 0x01);
	
	//Return a convert of BCD to binary
	return (min&0x0f)+((min >> 4)*10);
}	

/*
 * Setter for the minutes
 */
void set_minutes(unsigned char min){
    //Check if less than 60
	if (min<60) {
        //Change the min into BCD.
   		min = ((min/10)<<4)+(min%10);

        //Write minutes out
		i2c_write(RTC, 0x01, min);
	}	
}

/*
 * Getter for the hours
 */
unsigned char get_hours(void){
	unsigned char hour = i2c_read(RTC, 0x02);
	
	//Return a convert of BCD to binary
	return (hour&0x0f)+(((hour >> 4)&0x03)*10);
}	

/*
 * Setter for the hours
 */
void set_hours(unsigned char hour){
    //Check if less than 24
	if (hour<24) {
        //Change the hours into BCD.
   		hour=((hour/10)<<4)+(hour%10);

        //Get current seconds
		unsigned char curr_hour= i2c_read(RTC, 0x02);
		
		//24 hour mode bit
		curr_hour &= 0x40;
        
        //Write hours out
		i2c_write(RTC, 0x02, curr_hour | hour);
	}	
}

/*
 * Getter for the day name
 */
unsigned char get_dayname(void){
    //1-7 for Sunday-Saturday
	unsigned char name = i2c_read(RTC, 0x03);
	return name;
}	

/*
 * Setter for the day name
 */
void set_dayname(unsigned char name){
    //Check if a number between 1-7
	if (name>0 && name<8) {
		//Write day name out
		i2c_write(RTC, 0x03, name);
	}
}

/*
 * Getter for the month
 */
unsigned char get_months(void){
	unsigned char month = i2c_read(RTC, 0x05);

	//Return a convert of BCD to binary
	return (month&0x0f)+((month>> 4)*10);
}

/*
 * Setter for the month
 */
void set_months(unsigned char month){
	//Check if a number between 1-12
	if (month>0 && month<13) {
		//Change the month into BCD.
		month = ((month/10)<<4)+(month%10);

		//Write month out
		i2c_write(RTC, 0x05, month);
	}	
}

/*
 * Getter for the day of the month
 */
unsigned char get_days(void){
	unsigned char day = i2c_read(RTC, 0x04);

	//Return a convert of BCD to binary
	return (day&0x0f)+((day >> 4)*10);
}	

/*
 * Setter for the day of the month
 */
void set_days(unsigned char day){
	//Check if a number between 1-31
	if (day>0 && day<32) {
        //Change the day into BCD.
		day = ((day/10)<<4)+(day%10);
	
        //Write day of the month out
		i2c_write(RTC, 0x04, day);
	}	
}

/*
 * Getter for the year
 */
unsigned char get_years(void){
	unsigned char year = i2c_read(RTC, 0x06);

	//Return a convert of BCD to binary
	return (year & 0x0f)+((year>>4)*10);
}

/*
 * Setter for the year
 */
void set_years(unsigned char year){
    //Check if a number between 1-99
	if (year<100){
        //Change the day into BCD.
		year = ((year/10)<<4)+(year%10);

        //Write year out
		i2c_write(RTC, 0x06, year);
	}	
}

/*
 * Getter for the alarm seconds
 */
unsigned char get_Aseconds(void){
	unsigned char Asec = i2c_read(RTC, 0x07);
	
	//Return a convert of BCD to binary
	return (Asec&0x0f)+(((Asec>>4)&0x07)*10);
}	

/*
 * Setter for the alarm seconds
 */
void set_Aseconds(unsigned char Asec){
    //Check if less than 60
	if (Asec<60) {
		//Change the sec into BCD.
		Asec=((Asec/10)<<4)+(Asec%10);

		//Write seconds out
		i2c_write(RTC, 0x07, Asec);
	}	
}	

/*
 * Getter for the alarm minutes
 */
unsigned char get_Aminutes(void){
	unsigned char Amin = i2c_read(RTC, 0x08);
	
	//Return a convert of BCD to binary
	return (Amin&0x0f)+((Amin >> 4)*10);
}	

/*
 * Setter for the alarm minutes
 */
void set_Aminutes(unsigned char Amin){
    //Check if less than 60
	if (Amin<60) {
        //Change the min into BCD.
   		Amin = ((Amin/10)<<4)+(Amin%10);

        //Write minutes out
		i2c_write(RTC, 0x08, Amin);
	}	
}

/*
 * Getter for the alram hours
 */
unsigned char get_Ahours(void){
	unsigned char Ahour = i2c_read(RTC, 0x09);
	
	//Return a convert of BCD to binary
	return (Ahour&0x0f)+(((Ahour >> 4)&0x03)*10);
}	

/*
 * Setter for the alarm hours
 */
void set_Ahours(unsigned char Ahour){
    //Check if less than 24
	if (Ahour<24) {
        //Change the hours into BCD.
   		Ahour=((Ahour/10)<<4)+(Ahour%10);

        //Get current seconds
		unsigned char Acurr_hour= i2c_read(RTC, 0x09);
		
		//24 hour mode bit
		Acurr_hour &= 0x40;
        
        //Write hours out
		i2c_write(RTC, 0x09, Acurr_hour | Ahour);
	}	
}