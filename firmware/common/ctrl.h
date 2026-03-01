//
//  ctrl.h
//  rgb-switcher
//
//  Created by michaelobed on 03/01/2025.
//  
//  Copyright © 2025 Michael Obed.

#ifndef ctrl_h
#define ctrl_h

#include "sys.h"

#define CTRL_PARAMSSIZE                 512

typedef enum
{
    Cmd_None,
    Cmd_InputPrev = 'p',
    Cmd_InputNext = 'n',
    Cmd_InputSwitch = 's',
    Cmd_InputNone = 'o',
    Cmd_ChangeColour = 'c',
    Cmd_Ack = 'a',
    Cmd_Hello = 'h',
    Cmd_FWVersion = 'v',
    Cmd_BootloaderArm = 'b',
    Cmd_BootloaderWriteData = 'w',
    Cmd_Reset = 'r'
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

void CtrlHandleCmd(ctrlCmd cmd, ctrlParams* params);

#endif