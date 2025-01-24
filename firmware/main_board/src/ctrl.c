//
//  ctrl.c
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

#include "../../common/ctrl.h"
#include "led.h"
#include "../../common/sys.h"
#include "timer.h"
#include "../../common/uart.h"

#define CTRL_ASCIITOINPUT(x)            (x - 0x30)
#define CTRL_INPUTTOASCII(x)            (x + 0x30)

static const uint8_t ctrlNoInput = 0xff;
static const char ctrlCmdToAscii[Cmd_NumCmds] = {' ', 'p', 'n', 's', 'c', 'a', 'h'};
static uint8_t ctrlCurrentInput = ctrlNoInput;

ctrlCmd CtrlGetAsciiAsCmd(uint8_t ch)
{
    for(uint8_t i = 0; i < Cmd_NumCmds; i++)
    {
        if(ch == ctrlCmdToAscii[i])
            return i;
    }
    return Cmd_None;
}

uint8_t CtrlGetCmdAsAscii(ctrlCmd cmd)
{
    return ctrlCmdToAscii[cmd];
}

void CtrlHandleCmd(ctrlCmd cmd, ctrlParams* params)
{
    ctrlParams replyParams =
    {
        .bytes[0] = CtrlGetCmdAsAscii(Cmd_Ack),
        .bytes[1] = CtrlGetCmdAsAscii(cmd)
    };
    uint8_t replyParamsSize = 0;
    bool willReply = FALSE;

    /* When switching inputs, we must deal with the cases where no input is selected and when the first/last
     * input is selected. This flag helps to prevent out-of-bounds silliness. */
    bool willSwitchInput = FALSE;

    switch(cmd)
    {
        /* Previous input. */
        case Cmd_InputPrev:
            if(ctrlCurrentInput == ctrlNoInput)
            {
                ctrlCurrentInput = 0x07;
                willSwitchInput = TRUE;
            }
            else if(ctrlCurrentInput > 0x00)
            {
                ctrlCurrentInput--;
                willSwitchInput = TRUE;
            }
            break;

        /* Next input. */
        case Cmd_InputNext:
            if(ctrlCurrentInput == ctrlNoInput)
            {
                ctrlCurrentInput = 0x00;
                willSwitchInput = TRUE;
            }
            else if(ctrlCurrentInput < 0x07)
            {
                ctrlCurrentInput++;
                willSwitchInput = TRUE;
            }
            break;

        /* Switch to input. */
        case Cmd_InputSwitch:
            ctrlCurrentInput = CTRL_ASCIITOINPUT(params->bytes[0]);
            willSwitchInput = TRUE;
            break;

        /* Change input LED colour. */
        case Cmd_ChangeColour:
        {
            uint8_t input = CTRL_ASCIITOINPUT(params->colourChange.input);

            /* Copy strings so we can null terminate them. */
            char rStr[3];
            char gStr[3];
            char bStr[3];

            strncpy(rStr, params->colourChange.red, 2);
            rStr[2] = '\0';
            strncpy(gStr, params->colourChange.green, 2);
            gStr[2] = '\0';
            strncpy(bStr, params->colourChange.blue, 2);
            bStr[2] = '\0';

            char* rEnd = &rStr[2];
            char* gEnd = &gStr[2];
            char* bEnd = &bStr[2];
            
            uint8_t r = strtoul(rStr, &rEnd, 16);
            uint8_t g = strtoul(gStr, &gEnd, 16);
            uint8_t b = strtoul(bStr, &bEnd, 16);

            LedSetInputColour(input, LED_RGBTOCOLOUR(r, g, b));
            if(input == ctrlCurrentInput)
                LedRequestFade(LedGetInputColour(input));
            
            memcpy(&replyParams.bytes[2], &params->colourChange, 7);
            replyParams.bytes[9] = '\n';
            replyParamsSize = 9;
            willReply = TRUE;
            break;
        }

        /* Hello! */
        case Cmd_Hello:
            replyParams.bytes[2] = '\n';
            replyParamsSize = 2;
            willReply = TRUE;
            break;

        default:
            break;
    }

    if(willSwitchInput)
    {
        replyParamsSize = 3;

        /* Send an acknowledgement, communicating the command and the new input.
         * This could allow for communication with other things in future. */
        replyParams.bytes[2] = CTRL_INPUTTOASCII(ctrlCurrentInput);
        replyParams.bytes[3] = '\n';
        willReply = TRUE;

        TimerRequestInputSwitch(ctrlCurrentInput);
    }

    if(willReply)
        UartSendBytes(replyParams.bytes, replyParamsSize + 1);
}