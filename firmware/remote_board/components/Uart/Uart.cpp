//
//  Uart.cpp
//  remote_board
//
//  Created by michaelobed on 31/03/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "esp_log.h"
#include "Uart.hpp"

void Uart::handle()
{
    uint8_t data[bufferSize];
    size_t length = 0;

    /* Get the data from the FIFO. */
    uart_get_buffered_data_len(uartNum, &length);
    length = uart_read_bytes(uartNum, data, length, rxTicksToWait);

    /* TODO: Handling. :D */
    ESP_LOGI(__func__, "Got %d bytes.", length);
}

bool Uart::Init()
{
    uart_config_t config =
    {
        .baud_rate = 38400,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0
    };
    esp_err_t err = ESP_OK;
    
    err = uart_driver_install(uartNum, bufferSize, bufferSize, queueSize, &queue, 0);
    if(err != ESP_OK)
    {
        ESP_LOGE(__func__, "Driver install failed!");
        return false;
    }

    err = uart_param_config(uartNum, &config);
    if(err != ESP_OK)
    {
        ESP_LOGE(__func__, "Configuration failed!");
        return false;
    }

    /* These UART pins are according to the ESP32-WROOM-32 datasheet. Change in the .hpp file if needed. */
    err = uart_set_pin(uartNum, 18, 17, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if(err != ESP_OK)
    {
        ESP_LOGE(__func__, "Could not set pins!");
        return false;
    }

    /* Configure pattern recognition to respond to "\n" command ending. */
    err = uart_enable_pattern_det_baud_intr(uartNum, '\n', 1, 5, 0, 0);
    if(err != ESP_OK)
    {
        ESP_LOGE(__func__, "Could not set up pattern detection!");
        return false;
    }

    return true;
}

void Uart::onEvent(void* arg, esp_event_base_t base, int32_t id, void* data)
{
    switch(id)
    {
        /* We aren't interested. Flush everything. */
        case UART_FIFO_OVF:
            uart_flush_input(uartNum);
            xQueueReset(queue);
            break;

        /* Ah, something to do! */
        case UART_PATTERN_DET:
            handle();
            break;
    }
}