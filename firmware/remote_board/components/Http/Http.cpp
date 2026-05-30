//
//  Http.cpp
//  remote_board
//
//  Created by michaelobed on 29/05/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "esp_log.h"
#include "Http.hpp"

extern const char remote[] asm("_binary_remote_html_start");
extern const char styles[] asm("_binary_styles_css_start");

static const int remoteSize = strlen(remote);
static const int stylesSize = strlen(styles);

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

char* Http::ImportStyles()
{
    char* newHtml = nullptr;
    int newHtmlSize = 0;
    int stylesLoc = 0;
    char* stylesTag = nullptr;
    constexpr char stylesTagString[] = "[[STYLES]]";
    static const int stylesTagStringSize = strlen(stylesTagString);
    
    /* Look for the [[STYLES]] tag. */
    stylesTag = strstr(remote, stylesTagString);
    if(stylesTag != nullptr)
    {
        /* We need a buffer for the styles.css import. Copy everything until just before the [[STYLES]] tag,
         * then replace it with the styles text. */
        stylesLoc = stylesTag - remote;
        newHtml = new char[stylesSize + remoteSize];
        memcpy(newHtml, remote, stylesLoc);
        memcpy(newHtml + stylesLoc, styles, stylesSize);
        memcpy(newHtml + stylesLoc + stylesSize, remote + stylesLoc + stylesTagStringSize, remoteSize - (stylesLoc + stylesTagStringSize));
        newHtmlSize = stylesLoc + stylesSize + (remoteSize - (stylesLoc + stylesTagStringSize));
        newHtml[newHtmlSize - 1] = '\0';
    }

    return newHtml;
}

void Http::OnOops(httpd_req_t* request)
{
    httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "Oops.");
}

esp_err_t onUriRemote(httpd_req_t* request)
{
    Http& http = Http::GetInstance();
    char* newHtml = http.ImportStyles();

    if(newHtml == nullptr)
        http.OnOops(request);
    else httpd_resp_send(request, newHtml, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}