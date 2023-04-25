#include <msp430.h>
#include "..\base.h"
#include "TA1.h"
#include "event.h"


#pragma FUNC_ALWAYS_INLINE(TA1_init)
GLOBAL Void TA1_init(Void) {

   CLRBIT(TA1CTL,   MC0 | MC1  // stop mode
                  | TAIE       // disable interrupt
                  | TAIFG);    // clear interrupt flag
   CLRBIT(TA1CCTL0, CM1 | CM0  // no capture mode
                  | CAP        // compare mode
                  | CCIE       // disable interrupt
                  | CCIFG);    // clear interrupt flag
   TA1CCR0  = 96-1;            // set up Compare Register
   TA1EX0   = TAIDEX_7;        // set up expansion register
   TA1CTL   = TASSEL__ACLK     // 614.4 kHz
            | MC__UP           // Up Mode
            | ID__8            // /8
            | TACLR;           // clear and start Timer
   SETBIT(TA1CTL, TAIE);       // enable interrupt
}



#pragma vector = TIMER1_A1_VECTOR
__interrupt Void TIMER1_A1_ISR(Void) {

   /*
    * Der Inhalt der ISR ist zu implementieren
    */
    
    static volatile UInt state1 = 0;
    static volatile UInt cnt1 = 0;
    #define N_BOUNCING 4
    //Button1
    if(TSTBIT(P1IN, BIT0)) {
        if(state1 == 0) {
            if(cnt1 < N_BOUNCING - 1) {
                cnt1++;
            } else if(cnt1 == N_BOUNCING - 1) {
                state1 = 1;
                Event_set(EVENT_BTN1);
                __low_power_mode_off_on_exit();
            }
        } else if(state1 == 1) {
            if(cnt1 < N_BOUNCING - 1) {
                cnt1++;
            }
        }
    } else {
        if(state1 == 0) {
            if(cnt1 > 0) {
                cnt1--;
            }
        } else if(state1 == 1) {
            if(cnt1 > 0) {
                cnt1--;
            } else if(cnt1 == 0) {
                state1 = 0;
            }
        }
    }
    //Button2
    static volatile UInt state2 = 0;
    static volatile UInt cnt2 = 0;
    if(TSTBIT(P1IN, BIT1)) {
        if(state2 == 0) {
            if(cnt2 < N_BOUNCING - 1) {
                cnt2++;
            } else if(cnt2 == N_BOUNCING - 1) {
                state2 = 1;
                Event_set(EVENT_BTN2);
                __low_power_mode_off_on_exit();
            }
        } else if(state2 == 1) {
            if(cnt2 < N_BOUNCING - 1) {
                cnt2++;
            }
        }
    } else {
        if(state2 == 0) {
            if(cnt2 > 0) {
                cnt2--;
            }
        } else if(state2 == 1) {
            if(cnt2 > 0) {
                cnt2--;
            } else if(cnt2 == 0) {
                state2 = 0;
            }
        }
    }



}
