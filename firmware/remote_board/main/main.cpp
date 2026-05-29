//
//  main.cpp
//  remote_board
//
//  Created by michaelobed on 31/03/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "AppEvent.hpp"
#include <cstdio>
#include "esp_event.h"
#include "esp_log.h"
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Uart.hpp"

static Uart& uart = Uart::GetInstance();

static void errorHandler();
static void onEvent(void* arg, esp_event_base_t base, int32_t id, void* data);

ESP_EVENT_DEFINE_BASE(EVENT_APPEVENT);

extern "C" void app_main()
{
    int err = ESP_OK;

    /* Initialise a default event loop. */
    err = esp_event_loop_create_default();
    if(err == ESP_OK)
    {
        /* Attach the app event handler. */
        err = esp_event_handler_instance_register(  EVENT_APPEVENT,
                                                    ESP_EVENT_ANY_ID,
                                                    onEvent,
                                                    nullptr,
                                                    nullptr);
        if(err != ESP_OK)
        {
            /* We couldn't initialise the app event handler. This is bad. Panic! */
            ESP_LOGE(__func__, "App event handler registration failed (%d)!", err);
            errorHandler();
        }
    }
    else
    {
        /* We couldn't initialise the default event loop. This is even worse. Freak out! */
        ESP_LOGE(__func__, "Event loop init failed (%d)!", err);
        errorHandler();
    }

    /* Initialise peripherals. */
    if(!uart.Init())
        errorHandler();
    
    ESP_LOGI(__func__, "Peripheral init done! Running main loop...");
    
    while(true)
    {
        /* Delay so main loop doesn't choke for watchdog reasons. */
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void errorHandler()
{
    /* Do nothing forever. */
    while(true);
}

void onEvent(void* arg, esp_event_base_t base, int32_t id, void* data)
{

}