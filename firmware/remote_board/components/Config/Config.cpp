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
    strncpy(networkSsid, "YourNetworkHere", MAX_SSID_LEN);
    memset(networkPsk, 0, MAX_PASSPHRASE_LEN);
}

void Config::EraseAll()
{
    nvs_erase_all(handle);
    nvs_commit(handle);
}

bool Config::InitStorage()
{
    int ret = ESP_OK;
    
    /* Init the NVS flash driver. */
    ret = nvs_flash_init();
    if(ret != ESP_OK)
        return false;

    /* Now get a handle to the NVS. */
    ret = nvs_open("KeyStorage", NVS_READWRITE, &handle);
    return (ret == ESP_OK);

#if defined(CONFIG_ERASEALLONBOOT)
    EraseAll();
#endif
}

bool Config::Load()
{
    uint32_t existence = 0xffffffff;
    size_t size = 0;
    
    /* Get the existence number. Do we exist? */
    nvs_get_u32(handle, "existence", &existence);
    if(existence != existenceNum)
        return false;

    /* If we do, grab all the config data. */
    size = MAX_SSID_LEN;
    nvs_get_str(handle, "networkSsid", networkSsid, &size);
    size = MAX_PASSPHRASE_LEN;
    nvs_get_str(handle, "networkPsk", networkPsk, &size);

    return true;
} 

void Config::Save()
{
    nvs_set_u32(handle, "existence", existenceNum);
    nvs_set_str(handle, "networkSsid", networkSsid);
    nvs_set_str(handle, "networkPsk", networkPsk);

    nvs_commit(handle);
}