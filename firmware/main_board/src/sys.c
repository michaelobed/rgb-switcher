//
//  sys.c
//  main_board
//
//  Created by Michael Obed on 01/03/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "../../common/sys.h"

void SysReset(void)
{
    /* Just enable the watchdog and don't pet it. */
    WDTCSR |= _BV(WDE);
}