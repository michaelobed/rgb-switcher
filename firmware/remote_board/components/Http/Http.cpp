//
//  Http.cpp
//  remote_board
//
//  Created by michaelobed on 29/05/2026.
//  
//  Copyright © 2026 Michael Obed.

#include "Config.hpp"
#include "esp_log.h"
#include "Http.hpp"

extern const char remote[] asm("_binary_remote_html_start");
extern const char styles[] asm("_binary_styles_css_start");

static Config& config = Config::GetInstance();

static esp_err_t onUriRemote(httpd_req_t* request);

Http::Http()
{
    handle = nullptr;
    replacementBufferFlipFlop = false;
    replacementBufferUsed = false;
    uriIndexRemote.handler = onUriRemote;

    /* Zero out the "replacement" buffers. These are double buffers so that I can stop using dynamic allocation
     * and crashing the ESP32. */
    memset(replacementBuffer[0], 0, replacementBufferSize);
    memset(replacementBuffer[1], 0, replacementBufferSize);
}

char* Http::DoReplacement(char* html, const char* toLookFor, const char* toReplaceItWith, bool htmlIsStatic)
{
    int htmlSize = strlen(html);
    char* newHtml = nullptr;
    int newHtmlSize = 0;
    char* tag = nullptr;
    int tagLocation = 0;
    int toLookForSize = strlen(toLookFor);
    int toReplaceItWithSize = strlen(toReplaceItWith);
    
    /* Look for the tag. */
    tag = strstr(html, toLookFor);
    if(tag != nullptr)
    {
        /* We need a buffer for the replacement. 
         * Copy everything until just before the tag, then replace it with the contents of toReplaceItWith. */
        tagLocation = tag - html;
        ESP_LOGI(__func__, "toLookFor = %s, toLookForSize = %d, htmlSize = %d", toLookFor, toLookForSize, htmlSize);
        newHtml = replacementBufferFlipFlop ? replacementBuffer[1] : replacementBuffer[0];
        memcpy(newHtml, html, tagLocation);
        memcpy(newHtml + tagLocation, toReplaceItWith, toReplaceItWithSize);
        memcpy(newHtml + tagLocation + toReplaceItWithSize, html + tagLocation + toLookForSize, htmlSize - (tagLocation + toLookForSize));
        newHtmlSize = tagLocation + toReplaceItWithSize + (htmlSize - (tagLocation + toLookForSize));
        newHtml[newHtmlSize - 1] = '\0';
        replacementBufferUsed = true;
        replacementBufferFlipFlop ^= 0x01;
    }

    return newHtml;
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

void Http::OnOops(httpd_req_t* request)
{
    httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "Oops.");
}

esp_err_t onUriRemote(httpd_req_t* request)
{
    Http& http = Http::GetInstance();
    constexpr int inputTagMaxLen = 18;
    char inputTag[inputTagMaxLen] = "[[INPUTNAMEx]]";
    constexpr char stylesTag[] = "[[STYLES]]";

    /* Import styles.css. */
    char* newHtml = http.DoReplacement((char*)remote, stylesTag, styles, true);

    if(newHtml == nullptr)
        http.OnOops(request);
    else
    {
        /* Replace input names. */
        for(int i = 0; i < 8; i++)
        {
            snprintf(inputTag, inputTagMaxLen, "[[INPUTNAME%d]]", i);
            newHtml = http.DoReplacement(newHtml, inputTag, config.InputName[i]);
        }
        httpd_resp_send(request, newHtml, HTTPD_RESP_USE_STRLEN);
    }

    return ESP_OK;
}