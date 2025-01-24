//
//  io.h
//  main-board
//
//  Created by michaelobed on 03/01/2025.
//  
//  Copyright © 2025 Michael Obed.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see https://www.gnu.org/licenses/.

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