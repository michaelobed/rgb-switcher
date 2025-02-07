//
//  mem.c
//  main-board
//
//  Created by michaelobed on 07/02/2025.
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

#include "mem.h"

static bool memEmpty = TRUE;
static const uint32_t memSignature = 0x2b005ad1;

static void memEraseAll(void);

static void memEraseAll(void)
{
    for(uint8_t* i = 0x00; i < (uint8_t*)MemAddr_End; i++)
        eeprom_write_byte(i, 0xff);
}

void MemInit(void)
{
    /* Check for the 4-byte signature.
     * If it is correct, we're good. If not, erase everything and reinitialise with the signature. */
    uint32_t memReadValue = 0xffffffff;
    eeprom_read_block(&memReadValue, (const void*)MemAddr_Signature, 4);

    if(memReadValue == memSignature)
        memEmpty = FALSE;
    else
    {
        cli();
        for(uint8_t* i = 0x00; i < (uint8_t*)MemAddr_End; i++)
            eeprom_update_byte(i, 0xff);
        eeprom_update_dword((void*)MemAddr_Signature, memSignature);
        sei();
    }
}

bool MemEmpty(void)
{
    return memEmpty;
}

void MemRead(MemAddr addr, void* data, uint8_t length)
{
    eeprom_read_block(data, (const void*)addr, length);
}

void MemWrite(MemAddr addr, void* data, uint8_t length)
{
    cli();
    eeprom_update_block(data, (void*)addr, length);
    memEmpty = FALSE;
    sei();
}