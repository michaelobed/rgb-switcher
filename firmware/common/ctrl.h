//
//  ctrl.h
//  rgb-switcher
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

#ifndef ctrl_h
#define ctrl_h

#include "sys.h"

#define CTRL_PARAMSSIZE                 12

typedef enum
{
    Cmd_None,
    Cmd_InputPrev,
    Cmd_InputNext,
    Cmd_InputSwitch,
    Cmd_ChangeColour,
    Cmd_Ack,
    Cmd_Hello,
    Cmd_NumCmds
} ctrlCmd;

typedef union
{
    uint8_t bytes[CTRL_PARAMSSIZE];
    struct
    {
        char input;
        char red[2];
        char green[2];
        char blue[2];
    } colourChange;
} ctrlParams;

ctrlCmd CtrlGetAsciiAsCmd(uint8_t ch);
uint8_t CtrlGetCmdAsAscii(ctrlCmd cmd);
void CtrlHandleCmd(ctrlCmd cmd, ctrlParams* params);

#endif