//
//  Uart.cpp
//  remote_board
//
//  Created by michaelobed on 31/03/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "esp_log.h"
#include "Uart.hpp"

static void eventTask(void* arg);

void Uart::Handle()
{
    uint8_t data[bufferSize];
    size_t length = 0;

    /* Get the data from the FIFO. */
    uart_get_buffered_data_len(UartNum, &length);
    length = uart_read_bytes(UartNum, data, length, rxTicksToWait);
    ESP_LOGI(__func__, "Got %d bytes.", length);

    /* TODO: Handling. :D */
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
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
        .flags = {false, false}
    };
    esp_err_t err = ESP_OK;
    
    err = uart_driver_install(UartNum, bufferSize, bufferSize, queueSize, &Queue, 0);
    if(err != ESP_OK)
    {
        ESP_LOGE(__func__, "Driver install failed!");
        return false;
    }

    err = uart_param_config(UartNum, &config);
    if(err != ESP_OK)
    {
        ESP_LOGE(__func__, "Configuration failed!");
        return false;
    }

    /* These UART pins are according to the ESP32-WROOM-32 datasheet. Change in the .hpp file if needed. */
    err = uart_set_pin(UartNum, 18, 17, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if(err != ESP_OK)
    {
        ESP_LOGE(__func__, "Could not set pins!");
        return false;
    }

    /* Configure pattern recognition to respond to "\n" command ending. */
    err = uart_enable_pattern_det_baud_intr(UartNum, '\n', 1, 1, 0, 0);
    if(err != ESP_OK)
    {
        ESP_LOGE(__func__, "Could not set up pattern detection!");
        return false;
    }
    
    /* Fire off the event task. */
    xTaskCreate(eventTask, "uart_eventTask", taskStackDepth, nullptr, taskPriority, nullptr);

    return true;
}

void Uart::SwitchToInput(char inputChar)
{
    const char txBuffer[3] = {'s', inputChar, '\n'};
    uart_tx_chars(UartNum, txBuffer, 3);
}

void eventTask(void* arg)
{
    uart_event_t event;
    Uart& uart = Uart::GetInstance();

    while(true)
    {
        /* Get the event if there is one. */
        if(xQueueReceive(uart.Queue, (void*)&event, portMAX_DELAY))
        {
            switch(event.type)
            {
                /* We aren't interested. Flush everything. */
                case UART_FIFO_OVF:
                    uart_flush_input(uart.UartNum);
                    xQueueReset(uart.Queue);
                    break;
        
                /* Ah, something to do! */
                case UART_PATTERN_DET:
                    uart.Handle();
                    break;

                default:
                    break;
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}