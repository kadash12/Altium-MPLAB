/* 
 * File:   final_main.c
 * Author: Johnny Li
 * Created on April 22, 2020
 * Description:
 * Final design will mimic an infrared (IR) remote receiver such as those used 
 * for TV remotes. I will be using the given microcontroller, PIC18F47K40, from 
 * the course to process inputs from several IR remotes to change the ?channels? 
 * displayed on the LCD. This is done by using a photodiode detect incoming IR 
 * signals and the capture mode on the microcontroller to read the signal. 
 * There will be a I2C timer, being able to set the time and an alarm to an LED 
 * indicate that a mark show time is now happening. The time and alarm will be 
 * set by a digital input from a switch to change time/alarm mode and button to 
 * enable or disable alarm. A potentiometer will be used to alter the voltage 
 * value to the microcontroller ADC to select the values. The time, channel, and
 * alarm status will be displayed on the LCD. If the alarm has triggered, a 
 * digital output from the microcontroller will be set to turn on an LED. An 
 * external DAC using a SPI interface will be used to drive a piezoelectric 
 * speaker for a tone relative to which channel is on.
 */

//Includes files
#include "adc.h"
#include "lcd.h"
#include "i2c.h"
#include "timer.h"
#include "dac.h"
#include "ccp.h"

//Configuration
#pragma config WDTE = OFF   //Disable watch dog timer
#pragma config LVP = ON      //Enable low voltage programming mode

//Global Variable
unsigned long value0 = 0;   //Where to store the ADC result

/*
 *
 */
void main() {
    int setNum=0;   //Switch case selection
    int AsetNum=0;   //Alarm switch case selection
    int alarm = 0;  //alarm flag
    int check = 0;  //Check flag
    int alarmON = 0;    //Speaker alarm on 
    int channel=0;  //Channel number
    
    adc_init();     //Initialized ADC ports
    lcd_init();     //Initialize LCD screen, note this takes care of PORTC I/0 
                    //direction  
    i2c_init();     //Initialized i2c
    rtc_init();     //Initialized rtc
    spi_init();     //Initialized spi
    ccp_init();     //Initialize ccp
    
    TRISAbits.TRISA2 = 1;  //Configure PORTA pin 2 as input (switch1)
    TRISAbits.TRISA3 = 1;  //Configure PORTA pin 3 as input (button1)
    TRISAbits.TRISA4 = 1;  //Configure PORTA pin 4 as input (button2)
    TRISAbits.TRISA5 = 1;  //Configure PORTA pin 5 as input (alarm)
    ANSELA = 0x0;   //Clear and Enable
    TRISAbits.TRISA1 = 0; //Configure PORTA pin 1 as output
    PORTAbits.RA1 = 0;  //Clear and enable
    
    TRISCbits.TRISC0 = 0;   //Configure PORTC pin 0 as output
    PORTCbits.RC0 = 0;  //Clear and enable
    TRISCbits.TRISC1 = 0;   //Configure PORTC pin 1 as output
    PORTCbits.RC1 = 0;  //Clear and enable
    TRISCbits.TRISC2 = 1;   //Configure PORTC pin 2 as input
    PORTCbits.RC2 = 0;  //Clear and enable
    TRISCbits.TRISC5 = 0;   //Configure PORTC pin 5 as output
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
            
            //Next selection
            if(PORTAbits.RA3!=1){ 
                setNum++;
                if (setNum==4){
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
            sprintf(array, "%u", get_hours());
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
            sprintf(array, "%u", get_minutes());
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
            sprintf(array, "%u", get_seconds());
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

            //Alarm setting
            lcd_char(' ');
            lcd_char(' ');
            lcd_char(' ');
            if(alarm == 1){
                lcd_char('A');
                lcd_char('S');
            }
            else{
                lcd_char('*');
                lcd_char('*');
            }
            
            //Set value
           channel=getr();
            
            //Covert to char array
            sprintf(array, "%u", channel);
            temp1 = array[0];   //Temporary storage
            //Channel output
            lcd_command(0xC0);  //Next row
            
            lcd_char('C');
            lcd_char('H');
            lcd_char(':');
            lcd_char(' ');
            if((temp1 != 0)){  //Check if not null
                lcd_char(temp1);
            }
            else {
                lcd_char('0');
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
            sprintf(array, "%u", get_Ahours());
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
            sprintf(array, "%u", get_Aminutes());
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
            sprintf(array, "%u", get_Aseconds());
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
                    LATAbits.LATA1 =1;
            }
            LATAbits.LATA1 =0;
        }
        
        if(alarmON == 1 && alarm == 1){
           while(PORTAbits.RA4==1){
                //Speaker tone
                LATCbits.LATC5=1;
                __delay_ms(0.25);
                LATCbits.LATC5=0;
                __delay_ms(0.25);
            }
           i2c_write(RTC, 0x0F,0x00);
        }
        
        //Channel DAC output
        switch(channel){
           case 0:
               for(int y=0; y<2; y++){
                    for (int x = 0; x < 50; x++){
                         unsigned char volatile outv = (int)(sin[x]);     //Adjust to voltage
                         spi_write(outv);
                         __delay_ms(1);
                    }
               }
               break;
           case 1:
               for(int y=0; y<2; y++){
                    for (int x = 0; x < 50; x++){
                         unsigned char volatile outv = (int)(triangle[x]);     //Adjust to voltage
                         spi_write(outv);
                         __delay_ms(1);
                    }
               }
              break;
           case 2:
               for(int y=0; y<2; y++){
                for (int x = 0; x < 50; x++){
                     unsigned char volatile outv = (int)(sawtooth[x]);     //Adjust to voltage
                     spi_write(outv);
                     __delay_ms(1);
                }
               }
               break;
           default:
                break;
        }
    }
    return;
}

