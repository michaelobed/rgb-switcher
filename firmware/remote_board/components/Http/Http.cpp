//
//  Http.cpp
//  remote_board
//
//  Created by michaelobed on 29/05/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "Http.hpp"

extern const uint8_t html_remote[] asm("_binary_remote_html_start");

static esp_err_t onUriRemote(httpd_req_t* request);

Http::Http()
{
    handle = nullptr;
    uriIndexRemote.handler = onUriRemote;
}

esp_err_t Http::Init()
{
    httpd_config_t httpConfig = HTTPD_DEFAULT_CONFIG();

    /* Purge least recently used connection by default. */
    httpConfig.lru_purge_enable = true;

    /* Start httpd and register URIs. */
    return (    httpd_start(&handle, &httpConfig) |
                httpd_register_uri_handler(handle, &uriIndexRemote));
}

esp_err_t onUriRemote(httpd_req_t* request)
{
    esp_err_t err = ESP_OK;

    err = httpd_resp_send(request, (char*)html_remote, HTTPD_RESP_USE_STRLEN);

    return err;
}