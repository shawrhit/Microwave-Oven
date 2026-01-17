#include <xc.h>

/*
 * Timer2 setup for 1-second ticks via ISR.
 *
 * Prescaler 1:16, PR2=250. Combined with ISR counting to 1250,
 * yields ~1 second intervals at 20MHz. Timer2 remains off until cooking begins.
 */

void init_timer2(void)
{
    /* Selecting the scale as 1:16 */
    T2CKPS0 = 1;
    T2CKPS1 = 1;

    /* Loading the Pre Load register with 250 */
    PR2 = 250;  //TMR2 -> 0 to 250
    
    /* Clear Timer2 interrupt flag and enable its interrupt */
    TMR2IF = 0;
    TMR2IE = 1;

    /* Enable peripheral and global interrupts */
    PEIE = 1; // Peripheral Interrupt Enable
    GIE  = 1; // Global Interrupt Enable

    /* Keep Timer2 off initially; start it when cooking begins */
    TMR2ON = 0;
}