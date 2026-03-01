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

#define UART_BUFSIZE                    1024

void UartHexDump(uint8_t* data, uint8_t size);
void UartInit(void);
void UartHandler(void);
void UartPrint(const char* s, ...);
void UartSendBytes(uint8_t* bytes, uint8_t size);

#endif