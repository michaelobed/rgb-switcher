//
//  ctrl.c
//  main_board
//
//  Created by michaelobed on 03/01/2025.
//  
//  Copyright © 2025 Michael Obed.

#include "../../common/ctrl.h"
#include "led.h"
#include "../../common/mem.h"
#include "../../common/sys.h"
#include "timer.h"
#include "../../common/uart.h"

#define CTRL_ASCIITOINPUT(x)            (x - 0x30)
#define CTRL_INPUTTOASCII(x)            (x + 0x30)

static const char ctrlFWVersion[17] = "1.1";
static const uint8_t ctrlFWVersionLenMax = 17;
static const uint8_t ctrlNoInput = 0xff;
static const char ctrlCmdToAscii[Cmd_NumCmds] = {' ', 'p', 'n', 's', 'o', 'c', 'a', 'h', 'v', 'b', 'w', 'r'};
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
    uint8_t auxData = 0;
    ctrlParams replyParams =
    {
        .bytes[0] = CtrlGetCmdAsAscii(Cmd_Ack),
        .bytes[1] = CtrlGetCmdAsAscii(cmd)
    };
    uint8_t replyParamsSize = 0;

    /* When switching inputs, we must deal with the cases where no input is selected and when the first/last
     * input is selected. This flag helps to prevent out-of-bounds silliness. Of course, this property gets
     * deliberately exploited for the Cmd_InputNone case, but is still handled sensibly. */
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
        
        /* No input. */
        case Cmd_InputNone:
            ctrlCurrentInput = 0xff;
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
            break;
        }

        /* Hello! */
        case Cmd_Hello:
            replyParams.bytes[2] = '\n';
            replyParamsSize = 2;
            break;

        /* Report firmware version. */
        case Cmd_FWVersion:
        {
            uint8_t fwVersionLen = strlen(ctrlFWVersion);
            replyParams.bytes[0] = 'a';
            replyParams.bytes[1] = 'v';
            strncpy((char*)&replyParams.bytes[2], ctrlFWVersion, ctrlFWVersionLenMax);
            replyParams.bytes[fwVersionLen + 2] = '\n';
            replyParamsSize = fwVersionLen + 2;
            break;
        }

        /* Prepare to jump to the bootloader from the application. */
        case Cmd_BootloaderArm:
            auxData = 0x00;
            MemWrite(MemAddr_StayInBL, &auxData, 1);
            break;

        /* Reset, usually to jump to bootloader after arming. */
        case Cmd_Reset:
            SysReset();
            break;

        default:
            break;
    }

    if(willSwitchInput)
    {
        /* Reply with ack and command only. */
        if(cmd == Cmd_InputNone)
            replyParamsSize = 2;
        else
        {
            /* Send an acknowledgement, communicating the command and the new input.
             * This could allow for communication with other things in future. */
            replyParamsSize = 3;
            replyParams.bytes[2] = CTRL_INPUTTOASCII(ctrlCurrentInput);
        }
        replyParams.bytes[replyParamsSize] = '\n';

        TimerRequestInputSwitch(ctrlCurrentInput);
    }

    /* Always respond back. */
    UartSendBytes(replyParams.bytes, replyParamsSize + 1);
}