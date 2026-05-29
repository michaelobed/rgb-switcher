//
//  Http.cpp
//  remote_board
//
//  Created by michaelobed on 29/05/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "Http.hpp"

static esp_err_t onUri(httpd_req_t* request);

Http::Http()
{
    handle = nullptr;
    uriIndex.handler = onUri;
}

esp_err_t Http::Init()
{
    httpd_config_t httpConfig = HTTPD_DEFAULT_CONFIG();

    /* Purge least recently used connection by default. */
    httpConfig.lru_purge_enable = true;

    /* Start httpd and register URIs. */
    return (httpd_start(&handle, &httpConfig) | httpd_register_uri_handler(handle, &uriIndex));
}

esp_err_t onUri(httpd_req_t* request)
{
    esp_err_t err = ESP_OK;
    const char response[] = "Hello, world! :)";

    err = httpd_resp_send(request, response, HTTPD_RESP_USE_STRLEN);

    return err;
}