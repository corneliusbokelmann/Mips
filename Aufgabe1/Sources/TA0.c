#include <msp430.h>
#include "..\base.h"
#include "TA0.h"

LOCAL const UChar muster_1[] = {8, 2, 0}; // Toggle-Wert / Timebase (250ms)
LOCAL const UChar muster_2[] = {3, 3, 0};
LOCAL const UChar muster_3[] = {1, 1, 0};
LOCAL const UChar muster_4[] = {2, 8, 0};
LOCAL const UChar muster_5[] = {2, 2, 2, 8, 0};
LOCAL const UChar muster_6[] = {2, 2, 2, 2, 2, 8, 0};

LOCAL const UChar * const pattern[] = {
    &muster_1[0],
    &muster_2[0],
    &muster_3[0],
    &muster_4[0],
    &muster_5[0],
    &muster_6[0]
};

static UChar muster_index;
static UChar interrupt_counter;
static UChar pattern_index;
static UChar current_muster_value;
static UChar updated_pattern_index;

GLOBAL Void set_blink_muster(UInt arg)
{
    updated_pattern_index = arg;
}

#pragma FUNC_ALWAYS_INLINE(TA0_init)
GLOBAL Void TA0_init(Void)
{
        muster_index = 0;
        interrupt_counter = 0;
        pattern_index = 0;
        current_muster_value = 0;
        updated_pattern_index = 0;
   CLRBIT(TA0CCR0, CCIFG);        // clear CCI flag
   CLRBIT(TA0CTL, MC0 | MC1       // stop mode
                      | TAIE      // disable interrupt
                      | TAIFG);   // clear interrupt flag
   CLRBIT(TA0CCTL0, CM1 | CM0     // no capture mode
                        | CAP     // compare mode
                        | CCIE    // disable interrupt
                        | CCIFG); // clear interrupt flag
   TA0CCR0 = 2400-1;                   // set up Compare Register
   TA0EX0 = TAIDEX_7;             // set up expansion register
   TA0CTL = TASSEL__ACLK          // 614.4 kHz
            | MC__UP              // Up Mode
            | ID__8               // /8
            | TACLR;              // clear and start Timer
   SETBIT(TA0CTL, TAIE            // enable interrupt
                      | TAIFG);   // set interrupt flag
   SETBIT(P1OUT, BIT2);
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt Void TIMER0_A1_ISR(Void)
{
            current_muster_value = pattern[pattern_index][muster_index];
            interrupt_counter++;

            if (interrupt_counter EQ current_muster_value) {
                TGLBIT(P1OUT, BIT2);
                muster_index++;
                interrupt_counter = 0;
                if (pattern[pattern_index][muster_index] EQ 0) {
                    muster_index = 0;
                    pattern_index = updated_pattern_index;
                }
            }
            CLRBIT(TA0CTL, TAIFG);
}
