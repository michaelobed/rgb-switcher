//
//  led.c
//  main-board
//
//  Created by michaelobed on 03/01/2025.
//  
//  Copyright © 2025 Michael Obed.

#include "io.h"
#include "led.h"
#include "mem.h"
#include "../../common/sys.h"
#include "../../common/uart.h"

#define LED_PORTDMASK                   0x68

#define LED_EXTRACTRED(x)               (uint8_t)((x >> 16) & 0xff)
#define LED_EXTRACTGREEN(x)             (uint8_t)((x >> 8) & 0xff)
#define LED_EXTRACTBLUE(x)              (uint8_t)(x & 0xff)

static volatile uint32_t ledColourCurrent = LED_COLOURNONE;
static volatile uint32_t ledColourNext = LED_COLOURNONE;
static const MemAddr ledColourToMemAddr[8] =
{
    MemAddr_ColourInput0,
    MemAddr_ColourInput1,
    MemAddr_ColourInput2,
    MemAddr_ColourInput3,
    MemAddr_ColourInput4,
    MemAddr_ColourInput5,
    MemAddr_ColourInput6,
    MemAddr_ColourInput7
};
static uint32_t ledInputColours[8] =
{
    LED_COLOURRED,
    LED_COLOURGREEN,
    LED_COLOURBLUE,
    LED_COLOURCYAN,
    LED_COLOURMAGENTA,
    LED_COLOURYELLOW,
    LED_COLOURWHITE,
    LED_COLOURORANGE
};
volatile LedOpState ledOpState = LedOpState_Idle;

static void ledSetColour(volatile uint32_t colour);

uint32_t LedGetInputColour(uint8_t input)
{
    return ledInputColours[input];
}

LedOpState LedHandler(void)
{
    /* Change this to change fade speed. */
    const float fadeFactor = 0.05f;

    switch(ledOpState)
    {
        case LedOpState_Idle:
            break;

        case LedOpState_Fade:
        {
            uint8_t currentB = LED_EXTRACTBLUE(ledColourCurrent);
            uint8_t currentG = LED_EXTRACTGREEN(ledColourCurrent);
            uint8_t currentR = LED_EXTRACTRED(ledColourCurrent);
            int16_t diffB = (LED_EXTRACTBLUE(ledColourNext) - LED_EXTRACTBLUE(ledColourCurrent));
            int16_t diffG = (LED_EXTRACTGREEN(ledColourNext) - LED_EXTRACTGREEN(ledColourCurrent));
            int16_t diffR = (LED_EXTRACTRED(ledColourNext) - LED_EXTRACTRED(ledColourCurrent));

            currentB += (diffB * fadeFactor);
            currentG += (diffG * fadeFactor);
            currentR += (diffR * fadeFactor);

            ledColourCurrent = LED_RGBTOCOLOUR(currentR, currentG, currentB);
            
            if(ledColourCurrent == ledColourNext)
                ledOpState = LedOpState_Idle;
            break;
        }
    }

    ledSetColour(ledColourCurrent);
    return ledOpState;
}

void LedInit(void)
{
    /* LED I/O. */
    PORTD &= ~LED_PORTDMASK;
    DDRD |= LED_PORTDMASK;
    
    /* R and G channels on Timer0. */
    TCNT0 = 0x00;
    OCR0A = 0x00;
    OCR0B = 0x00;
    TCCR0A = (_BV(COM0A1) | _BV(COM0B1) | _BV(WGM00));
    TCCR0B = _BV(CS00);

    /* B channel on Timer2B. */
    TCNT2 = 0x00;
    OCR2B = 0x00;
    TCCR2A = (_BV(COM2B1) | _BV(WGM20));
    TCCR2B = _BV(CS20);

    /* Fetch colours from EEPROM, or write default ones if they don't already exist. */
    if(MemEmpty())
    {
        for(uint8_t i = 0; i < 8; i++)
            MemWrite(ledColourToMemAddr[i], &ledInputColours[i], 4);
    }
    else
    {
        for(uint8_t i = 0; i < 8; i++)
            MemRead(ledColourToMemAddr[i], &ledInputColours[i], 4);
    }
}

void LedRequestFade(volatile uint32_t colour)
{
    ledColourNext = colour;
    ledOpState = LedOpState_Fade;
}

static void ledSetColour(volatile uint32_t colour)
{
    /* Format is xRGB (most significant byte is ignored). */
    OCR0A = LED_EXTRACTRED(colour);
    OCR0B = LED_EXTRACTGREEN(colour);
    OCR2B = LED_EXTRACTBLUE(colour);
}

void LedSetInputColour(uint8_t input, volatile uint32_t colour)
{
    ledInputColours[input] = colour;
    MemWrite(ledColourToMemAddr[input], &ledInputColours[input], 4);
}