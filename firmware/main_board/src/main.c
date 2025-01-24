//
//  main.c
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
#include "io.h"
#include "led.h"
#include "../../common/sys.h"
#include "timer.h"
#include "../../common/uart.h"

int main(void)
{
    ctrlParams inputZero;

    IoInit();
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