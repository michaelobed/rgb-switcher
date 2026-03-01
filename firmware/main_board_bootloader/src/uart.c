//
//  uart.c
//  main_board_bootloader
//
//  Created by michaelobed on 01/03/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "../../common/io.h"
#include "../../common/sys.h"
#include "../../common/uart.h"

static uint8_t uartRxBuf[UART_BUFSIZE] = {};

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