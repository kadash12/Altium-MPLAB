/* 
 * File:   timer.c
 * Author: Johnny Li
 * Created on March 3, 2020
 * Description:
 * Design a I²C alarm clock that uses I²C methods to communicate with DS3231,
 * high precision clock. Set and read time in units of seconds, minutes, hours, 
 * day of week, month, date, and year. Take leap year into consideration. Be 
 * able to set, acknowledge and reset alarms. Design a digital alarm clock 
 * utilizing DS3231. Should be able to initialize clock with variable time, 
 * utilizing all available units of time. Clock will display current time on 
 * LCD, updated every second. Display format should logically display time of 
 * day and current date. Alarm will be set AFTER initialization of the clock. A 
 * switch should change the LCD from displaying time to setting alarm time. A 
 * potentiometer should be used to change alarm time in minutes, seconds, and 
 * hours. Once the time is set, a button should set the alarm. The alarm should 
 * be configured to output a active high signal from the DS3231. When the alarm 
 * is activated, produce an audile tone until a button is pressed. When the 
 * button is pressed, the tone should stop and alarm should be reset.
 */
//Code reference: https://tutorial.cytron.io/2013/10/28/interface-ds3231-rtc-module-pic-arduino/

//Includes files
#include "adc.h"
#include "lcd.h"
#include "i2c.h"

//Configuration
#pragma config WDTE = OFF   //Disable watch dog timer
#pragma config LVP = ON      //Enable low voltage programming mode
#define RTC 104     //I2C slave address.

//Global Variable
unsigned long value0 = 0;   //Where to store the ADC result

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

/*
 *
 */
void main() {
    int setNum=0;   //Switch case selection
    int AsetNum=0;   //Alarm switch case selection
    int alarm = 0;  //alarm flag
    int check = 0;  //Check flag
    int alarmON = 0;    //Speaker alarm on 
    
    adc_init();     //Initialized ADC ports
    lcd_init();     //Initialize LCD screen, note this takes care of PORTC I/0 
                    //direction  
    i2c_init();     //Initialized i2c
    rtc_init();     //Initialized rtc
    
    TRISAbits.TRISA2 = 1;  //Configure PORTA pin 2 as input (switch1)
    TRISAbits.TRISA3 = 1;  //Configure PORTA pin 3 as input (button1)
    TRISAbits.TRISA4 = 1;  //Configure PORTA pin 4 as input (button2)
    TRISAbits.TRISA5 = 1;  //Configure PORTA pin 4 as input (alarm)
    ANSELA = 0x0;   //Clear and Enable
    
    TRISCbits.TRISC0 = 0;   //Configure PORTC pin 0 as output
    PORTCbits.RC0 = 0;  //Clear and enable
    TRISCbits.TRISC1 = 0;   //Configure PORTC pin 1 as output
    PORTCbits.RC1 = 0;  //Clear and enable
    TRISCbits.TRISC2 = 0;   //Configure PORTC pin 1 as output
    PORTCbits.RC2 = 0;  //Clear and enable
    TRISCbits.TRISC5 = 0;   //Configure PORTC pin  as output
    PORTCbits.RC5 = 0;  //Clear and enable
    
    //Infinite loop to output the time. 
    while(1){        
        if(PORTAbits.RA2==1){   //Set mode
            //Load adc values
            if(value0 == 0){    //Average out value
                value0 = adcNum0();
            }
            else{
                value0 = ((value0+adcNum0())/2)+1;   //ADC average
            }

            //Convert adc to time unit
            int u24 = value0/42;    //24 unit
            int u60 = value0/17;    //60 unit
            int u7 = (value0/146)+1;    //7 unit
            int u12 = (value0/85)+1;    //12 unit
            int u31 = (value0/33)+1;    //31 unit
            int u99 = value0/10;    //100 unit
            
            //Next selection
            if(PORTAbits.RA3!=1){ 
                setNum++;
                if (setNum==8){
                    setNum = 0;
                }
                //High to output
                unsigned long num = 10000L;   //Delay
                while (--num > 0) {  //Signal change will occur
                        LATCbits.LATC0 =1;
                }
                if (setNum != 0){
                    LATCbits.LATC0 =0;
                }
            }
            //Set value
            switch((char)setNum){
               case 1:
                   set_hours(u24);
                   break;
               case 2:
                  set_minutes(u60);
                  break;
               case 3:
                   set_seconds(u60);
                   break;
               case 4:
                   set_dayname(u7);
                   break;
               case 5:
                  set_months(u12);
                  break;
               case 6:
                   set_days(u31);
                   break;
               case 7:
                   set_years(u99);
                   break;
               default:
                    break;
           }
            char array[2]; //Storage array
            //Clear array
            for(int i=0; i<2; i++){
                array[i]=0;
            }

            lcd_char('T');
            lcd_char(':');
            lcd_char(' ');
            //Print format HH/MM/SS
            //Covert to char array
            sprintf(array, "%lu", get_hours());
            char temp1 = array[0];   //Temporary storage
            char temp2 = array[1];
            if((temp1 != 0) && (temp2 != 0)){  //Check if not null
                lcd_char(temp1);
                lcd_char(temp2);
            }
            else if((temp1 != 0) && (temp2 == 0)){
                lcd_char('0');
                lcd_char(temp1); //Print digit
            }
            else{
                lcd_char('0');
                lcd_char('0');
            }
            lcd_char(':');
            //Covert to char array
            sprintf(array, "%lu", get_minutes());
            temp1 = array[0];   //Temporary storage
            temp2 = array[1];
            if((temp1 != 0) && (temp2 != 0)){  //Check if not null
                lcd_char(temp1);
                lcd_char(temp2);
            }
            else if((temp1 != 0) && (temp2 == 0)){
                lcd_char('0');
                lcd_char(temp1); //Print digit
            }
            else{
                lcd_char('0');
                lcd_char('0');
            }
            lcd_char(':');
            //Covert to char array
            sprintf(array, "%lu", get_seconds());
            temp1 = array[0];   //Temporary storage
            temp2 = array[1];
            if((temp1 != 0) && (temp2 != 0)){  //Check if not null
                lcd_char(temp1);
                lcd_char(temp2);
            }
            else if((temp1 != 0) && (temp2 == 0)){
                lcd_char('0');
                lcd_char(temp1); //Print digit
            }
            else{
                lcd_char('0');
                lcd_char('0');
            }

            //Pick the day name
            lcd_char(' ');
            char naming[3]; //Storage array
            //Clear array
            for(int i=0; i<3; i++){
                naming[i]=0;
            }
            //Pick day name
            switch(get_dayname()){
                case 1:
                    sprintf(naming, "Sun");
                    break;
                case 2:
                   sprintf(naming, "Mon");
                   break;
                case 3:
                    sprintf(naming, "Tue");
                    break;
                case 4:
                    sprintf(naming, "Wed");
                    break;
                case 5:
                   sprintf(naming, "Thu");
                   break;
                case 6:
                    sprintf(naming, "Fri");
                    break;
                case 7:
                    sprintf(naming, "Sat");
                    break;
                default:
                    break;
            }
            lcd_char(' ');
            //Print day name
            for(int i=0; i<3; i++){
                char temp = naming[i];   //Temporary storage
                lcd_char(temp); //Print letter of name
            }

            lcd_command(0xC0);  //Next row

            lcd_char('D');
            lcd_char(':');
            lcd_char(' ');
            //Print format MM/DD/YYYY
            //Covert to char array
            sprintf(array, "%lu", get_months());
            temp1 = array[0];   //Temporary storage
            temp2 = array[1];
            if((temp1 != 0) && (temp2 != 0)){  //Check if not null
                lcd_char(temp1);
                lcd_char(temp2);
            }
            else if((temp1 != 0) && (temp2 == 0)){
                lcd_char('0');
                lcd_char(temp1); //Print digit
            }
            else{
                lcd_char('0');
                lcd_char('0');
            }
            lcd_char('/');
            //Covert to char array
            sprintf(array, "%lu", get_days());
            temp1 = array[0];   //Temporary storage
            temp2 = array[1];
            if((temp1 != 0) && (temp2 != 0)){  //Check if not null
                lcd_char(temp1);
                lcd_char(temp2);
            }
            else if((temp1 != 0) && (temp2 == 0)){
                lcd_char('0');
                lcd_char(temp1); //Print digit
            }
            else{
                lcd_char('0');
                lcd_char('0');
            }
            lcd_char('/');
            //Covert to char array
            lcd_char('2');
            lcd_char('0');
            sprintf(array, "%lu", get_years());
            temp1 = array[0];   //Temporary storage
            temp2 = array[1];
            if((temp1 != 0) && (temp2 != 0)){  //Check if not null
                lcd_char(temp1);
                lcd_char(temp2);
            }
            else if((temp1 != 0) && (temp2 == 0)){
                lcd_char('0');
                lcd_char(temp1); //Print digit
            }
            else{
                lcd_char('0');
                lcd_char('0');
            }
            lcd_char(' ');
            lcd_char(' ');
            if(alarm == 1){
                lcd_char('A');
            }
            else{
                lcd_char('*');
            }
            
            lcd_command(0x02);  //Clear Home
        }
        else{   //Alarm mode
            if(check==0){
                lcd_command(0x01);  //Clear Home
                check++;
            }
            else if(check==1){
                lcd_command(0x02);  //Clear Home
                check--;
            }
            
            //Load adc values
            if(value0 == 0){    //Average out value
                value0 = adcNum0();
            }
            else{
                value0 = ((value0+adcNum0())/2)+1;   //ADC average
            }

            //Convert adc to time unit
            int u24 = value0/42;    //24 unit
            int u60 = value0/17;    //60 unit
            
            //Next selection
            if(PORTAbits.RA3!=1){              
                AsetNum++;
                if (AsetNum==4){
                    AsetNum = 0;
                }
                //High to output
                unsigned long num = 10000L;   //Delay
                while (--num > 0) {  //Signal change will occur
                        LATCbits.LATC1 =1;
                }
                if (AsetNum != 0){
                    LATCbits.LATC1 =0;
                }
            }
            //Set value
            switch((char)AsetNum){
               case 1:
                   set_Ahours(u24);
                   break;
               case 2:
                    set_Aminutes(u60);
                    break;
               case 3:
                   set_Aseconds(u60);
                   break;
               default:
                    break;
           }
            char array[2]; //Storage array
            //Clear array
            for(int i=0; i<2; i++){
                array[i]=0;
            }

            lcd_char('A');
            lcd_char(':');
            lcd_char(' ');
            //Print format HH/MM/SS
            //Covert to char array
            sprintf(array, "%lu", get_Ahours());
            char temp1 = array[0];   //Temporary storage
            char temp2 = array[1];
            if((temp1 != 0) && (temp2 != 0)){  //Check if not null
                lcd_char(temp1);
                lcd_char(temp2);
            }
            else if((temp1 != 0) && (temp2 == 0)){
                lcd_char('0');
                lcd_char(temp1); //Print digit
            }
            else{
                lcd_char('0');
                lcd_char('0');
            }
            lcd_char(':');
            //Covert to char array
            sprintf(array, "%lu", get_Aminutes());
            temp1 = array[0];   //Temporary storage
            temp2 = array[1];
            if((temp1 != 0) && (temp2 != 0)){  //Check if not null
                lcd_char(temp1);
                lcd_char(temp2);
            }
            else if((temp1 != 0) && (temp2 == 0)){
                lcd_char('0');
                lcd_char(temp1); //Print digit
            }
            else{
                lcd_char('0');
                lcd_char('0');
            }
            lcd_char(':');
            //Covert to char array
            sprintf(array, "%lu", get_Aseconds());
            temp1 = array[0];   //Temporary storage
            temp2 = array[1];
            if((temp1 != 0) && (temp2 != 0)){  //Check if not null
                lcd_char(temp1);
                lcd_char(temp2);
            }
            else if((temp1 != 0) && (temp2 == 0)){
                lcd_char('0');
                lcd_char(temp1); //Print digit
            }
            else{
                lcd_char('0');
                lcd_char('0');
            }
        }
        //Check for interrupt
        //if (PORTAbits.RA5 == 1){
        if ((i2c_read(RTC, 0x0F)&0x01) == 1){
            alarmON = 1;
        }
        else{
            alarmON = 0;
        }
        
        //Button trigger alarm
        if (PORTAbits.RA4!=1){
            if(alarm ==0){  //Set alarm
                i2c_write(RTC, 0x0E, 0x05);     //Enable interrupt
                i2c_write(RTC, 0x0A, 0x80);     //Only check hours, minutes, seconds
                alarm = 1;
            }
            else if(alarm ==1){   //Reset alarm
                i2c_write(RTC, 0x0E, 0x04);     //Disable interrupt
                alarm = 0;
            }
            //High to output
            unsigned long num = 10000L;   //Delay
            while (--num > 0) {  //Signal change will occur
                    LATCbits.LATC2 =1;
            }
            LATCbits.LATC2 =0;
        }
        
        if(alarmON == 1){
            while(PORTAbits.RA4==1){
                //Speaker tone
                LATCbits.LATC5=1;
                __delay_ms(0.25);
                LATCbits.LATC5=0;
                __delay_ms(0.25);
            }
            i2c_write(RTC, 0x0F,0x00);
        }
    }
    return;
}

