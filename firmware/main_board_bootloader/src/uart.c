//
//  uart.c
//  main_board_bootloader
//
//  Created by michaelobed on 01/03/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "../../common/ctrl.h"
#include "../../common/io.h"
#include "../../common/sys.h"
#include "../../common/uart.h"

static uint8_t uartRxBuf[UART_BUFSIZE] = {};
static uint8_t uartRxBufCtr = 0;
static bool uartDataAvaliable = FALSE;

void UartDeInit(void)
{
    cli();
    UCSR0B &= ~(_BV(RXCIE0) | _BV(TXEN0) | _BV(RXEN0));
    sei();
}

uint16_t UartGetBytesAvailable(void)
{
    return uartRxBufCtr;
}

void UartHandler(void)
{
    ctrlParams params;

    if(uartDataAvaliable)
    {
        memcpy(&params.bytes, &uartRxBuf[1], uartRxBufCtr - 1);
        CtrlHandleCmd(uartRxBuf[0], &params);
        uartDataAvaliable = FALSE;
        uartRxBufCtr = 0;
    }
}

void UartHexDump(uint8_t* data, uint16_t size)
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