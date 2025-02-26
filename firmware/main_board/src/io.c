//
//  io.c
//  main-board
//
//  Created by michaelobed on 03/01/2025.
//  
//  Copyright © 2025 Michael Obed.

#include "../../common/ctrl.h"
#include "io.h"
#include "../../common/sys.h"
#include "../../common/uart.h"

#define IO_BTNISPRESSED(reg, x)     ((reg & _BV(x)) == 0)
#define IO_BTNDEBOUNCEEND           8192
#define IO_PORTCMASK_BTNS           0x03
#define IO_PORTCMASK_SELEN          0x04
#define IO_PORTCMASK_SEL            0x38

static volatile enum
{
    Btn_None,
    Btn_Prev,
    Btn_Next
} ioBtnIndex = Btn_None;

static uint16_t ioBtnDebounceCtr = 0;

void IoButtonHandler(void)
{
    if(ioBtnIndex == Btn_None)
        ioBtnDebounceCtr = 0;
    else
    {
        if(++ioBtnDebounceCtr < IO_BTNDEBOUNCEEND)
            return;
        
        switch(ioBtnIndex)
        {
            case Btn_None:
                break;

            case Btn_Prev:
                CtrlHandleCmd(Cmd_InputPrev, NULL);
                break;

            case Btn_Next:
                CtrlHandleCmd(Cmd_InputNext, NULL);
                break;
        }
        ioBtnIndex = Btn_None;
    }
}

void IoInit(void)
{
    cli();

    /* Select lines. */
    PORTC &= ~(IO_PORTCMASK_SELEN | IO_PORTCMASK_SEL);
    DDRC |= (IO_PORTCMASK_SELEN | IO_PORTCMASK_SEL);

    /* Onboard buttons (input with pull-ups). Enable interrupts. */
    DDRC &= ~IO_PORTCMASK_BTNS;
    PORTC |= IO_PORTCMASK_BTNS;
    PCMSK1 |= (_BV(PCINT8) | _BV(PCINT9));
    PCIFR &= ~_BV(PCIF1);
    PCICR |= _BV(PCIE1);

    sei();
}

bool IoSelectInput(uint8_t input)
{
    PORTC &= ~_BV(PIN_SELEN);

    /* Out of range outputs are not allowed. We can also use this to say "select none". */
    if(input > 0x07)
        return FALSE;

    PORTC &= ~IO_PORTCMASK_SEL;
    PORTC |= (input << 3);
    PORTC |= _BV(PIN_SELEN);
    return TRUE;
}

void IoTestLed(bool state)
{
    /* TODO. */
}

ISR(PCINT1_vect)
{
    if(IO_BTNISPRESSED(PINC, PIN_BTNNEXT))
        ioBtnIndex = Btn_Next;
    else if(IO_BTNISPRESSED(PINC, PIN_BTNPREV))
        ioBtnIndex = Btn_Prev;
}