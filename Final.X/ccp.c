/*
 * CCP functions.
 */
#include "ccp.h"
#include "lcd.h"

//Global Variables
unsigned long f=0;
unsigned long f2=0;
unsigned long f3=0;
int r=0;

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
    CCP1CONbits.MODE=0x05;  //Every edge
    CCP1CONbits.EN = 1;  //Enable ccp
    CCP1IF=0;
    
    
    //Interrupt enable
    CCP1IE = 1;
    PEIE = 1;
    GIE = 1;
}

void __interrupt() ISR(){
  if (CCP1IF){  //If Capture Event Occurs, load f
        TMR1 = 0; //Reset
        PIR6bits.CCP1IF = 0;
      
        while(!PIR6bits.CCP1IF);    //Wait till the pulse is done
        PIR6bits.CCP1IF = 0;
        f = CCPR1;  //First edge 
        
        if(f>=1680){
            r=0;
            __delay_ms(20);
        }
        else if (f>=1410){
            r=1;
            __delay_ms(20);
        }
        else if (f>=890){
            r=2;
            __delay_ms(20);
        }
        
    
    //Reset
    TMR1 = 0;
  }
}


/*
 * Read ccp value and convert to frequency.
 */
/*float ccpNum0(){
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
}*/

//Getter of number of hits
int getr(){
    return r;
}