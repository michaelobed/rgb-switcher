//
//  ctrl.c
//  main_board_bootloader
//
//  Created by michaelobed on 01/03/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "../../common/ctrl.h"
#include "../../common/mem.h"
#include "../../common/uart.h"

static uint16_t ctrlWriteAddress = 0x0000;

static uint16_t lrc(uint8_t* data, uint16_t length);
static void writeData(uint8_t* data, uint16_t length);

void CtrlHandleCmd(ctrlCmd cmd, ctrlParams* params)
{
    uint8_t auxData = 0;
    uint16_t bytesAvailable = 0;
    uint16_t lrcRx = 0;
    ctrlParams replyParams =
    {
        .bytes[0] = Cmd_Ack,
        .bytes[1] = cmd,
        .bytes[2] = '\n',
    };
    uint8_t replyParamsSize = 2;

    switch(cmd)
    {
        /* Hello! */
        case Cmd_Hello:
            break;

        /* Write incoming data to application flash.
         * The data starts at byte 1. The last three bytes are a 16-bit lrc and the terminating '\n'.
         * Reject the data and do not respond with the ack if the lrc is invalid. */
        case Cmd_BootloaderWriteData:
            bytesAvailable = UartGetBytesAvailable() - 1;
            // UartPrint(  "%u bytes available, lrc = %04x:",
            //             bytesAvailable,
            //             lrc(params->bytes, bytesAvailable - 3));
            // UartHexDump(params->bytes, bytesAvailable - 3);
            // while(TRUE);
            lrcRx = *(uint16_t*)&params->bytes[bytesAvailable - 3];
            if(lrcRx != (lrc(params->bytes, bytesAvailable - 3)))
                return;
            writeData(params->bytes, bytesAvailable - 3);
            break;

        /* Signal that we won't re-enter the bootloader, then jump to the application. */
        case Cmd_Reset:
            auxData = 0xff;
            MemWrite(MemAddr_StayInBL, &auxData, 1);
            SysReset();
            break;

        default:
            return;
    }

    /* Always respond back if the command is valid. */
    UartSendBytes(replyParams.bytes, replyParamsSize + 1);
}

uint16_t lrc(uint8_t* data, uint16_t length)
{
    uint16_t ret = 0x0000;

    for(uint16_t i = 0; i < length; i++)
        ret += data[i];
    return ret;
}

void writeData(uint8_t* data, uint16_t length)
{
    uint16_t pageStartAddress = ctrlWriteAddress;
    uint16_t tempWord = 0x0000;
    
    /* Erase the page. */
    boot_page_erase(pageStartAddress);
    boot_spm_busy_wait();

    for(uint16_t i = 0; i < length; i += 2)
    {
        /* Fill this page with data (little-endian), then increment the word address. */
        tempWord = data[i];
        tempWord |= (data[i + 1] << 8);
        boot_page_fill((uint16_t)(void*)ctrlWriteAddress, tempWord);
        ctrlWriteAddress += 2;
    }

    /* Write the contents of the page buffer to flash. */
    boot_page_write(pageStartAddress);
    boot_spm_busy_wait();
    boot_rww_enable();
}