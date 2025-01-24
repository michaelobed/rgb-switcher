//
//  uart.c
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
#include "../../common/sys.h"
#include "../../common/uart.h"

#define UART_BUFSIZE                    1024

static uint8_t uartRxBuf[UART_BUFSIZE];
static uint8_t uartRxBufCtr = 0;
static bool uartDataAvaliable = FALSE;

void UartHandler(void)
{
    ctrlParams params;

    if(uartDataAvaliable)
    {
        memcpy(&params.bytes, &uartRxBuf[1], uartRxBufCtr - 1);
        CtrlHandleCmd(CtrlGetAsciiAsCmd(uartRxBuf[0]), &params);
        uartDataAvaliable = FALSE;
        uartRxBufCtr = 0;
    }
}

void UartHexDump(uint8_t* data, uint8_t size)
{
    UartPrint("\nData size: %u bytes:\n", size);

    for(uint8_t i = 0; i < size; i++)
    {
        if((i > 0) && ((i & 15) == 0))
            UartPrint("\n");
        UartPrint("%02x ", data[i]);
    }
    UartPrint("\n");
}

void UartInit(void)
{
    cli();
    memset(uartRxBuf, 0, UART_BUFSIZE);
    
    /* UART I/O. */
    DDRD &= ~_BV(PIN_UARTRX);
    DDRD |= _BV(PIN_UARTTX);
    
    /* Initialise UART for 8N1, 38.4 kbaud. */
    UBRR0 = 12;
    UCSR0B |= (_BV(RXCIE0) | _BV(TXEN0) | _BV(RXEN0));
    sei();
}

void UartPrint(const char* s, ...)
{
    char buf[UART_BUFSIZE];

    /* Fetch args. */
    va_list args;
    va_start(args, s);
    
    /* Construct string. */
    vsnprintf(buf, UART_BUFSIZE, s, args);
    va_end(args);

    size_t len = strlen(buf);
    UartSendBytes((uint8_t*)buf, len);
}

void UartSendBytes(uint8_t* bytes, uint8_t size)
{
    for(uint8_t i = 0; i < size; i++)
    {
        UDR0 = bytes[i];
        while(!(UCSR0A & _BV(TXC0)));
        UCSR0A |= _BV(TXC0);
    }
}

ISR(USART0_RX_vect)
{
    if(uartRxBufCtr < UART_BUFSIZE)
    {
        uartRxBuf[uartRxBufCtr] = UDR0;
        if(uartRxBuf[uartRxBufCtr] == '\n')
            uartDataAvaliable = TRUE;
        uartRxBufCtr++;
    }
}