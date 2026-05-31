//
//  main.cpp
//  remote_board
//
//  Created by michaelobed on 31/03/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "Config.hpp"
#include <cstdio>
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "Http.hpp"
#include <inttypes.h>
#include "Network.hpp"
#include "sdkconfig.h"
#include "freertos/task.h"
#include "Uart.hpp"

static Config& config = Config::GetInstance();
static Http& http = Http::GetInstance();
static Network& network = Network::GetInstance();
static Uart& uart = Uart::GetInstance();

static void errorHandler();
static void onEvent(void* arg, esp_event_base_t base, int32_t id, void* data);

ESP_EVENT_DEFINE_BASE(EVENT_APPEVENT);

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

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
        ESP_LOGE(__func__, "Could not init event loop (%d)!", err);
        errorHandler();
    }

    /* Initialise peripherals. */
    if(!uart.Init())
    {
        ESP_LOGE(__func__, "Could not init UART!");
        errorHandler();
    }
    
    /* Look for a config in storage. If it doesn't exist, save the initialised one. */
    err = config.InitStorage();
    if(err != ESP_OK)
    {
        ESP_LOGE(__func__, "Could not init config storage (%d)!", err);
        errorHandler();
    }

    if(!config.Load())
    {
        ESP_LOGW(__func__, "Config did not exist. Saving afresh...");
        config.Save();
    }
    else ESP_LOGI(__func__, "Config loaded successfully.");

    /* We have all the information we need. Start the WiFi! */
    if(config.NetworkIsSTA)
    {
        /* TODO: WiFi STA config. For now, panic. */
        ESP_LOGE(__func__, "WiFi STA not yet supported!");
        errorHandler();
    }
    else
    {
        err = network.InitAP();
        if(err != ESP_OK)
        {
            ESP_LOGE(__func__, "Could not start WiFi access point (%d)!", err);
            errorHandler();
        }
        else ESP_LOGI(__func__, "WiFi access point started.");
    }

    /* Get HTTP server going. */
    err = http.Init();
    if(err != ESP_OK)
    {
        ESP_LOGE(__func__, "Could not start HTTP server (%d)!", err);
        errorHandler();
    }
    else ESP_LOGI(__func__, "HTTP server started.");

    ESP_LOGI(__func__, "Init done! Running main loop...");
    
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