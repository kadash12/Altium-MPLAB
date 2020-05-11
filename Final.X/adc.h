/*
 * Header for adc functions.
 */
#ifndef ADC_H
#define	ADC_H

#include <xc.h>     //Contain the PIC C commands
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void adc_init();
unsigned long adcNum0();    //Get ADC value

#endif	/* ADC_H */

