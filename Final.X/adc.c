/*
 * ADC functions.
 */
#include "adc.h"

/*
 * The function will initialize the PORTA pin 0 to be input.
 */
void adc_init(){
    TRISAbits.TRISA0 = 1;  //Configure PORTA pin 0 as input
        
    //Clear and Enable
    ANSELC = 0x0;
}

/*
 * Get the ADC value from PortA pin 0 and convert it to a char.
 */
unsigned long adcNum0() {
    ADCON0 = 0x00;   //Select RA0
    ADPCH = 0;
    ADCON0bits.ADON = 1;    //Enable ADC
    ADCON0bits.GO = 1;  //Set go bit
    
    while(ADCON0bits.GO){};  //Wait til conversion is complete 
    
    ADCON0bits.ADON = 0;    //Disable ADC
    
    //Shift bits of output
    return (ADRES >> 6);
}
