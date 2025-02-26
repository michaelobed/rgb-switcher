//
//  timer.c
//  main-board
//
//  Created by michaelobed on 03/01/2025.
//  
//  Copyright © 2025 Michael Obed.

#include "io.h"
#include "led.h"
#include "../../common/sys.h"
#include "timer.h"

static uint8_t timerSwInput = 0x00;
static volatile enum
{
    SwState_Idle,
    SwState_WaitingForLed,
    SwState_Switching
} timerSwState;

void TimerInit(void)
{
    /* Service LED animations at roughly 100 Hz. */
    cli();
    TCNT1 = 0x00;
    OCR1A = 155;
    TCCR1B = (_BV(WGM12) | _BV(CS12));
    TIFR1 &= ~_BV(OCF1A);
    TIMSK1 |= _BV(OCIE1A);
    sei();
}

void TimerRequestInputSwitch(uint8_t input)
{
    timerSwInput = input;
    LedRequestFade(LED_COLOURNONE);
    timerSwState = SwState_WaitingForLed;
}

ISR(TIMER1_COMPA_vect)
{
    LedOpState led = LedHandler();

    switch(timerSwState)
    {
        /* Nothing to do. */
        case SwState_Idle:
            break;

        /* Waiting for LedHandler to become idle as it fades out the current input. */
        case SwState_WaitingForLed:
            if(led == LedOpState_Idle)
                timerSwState = SwState_Switching;
            break;

        case SwState_Switching:
            IoSelectInput(timerSwInput);
            LedRequestFade(LedGetInputColour(timerSwInput));
            timerSwState = SwState_Idle;
            break;
    }
}