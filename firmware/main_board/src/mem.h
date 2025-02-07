//
//  mem.h
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

#ifndef mem_h
#define mem_h

#include "../../common/sys.h"

typedef enum
{
    MemAddr_Signature = 0x00,
    MemAddr_ColourInput0 = 0x10,
    MemAddr_ColourInput1 = 0x14,
    MemAddr_ColourInput2 = 0x18,
    MemAddr_ColourInput3 = 0x1c,
    MemAddr_ColourInput4 = 0x20,
    MemAddr_ColourInput5 = 0x24,
    MemAddr_ColourInput6 = 0x28,
    MemAddr_ColourInput7 = 0x2c,
    MemAddr_End = E2END
} MemAddr;

bool MemEmpty(void);
void MemInit(void);
void MemRead(MemAddr addr, void* data, uint8_t length);
void MemWrite(MemAddr addr, void* data, uint8_t length);

#endif