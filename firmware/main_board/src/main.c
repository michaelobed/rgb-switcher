//
//  main.c
//  main-board
//
//  Created by michaelobed on 03/01/2025.
//  
//  Copyright © 2025 Michael Obed.

#include "../../common/ctrl.h"
#include "io.h"
#include "led.h"
#include "mem.h"
#include "../../common/sys.h"
#include "timer.h"
#include "../../common/uart.h"

int main(void)
{
    ctrlParams inputZero;

    IoInit();
    MemInit();
    LedInit();
    TimerInit();
    UartInit();

    /* Select input 0 to avoid invalid state of front indicator LEDs. */
    inputZero.bytes[0] = '0';
    CtrlHandleCmd(Cmd_InputSwitch, &inputZero);

    while(TRUE)
    {
        UartHandler();
        IoButtonHandler();
    }
}