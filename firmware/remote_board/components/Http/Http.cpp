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
#include "Uart.hpp"

extern const char htmlConfig[] asm("_binary_config_html_start");
extern const char htmlRemote[] asm("_binary_remote_html_start");
extern const char htmlStyles[] asm("_binary_styles_css_start");

static Config& config = Config::GetInstance();
static Uart& uart = Uart::GetInstance();

static esp_err_t onUriGet(httpd_req_t* request);
static esp_err_t onUriPost(httpd_req_t* request);

Http::Http()
{
    handle = nullptr;
    memset(replacementBuffer, 0, replacementBufferSize);
    uriIndexConfig.handler = onUriGet;
    uriIndexConfigSubmit.handler = onUriPost;
    uriIndexInput.handler = onUriGet;
    uriIndexRemote.handler = onUriGet;
}

char* Http::DoReplacement(char* html, const char* toLookFor, const char* toReplaceItWith, bool htmlIsStatic)
{
    char* tag = nullptr;
    int tagLocation = 0;
    int toLookForSize = strlen(toLookFor);
    int toReplaceItWithSize = strlen(toReplaceItWith);
    
    /* Look for the tag. */
    tag = strstr(html, toLookFor);
    if(tag != nullptr)
    {
        /* Copy everything until just before the tag, then replace it with the contents of toReplaceItWith. */
        tagLocation = tag - html;
        strncpy(replacementBuffer, html, tagLocation);
        strcpy(replacementBuffer + tagLocation, toReplaceItWith);
        strcpy(replacementBuffer + tagLocation + toReplaceItWithSize, html + tagLocation + toLookForSize);
    }

    return replacementBuffer;
}

esp_err_t Http::Init()
{
    httpd_config_t httpConfig = HTTPD_DEFAULT_CONFIG();

    /* Purge least recently used connection by default. */
    httpConfig.lru_purge_enable = true;

    /* Start httpd and register URIs. */
    return (    httpd_start(&handle, &httpConfig) |
                httpd_register_uri_handler(handle, &uriIndexConfig) |
                httpd_register_uri_handler(handle, &uriIndexConfigSubmit) |
                httpd_register_uri_handler(handle, &uriIndexInput) |
                httpd_register_uri_handler(handle, &uriIndexRemote));
}

void Http::OnOops(httpd_req_t* request)
{
    httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "Oops.");
}

esp_err_t onUriGet(httpd_req_t* request)
{
    Http& http = Http::GetInstance();
    char* inputFromUri = nullptr;
    constexpr int inputTagMaxLen = 18;
    char inputTag[inputTagMaxLen] = "[[INPUTNAMEx]]";
    static constexpr int queryBufferSize = 64;
    char queryBuffer[queryBufferSize];
    constexpr char stylesTag[] = "[[STYLES]]";
    char* toServe = nullptr;

    /* What page are we serving? */
    if(strstr(request->uri, "config") != nullptr)
        toServe = (char*)htmlConfig;
    else toServe = (char*)htmlRemote;

    /* Are we doing anything UART-wise? If so, check that the input number is valid before executing. */
    inputFromUri = strstr(request->uri, "input");
    if(inputFromUri != nullptr)
    {
        httpd_req_get_url_query_str(request, queryBuffer, queryBufferSize);
        if((queryBuffer[0] >= '0') && (queryBuffer[0] <= '7'))
            uart.SwitchToInput(queryBuffer[0]);
    }

    /* Import styles.css. */
    char* newHtml = http.DoReplacement(toServe, stylesTag, htmlStyles, true);

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

esp_err_t onUriPost(httpd_req_t* request)
{
    httpd_req_t redirect = 
    {
        .method = HTTP_GET,
        .uri = "/"
    };

    /* TODO. For now, just redirect. */
    onUriGet(&redirect);
    return ESP_OK;
}