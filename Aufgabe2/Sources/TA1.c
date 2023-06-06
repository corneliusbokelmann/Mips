#include <msp430.h>
#include "..\base.h"
#include "TA1.h"
#include "event.h"
#include <stdio.h>


#define BUTTONCOUNT 6
#define THRESHHOLD 4

// Button
typedef struct {
    const UChar * const port;
    const UChar mask;
    const TEvent event;
    UInt * const pcnt;
} ButtonStruct;

LOCAL struct {
    UInt idx;
    UInt cnt[BUTTONCOUNT];
    const ButtonStruct *ptr;
} button;

LOCAL const ButtonStruct btn[BUTTONCOUNT] = {
    {(UChar *)(&P1IN), BIT1, EVENT_BTN1, &button.cnt[0]},
    {(UChar *)(&P1IN), BIT0, EVENT_BTN2, &button.cnt[1]},
    {(UChar *)(&P3IN), BIT0, EVENT_BTN3, &button.cnt[2]},
    {(UChar *)(&P3IN), BIT1, EVENT_BTN4, &button.cnt[3]},
    {(UChar *)(&P3IN), BIT2, EVENT_BTN5, &button.cnt[4]},
    {(UChar *)(&P3IN), BIT3, EVENT_BTN6, &button.cnt[5]}
};


#pragma FUNC_ALWAYS_INLINE(TA1_init)
GLOBAL Void TA1_init(Void)
{
    button.idx = 0;
    button.ptr = &btn[0];

    for (UChar i=0; i LE BUTTONCOUNT-1; i++) {
        button.cnt[i] = 0;
    }

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
    register UInt * const pcnt = button.ptr->pcnt;
    if (TSTBIT(*button.ptr->port, button.ptr->mask)) {
        if ((*pcnt) LT THRESHHOLD) {
            if  (++(*pcnt) EQ THRESHHOLD) {
                Event_set(button.ptr->event);
                __low_power_mode_off_on_exit();
            }
        }
    } else {
        if (*pcnt GT 0) {
            (*pcnt)--;
        }
    }

    button.ptr++;
    button.idx++;
    if (button.idx GT BUTTONCOUNT-1) {
        button.idx = 0;
        button.ptr = &btn[0];
    }
    CLRBIT(TA1CTL, TAIFG);
}

