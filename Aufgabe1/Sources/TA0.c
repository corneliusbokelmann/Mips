#include <msp430.h>
#include "..\base.h"
#include "TA0.h"

/*
 * Man soll sich eine geeignete Datenstruktur überlegen,
 * die eine laufzeiteffiziente Ausführung der ISR ermöglicht.
 */


#define HIGH 0x8000
#define LOW 0x0000
#define ACKFRQ 614.4 // kHz
#define TIMEBASE 250 // ms
#define SCALING ((UInt)(ACKFRQ * TIMEBASE))
#define TICK(t) (((SCALING / 8) / 8) * ((t) / TIMEBASE) - 1) // TODO: proper scaling!!!

const static UInt muster_1[2] = {HIGH | TICK(200), LOW | TICK(50), 0};
const static UInt muster_2[2] = {HIGH | TICK(75), LOW | TICK(75), 0};
const static UInt muster_3[2] = {HIGH | TICK(25), LOW | TICK(25), 0};
const static UInt muster_4[2] = {HIGH | TICK(50), LOW | TICK(200)};
const static UInt muster_5[4] = {HIGH | TICK(50), LOW | TICK(50), HIGH | TICK(50), LOW | TICK(200), 0};
const static UInt muster_6[6] = {HIGH | TICK(50), LOW | TICK(50), HIGH | TICK(50), LOW | TICK(50), HIGH | TICK(50), LOW | TICK(200), 0};

const static UInt *muster[6] = {muster_1, muster_2, muster_3, muster_4, muster_5, muster_6};
const static UInt size[6] = {3,3,3,3,5,7};

static UInt current_pattern_idx;

LOCAL const Int *ptr;


GLOBAL Void set_blink_muster(UInt arg) {
/*
 * Die Funktion muss so erweitert werden,
 * dass ein Blinkmuster selektiert wird.
 * Diese Lösung hängt stark von der gewählten
 * Datenstruktur ab.
 */
    
    current_pattern_idx = arg;
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
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt Void TIMER0_A1_ISR(Void) {

   /*
    * Der Inhalt der ISR ist zu implementieren
    */
    
    UInt cnt = *ptr++;
    
    Int pattern[size[current_pattern_idx]] = muster[current_pattern_idx]
    
    if(TSTBIT(cnt, HIGH)) {
        SETBIT(P2OUT, BIT7);
    } else {
        CLRBIT(P2OUT, BIT7);
    }
    
    CLRBIT(TA0CTL, TAIFG);
    TA0CCR0 = ~HIGH BAND cnt;
    
    if(*ptr EQ 0) {
        ptr = pattern[0];
    }
       
}
