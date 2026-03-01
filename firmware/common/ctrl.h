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

#define CTRL_PARAMSSIZE                 1024

typedef enum
{
    Cmd_None,
    Cmd_InputPrev,
    Cmd_InputNext,
    Cmd_InputSwitch,
    Cmd_InputNone,
    Cmd_ChangeColour,
    Cmd_Ack,
    Cmd_Hello,
    Cmd_FWVersion,
    Cmd_BootloaderArm,
    Cmd_BootloaderWriteData,
    Cmd_Reset,
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