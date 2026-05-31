//
//  Config.cpp
//  remote_board
//
//  Created by michaelobed on 29/05/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "Config.hpp"
#include <cstring>

Config::Config()
{
    /* Populate with initial values. */
    for(int i = 0; i < 8; i++)
        snprintf(InputName[i], InputNameMaxLen, "Input %d", i);
    NetworkIsSTA = false;
    strncpy(NetworkSsid, "YourNetworkHere", MAX_SSID_LEN);
    memset(NetworkPsk, 0, MAX_PASSPHRASE_LEN);
}

void Config::EraseAll()
{
    nvs_erase_all(handle);
    nvs_commit(handle);
}

esp_err_t Config::InitStorage()
{
    esp_err_t err = ESP_OK;
    
    /* Init the NVS flash driver. */
    err = nvs_flash_init();
    if(err != ESP_OK)
        return err;

    /* Now get a handle to the NVS. */
    err = nvs_open("KeyStorage", NVS_READWRITE, &handle);

#if defined(CONFIG_CONFIG_ERASEALLONBOOT)
    EraseAll();
#endif

    return err;
}

bool Config::Load()
{
    size_t size = 0;
    char tag[tagBufferSize];
    union
    {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
    } temp;
    
    /* Get the existence number. Do we exist? */
    nvs_get_u32(handle, "existence", &temp.u32);
    if(temp.u32 != existenceNum)
        return false;

    /* If we do, grab all the config data. */
    for(int i = 0; i < 8; i++)
    {
        size = InputNameMaxLen;
        sprintf(tag, "inputName%d", i);
        nvs_get_str(handle, tag, InputName[i], &size);
    }
    nvs_get_u8(handle, "networkIsSTA", &temp.u8);
    NetworkIsSTA = (temp.u8 > 0);
    size = MAX_SSID_LEN;
    nvs_get_str(handle, "networkSsid", NetworkSsid, &size);
    size = MAX_PASSPHRASE_LEN;
    nvs_get_str(handle, "networkPsk", NetworkPsk, &size);

    return true;
} 

void Config::Save()
{
    char tag[tagBufferSize];

    nvs_set_u32(handle, "existence", existenceNum);
    for(int i = 0; i < 8; i++)
    {
        sprintf(tag, "inputName%d", i);
        nvs_set_str(handle, tag, InputName[i]);
    }
    nvs_set_u8(handle, "networkIsSTA", NetworkIsSTA ? 0x01 : 0x00);
    nvs_set_str(handle, "networkSsid", NetworkSsid);
    nvs_set_str(handle, "networkPsk", NetworkPsk);

    nvs_commit(handle);
}