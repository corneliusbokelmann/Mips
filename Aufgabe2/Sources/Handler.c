#include <msp430.h>
#include "Handler.h"
#include "event.h"
#include "UCA1.h"
#include "TA0.h"

#define DIGISIZE 4
#define BASE     10

UChar position_index;
signed char position[DIGISIZE];
UChar i;
// ----------------------------------------------------------------------------

GLOBAL Void Button_Handler(Void) {

    if (Event_tst(EVENT_BTN3)) {
        Event_clr(EVENT_BTN3);
        position_index = 0;
        Event_set(EVENT_11);
    } else if (Event_tst(EVENT_BTN4)) {
        Event_clr(EVENT_BTN4);
        position_index = 1;
        Event_set(EVENT_11);
    } else if (Event_tst(EVENT_BTN5)) {
        Event_clr(EVENT_BTN5);
        position_index = 2;
        Event_set(EVENT_11);
    } else if (Event_tst(EVENT_BTN6)) {
        Event_clr(EVENT_BTN6);
        position_index = 3;
        Event_set(EVENT_11);
    }
}

// ----------------------------------------------------------------------------


GLOBAL Void Number_Handler(Void) {
    if (Event_tst(EVENT_11)) {
       Event_clr(EVENT_11);
       if (TSTBIT(P2OUT, BIT7)) {
           position[position_index] -= 1;
       } else {
           position[position_index] += 1;
       }
       Event_set(EVENT_RUN);
   }
}

// ----------------------------------------------------------------------------

typedef Void (* VoidFunc)(Void);

LOCAL Void State0(Void);
LOCAL Void State1(Void);
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
            if (position[index] > BASE-1) {
                position[index] = 0;
                position[i]++;
            } else if (position[index] < 0) {
                position[index] = BASE-1;
                position[i]--;
            }
            UCA1_emit(i, position[index]);
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
    position[0] = 0;
    position[1] = 0;
    position[2] = 0;
    position[3] = 0;
    state = State0;
}
