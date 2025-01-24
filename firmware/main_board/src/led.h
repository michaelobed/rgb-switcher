//
//  led.h
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

#ifndef led_h
#define led_h

#include "../../common/sys.h"

#define LED_COLOURNONE                  0x00000000
#define LED_COLOURRED                   0x00ff0000
#define LED_COLOURGREEN                 0x0000ff00
#define LED_COLOURBLUE                  0x000000ff
#define LED_COLOURYELLOW                0x00ffff00
#define LED_COLOURMAGENTA               0x00ff00ff
#define LED_COLOURCYAN                  0x0000ffff
#define LED_COLOURWHITE                 0x00ffffff
#define LED_COLOURORANGE                0x00fe33ba

#define LED_RGBTOCOLOUR(r,g,b)          ((uint32_t)b | ((uint32_t)g << 8) | ((uint32_t)r << 16))

typedef enum
{
    LedOpState_Idle,
    LedOpState_Fade
} LedOpState;

uint32_t LedGetInputColour(uint8_t input);
LedOpState LedHandler(void);
void LedInit(void);
void LedRequestFade(volatile uint32_t colour);
void LedSetInputColour(uint8_t input, volatile uint32_t colour);

#endif