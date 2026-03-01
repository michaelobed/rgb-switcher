//
//  mem.h
//  main_board
//
//  Created by michaelobed on 07/02/2025.
//  
//  Copyright © 2025 Michael Obed.

#ifndef mem_h
#define mem_h

#include "../../common/sys.h"

typedef enum
{
    MemAddr_Signature = 0x10,
    MemAddr_ColourInput0 = 0x20,
    MemAddr_ColourInput1 = 0x24,
    MemAddr_ColourInput2 = 0x28,
    MemAddr_ColourInput3 = 0x2c,
    MemAddr_ColourInput4 = 0x30,
    MemAddr_ColourInput5 = 0x34,
    MemAddr_ColourInput6 = 0x38,
    MemAddr_ColourInput7 = 0x3c,
    MemAddr_End = E2END
} MemAddr;

bool MemEmpty(void);
void MemInit(void);
void MemRead(MemAddr addr, void* data, uint8_t length);
void MemWrite(MemAddr addr, void* data, uint8_t length);

#endif