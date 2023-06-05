#include <msp430.h>
#include "..\base.h"
#include "TA1.h"
#include "event.h"
#include <stdio.h>
#define THRSH 4

UInt count = 0;
UInt state = 0;


// TODO: Datenstruktur für buttons mit passenden Events. evtl. mit passenden Events
typedef struct B {
    const UInt *port;
    const UInt mask;
    UInt state;
    UInt count;
} Button;

static UInt current_button_index = 0;

Button buttons[6] = {
    {(UInt*)&P1IN, BIT0, 0, 0},
    {(UInt*)&P1IN, BIT1, 0, 0},
    {(UInt*)&P3IN, BIT0, 0, 0},
    {(UInt*)&P3IN, BIT1, 0, 0},
    {(UInt*)&P3IN, BIT2, 0, 0},
    {(UInt*)&P3IN, BIT3, 0, 0}
};


#pragma FUNC_ALWAYS_INLINE(TA1_init)
GLOBAL Void TA1_init(Void)
{
    CLRBIT(TA1CTL, MC0 | MC1       // stop mode
           | TAIE// disable interrupt
           | TAIFG);// clear interrupt flag
    CLRBIT(TA1CCTL0, CM1 | CM0     // no capture mode
           | CAP// compare mode
           | CCIE// disable interrupt
           | CCIFG);// clear interrupt flag
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
    Button *currentButton = &buttons[current_button_index];
    // umbauen für datenstruktur
    if (TSTBIT(*currentButton->port, currentButton->mask) && currentButton->count LT 5)
    {
        currentButton->count++;
    }
    else if (currentButton->count GT 0)
    {
        currentButton->count--;
    }
    if (!currentButton->state)
    {
        if (currentButton->count EQ THRSH)
        {
            currentButton->state = 1;
            // send button event
            Event_set(EVENT_BTN1  << current_button_index); // set up event
        }
    }
    else
    {
        if (currentButton->count EQ 0)
        {
            currentButton->state = 0;
        }
    }
    current_button_index += 1;
        if(current_button_index == 6) {
            current_button_index = 0;
    }
    CLRBIT(TA1CTL, TAIFG); // clear interrupt flag
    __low_power_mode_off_on_exit(); // restore Active Mode on return
}
