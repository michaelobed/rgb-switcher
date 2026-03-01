//
//  main.c
//  main_board_bootloader
//
//  Created by michaelobed on 01/03/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "../../common/mem.h"
#include "../../common/sys.h"
#include "../../common/uart.h"

int main(void)
{
    uint8_t stayInBl = 0x00;

    UartInit();

    /* Read the jump-to-bootloader byte. If asserted (0x00), we stay here, else we jump to the application. */
    MemRead(MemAddr_StayInBL, &stayInBl, 1);
    if(stayInBl == 0xff)
    {
        UartDeInit();
        SysReset();
    }

    /* Disable the WDT, then move interrupt vectors to be offset from bootloader section of flash rather than application section. */
    MCUCR |= _BV(IVCE);
    MCUCR = 0x02;
    MCUSR &= ~_BV(WDRF);
    wdt_disable();
    cli();
    boot_spm_interrupt_disable();
    sei();

    while(TRUE)
        UartHandler();
}