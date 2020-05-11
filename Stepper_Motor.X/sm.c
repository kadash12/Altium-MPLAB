/* 
 * File: sm.c
 * Author: Johnny Li
 * Created on March 19, 2020
 * Description:
 * NOTE: Gauge face values are multiples of 10
 * Design a controller for the provided stepper motor driver
-That can rotate output shaft of motor to any specified position with at least 1 degree precision.
-That can rotate in both the clockwise and counter-clock wise direction.
-That is able to move to said position in under 2 seconds.
-That takes the shortest path to following desired position.
-That utilizes a break beam sensor to locate absolute position.
 * Design a speed gauge
-That uses a stepper motors to control the left gauge face
-That updates displayed voltage at least at 2 Hertz.
-That can translate a pulse stream to miles per hour
 * Design a fuel level gauge
-That uses a stepper motor to control the right gauge face
-The voltage will range from 0V to 5V, should show the percentage of the fuel
-That turns on a LED when the fuel is less than or equal to 10%
 */

//Includes files
#include "adc.h"
#include <math.h>
#include <stdint.h>

//Configuration
#pragma config WDTE = OFF   //Disable watch dog timer
#pragma config LVP = ON      //Enable low voltage programming mode
#define _XTAL_FREQ 4000000  //The default clock is 4MHz so set delay clock by 
                            //same frequency.

//Global Variable
unsigned long value0 = 0;   //Where to store the ADC result
unsigned long currfuel = 0;   //Current fuel level
unsigned long currfreq = 0;   //Current frequency
int step = 1;   //Current step
int stepf = 1;   //Current step
int freq = 0;     //Current frequency
int test = 0;   //Tester variable
int temp = 0;   //Temporary variable
int check = 0;

/*
 * Right Stepper Motor 
 * A3-OPTO2    
 * B3-M2 INT1    
 * B2-M2 INT2    
 * B1-M2 INT3   
 * B0-M2 INT4   
 */
void steptakeR(int dir){
    if(dir == 1){   //cw
        switch((char)step){
            case 1:
                PORTB = 0b1100;
                __delay_ms(2);
                step++;
                break;
            case 2:
                PORTB = 0b1001;
                __delay_ms(2);
                step++;
                break;
            case 3:
                PORTB = 0b0011;
                __delay_ms(2);
                step++;
                break;
            case 4:
                PORTB = 0b0110;
                __delay_ms(2);
                step++;
                break;
            default:
                step = 1;   //Reset step
        }
    }
    else if(dir == 2){   //ccw
        switch((char)step){
            case 1:
                PORTB = 0b1100;
                __delay_ms(2);
                step++;
                break;
            case 2:
                PORTB = 0b0110;
                __delay_ms(2);
                step++;
                break;
            case 3:
                PORTB = 0b0011;
                __delay_ms(2);
                step++;
                break;
            case 4:
                PORTB = 0b1001;
                __delay_ms(2);
                step++;
                break;
            default:
                step = 1;   //Reset step
        }
    }
}

/*
 * Left Stepper Motor 
 * A4-OPTO1
 * D3-M1 INT1
 * D2-M1 INT2
 * D1-M1 INT3
 * D0-M1 INT4
 */
void steptakeL(int dir){
    if(dir == 1){   //cw
        switch((char)stepf){
            case 1:
                PORTD = 0b1100;
                __delay_ms(2);
                stepf++;
                break;
            case 2:
                PORTD = 0b1001;
                __delay_ms(2);
                stepf++;
                break;
            case 3:
                PORTD = 0b0011;
                __delay_ms(2);
                stepf++;
                break;
            case 4:
                PORTD = 0b0110;
                __delay_ms(2);
                stepf++;
                break;
            default:
                stepf = 1;   //Reset step
        }
    }
    else if(dir == 2){   //ccw
        switch((char)stepf){
            case 1:
                PORTD = 0b1100;
                __delay_ms(2);
                stepf++;
                break;
            case 2:
                PORTD = 0b0110;
                __delay_ms(2);
                stepf++;
                break;
            case 3:
                PORTD = 0b0011;
                __delay_ms(2);
                stepf++;
                break;
            case 4:
                PORTD = 0b1001;
                __delay_ms(2);
                stepf++;
                break;
            default:
                stepf = 1;   //Reset step
        }
    }
}

/*
 *  Initialize ccp and timer1
 */
void ccp_init(){
    //Timer setup
    TMR1 = 0;   //Initialize to 0
    T1CKPS0 = 1;
    T1CKPS1 = 1;
    T1CONbits.NOT_SYNC = 0;
    T1CONbits.RD16 = 1;
    TMR1CLKbits.CS=1;
    TMR1IF=0;
    TMR1ON = 1;
    
    //CCP setup
    CCP1PPS=0x12;   //Setup input PPS
    CCPR1=0x00;     //Clear
    CCP1CONbits.MODE=0x05;  //Rising edge
    CCP1CONbits.EN = 1;  //Enable ccp
    
    
    //Interrupt enable
    CCP1IE = 1;
    PEIE = 1;
    GIE = 1;
}

unsigned long f=0;
unsigned long f2=0;
unsigned long f3=0;
/*void __interrupt() ISR(){
  if (CCP1IF){  //If Capture Event Occurs, load f
    f = CCPR1;
    
    //Reset
    CCP1IF = 0;
    //TMR1 = 0;
  }
}*/


/*
 * Read ccp value and convert to frequency.
 */
float ccpNum0(){
    for(int q=0; q<5; q++){
        //wait till pulse signal
        while(!PIR6bits.CCP1IF);
        PIR6bits.CCP1IF=0;  //Reset
        f=CCPR1;  //Puts the timer value
        
        if(CCPR1>60000){
            return f3=0;
        }
        
        while(!PIR6bits.CCP1IF);    //Wait till the pulse is done
        PIR6bits.CCP1IF = 0;
        f2=CCPR1;  //Puts the timer value

        if (f < f2){           //Get frequency
          unsigned long period = f2-f;
          f3=((((float)_XTAL_FREQ/4)/((float)period*8))+f3)/2;
        }
        TMR1 = 0;
    }
    return f3;
}

/*
 * 
 */
void main(){       
    adc_init();     //Initialize ADC ports
    ccp_init();     //Initialize ccp 
    
    TRISAbits.TRISA2 = 0;   //Configure PORTC pin 2 as output   (LED)
    PORTAbits.RA2 = 0;  //Clear and enable
    //Configure PortC and PORTD as output   (Stepper Motor)
    TRISB = 0x0;    //M2 Right
    TRISD = 0x0;    //M1 Left
    //Clear Ports
    PORTB = 0;
    PORTD = 0;
    TRISAbits.TRISA3 = 1;   //Configure PORTA pin 3 as input    (Brake right)
    TRISAbits.TRISA4 = 1;   //Configure PORTA pin 4 as input    (Brake left)
    ANSELA = 0x0;   //Clear and Enable
    TRISCbits.TRISC2 = 1;   //Configure PORTC pin 2 as input    (Frequency)
    ANSELC = 0x0;   //Clear and Enable
    
    //Find absolute position
    while(PORTAbits.RA3!=1){
        steptakeR(1);
    }
    //Move to zero
    for(int t=0; t<245; t++){   //Right
        steptakeR(1);
    }
     //Find absolute position
    while(PORTAbits.RA4!=1){
        steptakeL(1);
    }
    //Move to zero
    for(int t=0; t<245; t++){   //Right
        steptakeL(1);
    }

    //Infinite loop. 
    while(1){           
        //Reset
        int n=0;    //Extra steps
        
        //Right Gauge- Fuel Level
        //Load adc values
        for(int w=0; w<10;w++){
            if(value0 == 0){    //Average out value
                value0 = adcNum0();
            }
            else{
                value0 = ((value0+adcNum0())/2);   //ADC average
           }
        }
        //------------------------------------------------------------------------
        //Determine shortest path
        if(abs(currfuel-value0)>1){
            if((value0<currfuel)&&(temp==0)){
                __delay_ms(100);
                temp=1;
            }
            else{
                int dist =((value0/3)-(currfuel/3)+360)%360;
                if(dist<= 180){
                    //Extra steps for 5V 
                    if(value0>1000){
                        for(n=0; n<77; n++){
                            steptakeR(1);
                            test=1;
                        }
                    }
                    //Go cw
                    for(int t=0; t<((int)value0-currfuel)*2+800/value0; t++){
                        steptakeR(1);
                        if(PORTAbits.RA3==1){   //Cross brake line
                            //Move to 0
                            for(int t=0; t<245; t++){ 
                                steptakeR(1);
                            }
                            //Continue to value
                            for(int t=0; t<abs((int)value0)*2; t++){
                                steptakeR(1);
                                currfuel = value0/2; 
                            }
                            break;
                        }
                    }
                    currfuel = value0;    //New position is now current position
                }
                else if (dist>180){                 
                    //Remove extra steps for 5V 
                    if (test==1){
                        for(n=0; n<77; n++){
                            steptakeR(2);
                            test=0;
                        }
                    }
                    //Go ccw
                    for(int t=0; t<abs(currfuel-(int)value0)*2-400/value0; t++){
                        steptakeR(2);
                        //Adjust for zero
                        if((value0==0)&&(currfuel>1000)){
                            currfuel=currfuel+50;
                        }
                        if(PORTAbits.RA3==1){   //Cross brake line
                            //Move to 10
                            for(int t=0; t<240; t++){ 
                                steptakeR(2);
                            }
                            //Continue to value
                            for(int t=0; t<abs(2048/2-(int)value0)*2; t++){
                                steptakeR(2);
                            }
                            currfuel = value0; 
                            break;
                        }
                    }
                    currfuel = value0;    //New position is now current position 
                }
                temp=0;
            }
        }
            
        //Turn on LED when fuel is below 10%
        if(value0 <= 100){
             LATAbits.LATA2 =1;
        }
        else{
            LATAbits.LATA2 =0;
        }
        //------------------------------------------------------------------------
        freq=ccpNum0()+1;
        if(abs(currfreq-freq)>=1){
            if(freq<5){
                freq=0;
            }
            int dist2 =((int)(freq/0.2778)-(int)(currfreq/0.2778)+360)%360;
            if(dist2<= 180){
                //Go cw
                for(int t=0; t<(freq-currfreq)*20.48+1000/(freq*9)+freq/3; t++){
                    steptakeL(1);
                    if(PORTAbits.RA4==1){   //Cross brake line
                        //Move to 0
                        for(int t=0; t<245; t++){ 
                            steptakeL(1);
                        }
                        //Continue to value
                        for(int t=0; t<abs((int)freq)*20; t++){
                            steptakeL(1);
                        }
                        currfreq = freq;
                        break;
                    }
                }
                currfreq = freq;
            }
            else if (dist2>180){  
                //Go ccw       
                for(int t=0; t<abs(currfreq-(int)freq)*20.48-500/(freq*10); t++){
                    steptakeL(2);
                    if(PORTAbits.RA4==1){   //Cross brake line
                        //Move to 10
                        for(int t=0; t<245; t++){ 
                            steptakeL(2);
                        }
                        //Continue to value
                        for(int t=0; t<abs(2000/2-(int)freq*10)*2; t++){
                            steptakeL(2);
                        }
                        currfreq = freq;
                        break;
                    }
                }
                currfreq = freq;    //New position is now current position */      
            }
        }
    }
    return;
}