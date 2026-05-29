//
//  Network.hpp
//  remote_board
//
//  Created by michaelobed on 29/05/2026.
//  
//  Copyright © 2026 Michael Obed.

#ifndef Network_hpp
#define Network_hpp

#include "esp_wifi.h"

class Network
{
    public:
        Network();

        static Network& GetInstance()
        {
            static Network n;
            return n;
        }

        esp_err_t InitAP();

    private:
        esp_netif_t* netIfInstance;
        wifi_config_t wifiConfig;

        esp_err_t postInit();
        esp_err_t preInit();
};

#endif