#include <msp430.h>
#include "Handler.h"
#include "event.h"
#include "UCA1.h"

#define DIGISIZE 4
#define BASE     10 // Basis des Zahlensystems kann zwischen 2 und 16 gewählt werden

static UInt count[4] = {0, 0, 0, 0};

// TODO: Events anpassen

LOCAL Void add(UInt idx) {
    if(count[idx] == BASE - 1) { //Plus
        count[idx] = 0;
        if (idx != 3) {
            add(idx + 1);
        }
    } else {
        count[idx] += 1;
    }
}

LOCAL Void sub(UInt idx) {
    if(count[idx] == 0) {
        count[idx] = BASE - 1;
        if (idx != 3) {
            sub(idx + 1);
        }
    } else {
        count[idx] -= 1;
    }
}

LOCAL Void update_number(UInt idx) {
    if(TSTBIT(P2OUT, BIT7)) {
        sub(idx);
    } else {
        add(idx);
    }

    Event_set(EVENT_DIGI);
}

// ----------------------------------------------------------------------------

GLOBAL Void Button_Handler(Void) {
    static UInt idx = 0;
        UInt evt = EVENT_BTN3 << idx;
        if(Event_tst(evt)) {
            Event_clr(evt);
            update_number(idx);
        }

        idx++;
        if(idx == 4) {
            idx = 0;
        }

}

// ----------------------------------------------------------------------------


GLOBAL Void Number_Handler(Void) {
    if(Event_tst(EVENT_DIGI)) {
        Event_clr(EVENT_DIGI);
        // Digits anzeigen mit UCA1.emit
            //display digits
        if(tst_event(EVENT_DIGI)) {
            clr_event(EVENT_DIGI);
            emit(0x01, count[0]);
            emit(0x02, count[1]);
            emit(0x03, count[2]);
            emit(0x04, count[3]);
            emit(0x0B, 0x03); //display digits
        }
    }
}

// ----------------------------------------------------------------------------



GLOBAL Void AS1108_Handler(Void) {

}

// ----------------------------------------------------------------------------

GLOBAL Void Handler_init(Void) {
 // TODO: main aufsetzen
}

