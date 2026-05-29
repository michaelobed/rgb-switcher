//
//  AppEvent.hpp
//  remote_board
//
//  Created by michaelobed on 01/04/2026.
//  
//  Copyright © 2026 Michael Obed.

#ifndef AppEvent_hpp
#define AppEvent_hpp

#include "esp_event.h"

ESP_EVENT_DECLARE_BASE(EVENT_APPEVENT);

enum AppEvent
{
    /* Add events to this as needed. */
};

#endif