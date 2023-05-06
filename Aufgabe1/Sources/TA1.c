#include <msp430.h>
#include "..\base.h"
#include "TA1.h"
#include "event.h"
#include <stdio.h>
#define THRSH 4

UInt count_1 = 0;
UInt count_2 = 0;
UInt state_1 = 0;
UInt state_2 = 0;




#pragma FUNC_ALWAYS_INLINE(TA1_init)
GLOBAL Void TA1_init(Void)
{
   CLRBIT(TA1CTL, MC0 | MC1       // stop mode
                      | TAIE      // disable interrupt
                      | TAIFG);   // clear interrupt flag
   CLRBIT(TA1CCTL0, CM1 | CM0     // no capture mode
                        | CAP     // compare mode
                        | CCIE    // disable interrupt
                        | CCIFG); // clear interrupt flag
   TA1CCR0 = 96 - 1;              // set up Compare Register
   TA1EX0 = TAIDEX_7;             // set up expansion register
   TA1CTL = TASSEL__ACLK          // 614.4 kHz
            | MC__UP              // Up Mode
            | ID__8               // /8
            | TACLR;              // clear and start Timer
   SETBIT(TA1CTL, TAIE);          // enable interrupt
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt Void TIMER1_A1_ISR(Void)
{
    if(TSTBIT(P1IN, BIT1) && count_1 LT 5) {
        count_1++;
        } else if(count_1 GT 0) {
            count_1--;
        }
        if (!state_1) {
            if (count_1 EQ THRSH) {
                state_1 = 1;
                // send button event
                Event_set(EVENT_BTN1); // set up event
            }
        } else {
            if (count_1 EQ 0) {
                state_1 = 0;
            }
        }

        //btn2

        if(TSTBIT(P1IN, BIT0) && count_2 LT 5) {
            count_2++;
        } else if(count_2 GT 0) {
            count_2--;
        }
        if (!state_2) {
            if (count_2 EQ THRSH) {
                state_2 = 1;
                // send button event
                Event_set(EVENT_BTN2); // set up event
            }
        } else {
            if (count_2 EQ 0) {
                state_2 = 0;
            }
        }

        CLRBIT(TA1CTL, TAIFG); // clear interrupt flag
        __low_power_mode_off_on_exit(); // restore Active Mode on return
}
