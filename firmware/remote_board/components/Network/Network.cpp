//
//  Network.cpp
//  remote_board
//
//  Created by michaelobed on 29/05/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "Config.hpp"
#include <cstring>
#include "esp_log.h"
#include "mdns.h"
#include "Network.hpp"

static Config& config = Config::GetInstance();

Network::Network()
{
    netIfInstance = nullptr;
}

esp_err_t Network::InitAP()
{
    constexpr char defaultSsid[] = "rgb-switcher";
    esp_err_t err = ESP_OK;

    wifi_init_config_t initConfig = WIFI_INIT_CONFIG_DEFAULT();

    err = preInit();
    if(err != ESP_OK)
    {
        ESP_LOGE(__func__, "WiFi net-if initialisation failed (%d)!", err);
        return err;
    }

    /* Initialise an AP netif instance. */
    netIfInstance = esp_netif_create_default_wifi_ap();
    if(netIfInstance == nullptr)
    {
        err = ESP_FAIL;
        ESP_LOGE(__func__, "WiFi AP creation failed!");
        return err;
    }

    /* Initialise WiFi and copy some default settings across to its config. */
    strncpy((char*)wifiConfig.ap.ssid, defaultSsid, MAX_SSID_LEN);
    strncpy((char*)wifiConfig.ap.password, defaultSsid, MAX_PASSPHRASE_LEN);
    wifiConfig.ap.channel = CONFIG_NETWORK_CHANNEL;
    wifiConfig.ap.ssid_len = 0;
    wifiConfig.ap.max_connection = maxConnections;
    wifiConfig.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    err = esp_wifi_init(&initConfig);
    if(err == ESP_OK)
        err = postInit();

    return err;
}

esp_err_t Network::postInit()
{
    esp_err_t err = ESP_OK;

    /* Set up WiFi according to the desired mode and start it. */
    err = esp_wifi_set_mode(config.NetworkIsSTA ? WIFI_MODE_STA : WIFI_MODE_AP);
    if(err != ESP_OK)
        return err;

    err = esp_wifi_set_config(config.NetworkIsSTA ? WIFI_IF_STA : WIFI_IF_AP, &wifiConfig);
    if(err != ESP_OK)
        return err;

    err = esp_wifi_start();
    if(err != ESP_OK)
        return err;

    /* Start mDNS so we're not memorising the IP address just to log into this! */
    err = mdns_init();
    if(err != ESP_OK)
        return err;
    
    mdns_hostname_set("rgb-switcher");
    mdns_service_add(nullptr, "_http", "_tcp", 80, nullptr, 0);
    mdns_service_instance_name_set("_http", "_tcp", "RGB Switcher");
    return ESP_OK;
}

esp_err_t Network::preInit()
{
    return esp_netif_init();
}