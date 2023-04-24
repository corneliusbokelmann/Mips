#include <msp430.h>
#include "..\base.h"
#include "TA0.h"

/*
 * Man soll sich eine geeignete Datenstruktur überlegen,
 * die eine laufzeiteffiziente Ausführung der ISR ermöglicht.
 */
const static UInt TWINKLE_1[2] = {200, 50};
const static UInt TWINKLE_2[2] = {75, 75};
const static UInt TWINKLE_3[2] = {25, 25};
const static UInt TWINKLE_4[2] = {50, 200};
const static UInt TWINKLE_5[4] = {50, 50, 50, 200};
const static UInt TWINKLE_6[6] = {50, 50, 50, 50, 50, 200};

const static UInt *TIME_FOR_TWINKLE[6] = {TWINKLE_1, TWINKLE_2, TWINKLE_3, TWINKLE_4, TWINKLE_5, TWINKLE_6};
const static UInt size[6] = {2,2,2,2,4,6};

static UInt twinkle_pattern;
static UInt current_twinkle_pattern;
static UInt twinkle_phase_count = 0;
static UInt twinkle_time_count = 0;
static UInt period_limit;

GLOBAL Void set_blink_muster(UInt arg) {
/*
 * Die Funktion muss so erweitert werden,
 * dass ein Blinkmuster selektiert wird.
 * Diese Lösung hängt stark von der gewählten
 * Datenstruktur ab.
 */
    twinkle_pattern = arg;
}

#pragma FUNC_ALWAYS_INLINE(TA0_init)
GLOBAL Void TA0_init(Void) {

   CLRBIT(TA0CTL, MC0 | MC1   // stop mode
                  | TAIE      // disable interrupt
                  | TAIFG);   // clear interrupt flag
   CLRBIT(TA0CCTL0, CM1 | CM0 // no capture mode
                  | CAP       // compare mode
                  | CCIE      // disable interrupt
                  | CCIFG);   // clear interrupt flag
   TA0CCR0  = 0;              // set up Compare Register
   TA0EX0   = TAIDEX_7;       // set up expansion register
   TA0CTL   = TASSEL__ACLK    // 614.4 kHz
            | MC__UP          // Up Mode
            | ID__8           // /8
            | TACLR;          // clear and start Timer
   SETBIT(TA0CTL, TAIE        // enable interrupt
                | TAIFG);     // set interrupt flag

   twinkle_pattern = 0;
      current_twinkle_pattern = twinkle_pattern;
      period_limit = TIME_FOR_TWINKLE[current_twinkle_pattern][0];
      SETBIT(P1OUT, BIT2);
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt Void TIMER0_A1_ISR(Void) {
    //Button entprellen
    static volatile UInt bouncing_state1 = 0;
    static volatile UInt cnt1 = 0;
    #define N_BOUNCING 4
    //Button1
    if(TSTBIT(P1IN, BIT0)) {
        if(bouncing_state1 == 0) {
            if(cnt1 < N_BOUNCING - 1) {
                cnt1++;
            } else if(cnt1 == N_BOUNCING - 1) {
                bouncing_state1 = 1;
                set_event(EVENT_BTN1);
                __low_power_mode_off_on_exit();
            }
        } else if(bouncing_state1 == 1) {
            if(cnt1 < N_BOUNCING - 1) {
                cnt1++;
            }
        }
    } else {
        if(bouncing_state1 == 0) {
            if(cnt1 > 0) {
                cnt1--;
            }
        } else if(bouncing_state1 == 1) {
            if(cnt1 > 0) {
                cnt1--;
            } else if(cnt1 == 0) {
                bouncing_state1 = 0;
            }
        }
    }
    //Button2
    static volatile UInt bouncing_state2 = 0;
    static volatile UInt cnt2 = 0;
    if(TSTBIT(P1IN, BIT1)) {
        if(bouncing_state2 == 0) {
            if(cnt2 < N_BOUNCING - 1) {
                cnt2++;
            } else if(cnt2 == N_BOUNCING - 1) {
                bouncing_state2 = 1;
                set_event(EVENT_BTN2);
                __low_power_mode_off_on_exit();
            }
        } else if(bouncing_state2 == 1) {
            if(cnt2 < N_BOUNCING - 1) {
                cnt2++;
            }
        }
    } else {
        if(bouncing_state2 == 0) {
            if(cnt2 > 0) {
                cnt2--;
            }
        } else if(bouncing_state2 == 1) {
            if(cnt2 > 0) {
                cnt2--;
            } else if(cnt2 == 0) {
                bouncing_state2 = 0;
            }
        }
    }

    //blinken
    twinkle_time_count += 1;
    //Schaltzyklus beendet?
    if(twinkle_time_count == period_limit) {
        TGLBIT(P1OUT, BIT2);
        twinkle_time_count = 0;
        twinkle_phase_count += 1;
        //Ende des Musters erreicht
        if (twinkle_phase_count == size[current_twinkle_pattern]) {
            twinkle_phase_count = 0;
            //Falls Taster gedrückt, nächstes Muster
            current_twinkle_pattern = twinkle_pattern;
        }
        period_limit = TIME_FOR_TWINKLE[current_twinkle_pattern][twinkle_phase_count];
    }

   /*
    * Der Inhalt der ISR ist zu implementieren
    */
}
