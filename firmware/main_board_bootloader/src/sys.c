//
//  sys.c
//  main_board_bootloader
//
//  Created by Michael Obed on 01/03/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "../../common/sys.h"

void SysReset(void)
{
    /* Move interrupt vector offset back to application base address, then jump! */
    MCUCR |= _BV(IVCE);
    MCUCR = 0x00;
    asm volatile("jmp 0x0000");
    while(TRUE);
}