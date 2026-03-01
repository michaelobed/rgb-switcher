//
//  uart.h
//  rgb-switcher
//
//  Created by Michael Obed on 03/01/2025.
//  
//  Copyright © 2025 Michael Obed.

#ifndef uart_h
#define uart_h

#include "sys.h"

#define UART_BUFSIZE                    512

void UartDeInit(void);
uint16_t UartGetBytesAvailable(void);
void UartHexDump(uint8_t* data, uint16_t size);
void UartInit(void);
void UartHandler(void);
void UartPrint(const char* s, ...);
void UartSendBytes(uint8_t* bytes, uint8_t size);

#endif