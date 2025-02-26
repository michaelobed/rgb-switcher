//
//  io.h
//  main-board
//
//  Created by michaelobed on 03/01/2025.
//  
//  Copyright © 2025 Michael Obed.

#ifndef io_h
#define io_h

#include "../../common/sys.h"

#define PIN_SELEN                       PC2
#define PIN_SEL0                        PC3
#define PIN_SEL1                        PC4
#define PIN_SEL2                        PC5

#define PIN_LEDR                        PD6
#define PIN_LEDB                        PD3
#define PIN_LEDG                        PD5

#define PIN_BTNPREV                     PC0
#define PIN_BTNNEXT                     PC1

#define PIN_MOSI                        PB3
#define PIN_MISO                        PB4
#define PIN_SCK                         PB5
#define PIN_SS                          PB2

#define PIN_UARTRX                      PD0
#define PIN_UARTTX                      PD1

void IoButtonHandler(void);
void IoInit(void);
bool IoSelectInput(uint8_t input);
void IoTestLed(bool state);

#endif