//
//  Config.hpp
//  remote_board
//
//  Created by michaelobed on 29/05/2026.
//  
//  Copyright © 2026 Michael Obed.

#ifndef Config_hpp
#define Config_hpp

#include "esp_wifi_types.h"
#include "nvs_flash.h"

class Config
{
    public:
        Config();

        static Config& GetInstance()
        {
            static Config c;
            return c;
        }

        static constexpr size_t InputNameMaxLen = 32;
        char InputName[InputNameMaxLen][8];
        bool NetworkIsSTA;
        char NetworkSsid[MAX_SSID_LEN];
        char NetworkPsk[MAX_PASSPHRASE_LEN];

        void EraseAll();
        esp_err_t InitStorage();
        bool Load();
        void Save();

    private:
        static constexpr uint32_t existenceNum = 0xbf9d45c5;
        nvs_handle_t handle;
        int tagBufferSize = 16;
};

#endif