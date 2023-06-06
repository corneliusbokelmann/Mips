#include <msp430.h>
#include "Handler.h"
#include "event.h"
#include "UCA1.h"
#include "TA0.h"

#define DIGISIZE 4
#define BASE     10


UChar select;
signed char digits[DIGISIZE];
UChar i;
LOCAL Int cnt;
// ----------------------------------------------------------------------------

GLOBAL Void Button_Handler(Void) {

    if (Event_tst(EVENT_BTN1)) {
        Event_clr(EVENT_BTN1);
        TGLBIT(P2OUT, BIT7);
    }  else if (Event_tst(EVENT_BTN2)) {
        Event_clr(EVENT_BTN2);
        if (++cnt GT MUSTER6) {
           cnt = MUSTER1;
        }
        set_blink_muster(cnt);
     }

    else if (Event_tst(EVENT_BTN3)) {
        Event_clr(EVENT_BTN3);
        select = 0;
        Event_set(EVENT_15);
    } else if (Event_tst(EVENT_BTN4)) {
        Event_clr(EVENT_BTN4);
        select = 1;
        Event_set(EVENT_15);
    } else if (Event_tst(EVENT_BTN5)) {
        Event_clr(EVENT_BTN5);
        select = 2;
        Event_set(EVENT_15);
    } else if (Event_tst(EVENT_BTN6)) {
        Event_clr(EVENT_BTN6);
        select = 3;
        Event_set(EVENT_15);
    }
}

// ----------------------------------------------------------------------------


GLOBAL Void Number_Handler(Void) {
    if (Event_tst(EVENT_15)) {
       Event_clr(EVENT_15);
       if (TSTBIT(P2OUT, BIT7)) {
           digits[select] -= 1;
       } else {
           digits[select] += 1;
       }
       Event_set(EVENT_RUN);
   }
}

// ----------------------------------------------------------------------------

// Datentyp eines Funktionspointers
typedef Void (* VoidFunc)(Void);

// Funktionsprototypen
LOCAL Void State0(Void);
LOCAL Void State1(Void);

// lokale Zustandsvariable
LOCAL VoidFunc state;

LOCAL Void State0(Void) {
    if (Event_tst(EVENT_RUN)) {
        Event_clr(EVENT_RUN);
        i = 1;
        state = State1;
        Event_set(EVENT_DONE);
    }
}

LOCAL Void State1(Void) {
    if (Event_tst(EVENT_DONE)) {
        Event_clr(EVENT_DONE);
        if (i LE 4) {
            UChar index = i - 1;
            if (digits[index] > BASE-1) {
                digits[index] = 0;
                digits[i]++;
            } else if (digits[index] < 0) {
                digits[index] = BASE-1;
                digits[i]--;
            }
            UCA1_emit(i, digits[index]);
            i++;
        } else {
            state = State0;
        }
    }
}

GLOBAL Void AS1108_Handler(Void) {
    (*state)();
}

// ----------------------------------------------------------------------------
GLOBAL Void Handler_init(Void) {
    cnt = MUSTER1;
    digits[0] = 0;
    digits[1] = 0;
    digits[2] = 0;
    digits[3] = 0;
    state = State0;
}
