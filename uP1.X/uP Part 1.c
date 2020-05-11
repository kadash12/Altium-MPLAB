/* 
 * File: uP1.c
 * Author: Johnny Li
 * Created on January 28, 2020
 * Description:
 * A microcontroller based system that inputs a digital voltage level controlled
 * by a switch and either flashes an LED at 2 Hz or outputs a 2 kHz square wave 
 * driving a piezo-electric speaker.
 */
//Includes files
#include <xc.h>     //Contain the PIC C commands
#include <stdio.h>
#include <stdlib.h>

//Define
#pragma config WDTE = OFF   //Disable watch dog timer
#pragma config LVP = ON      //Enable low voltage programming mode
#define _XTAL_FREQ 4000000  //The default clock is 4MHz so set delay clock by 
                            //same frequency.

/*
 * Continuously loop/poll for switch input to determine function of the program.
 * If switch is on/set, the LED with blink at 2Hz.
 * If switch is off/close, the speaker will sound at 2kHz.
 * Contain setup of I/O values - PORTA is output while PORTB is input. 
 */
int main() {
    //Configure PortA as output
    TRISA = 0x0;
    //Clear PortA
    PORTA = 0;
    
    //Configure PORTB as input
    TRISB = 0xFF; 
    //Clear and Enable PortB
    ANSELB = 0x0;
    
    //Infinite Loop
    //Since the manual switching is slower than the delays, polling will work as
    //if instant interrupt. 
    while(1){
        //Turning the voltage output of the pin connected to the LED to high and
        //low causes blinking.
       
        //If input PortB pin0 is set, toggle LED on
        if(PORTBbits.RB0==1){
            //Set/turn on LED to output PortA pin 0
            LATAbits.LATA0=1;
            __delay_ms(250); //Delay by 500ms -> 2Hz but half duty cycle so 
                             //250ms
        }
        //Continue LED function if switch PortB pin0 is still set
        if(PORTBbits.RB0==1){
            //Ground/turn off LED to output PortA pin 0
            LATAbits.LATA0=0;
            __delay_ms(250);
        }
        
        //Turning the voltage output of the pin high and low in a digital signal
        //simulate a square wave so when the connected output pin will generate
        //a 2kHz square wave to the speaker if it is toggled fasted enough.
        
        //If input PortB pin0 is close, turn on speaker
        if(PORTBbits.RB0==0){
            //High to output PortA pin 1
            LATAbits.LATA1 =1;
            __delay_ms(0.25); //Delay by 0.5ms -> 2kHz but half duty cycle so 
                              //0.25ms
        }
        //Continue speaker function if switch PortB pin0 is still close
        if(PORTBbits.RB0==0){
            //Low to output PortA pin 1
            LATAbits.LATA1=0;
            __delay_ms(0.25);
        }
    };
    
    //Does not reach return
    return 0;
}

