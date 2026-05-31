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
    memset(Buffer, 0, BufferSize);
    uriIndexConfig.handler = onUriGet;
    uriIndexConfigSubmit.handler = onUriPost;
    uriIndexInput.handler = onUriGet;
    uriIndexRemote.handler = onUriGet;
}

char* Http::doReplacement(char* html, const char* toLookFor, const char* toReplaceItWith, bool htmlIsStatic)
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
        strncpy(Buffer, html, tagLocation);
        strcpy(Buffer + tagLocation, toReplaceItWith);
        strcpy(Buffer + tagLocation + toReplaceItWithSize, html + tagLocation + toLookForSize);
    }

    return Buffer;
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

void Http::onOops(httpd_req_t* request)
{
    httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "Oops.");
}

esp_err_t Http::SendPage(httpd_req_t* request, char* page)
{
    constexpr int inputTagMaxLen = 18;
    char inputTag[inputTagMaxLen] = "[[INPUTNAMEx]]";
    constexpr char stylesTag[] = "[[STYLES]]";

    /* Import styles.css. */
    char* newHtml = doReplacement(page, stylesTag, htmlStyles, true);

    if(newHtml == nullptr)
        onOops(request);
    else
    {
        /* Replace input names. */
        for(int i = 0; i < 8; i++)
        {
            snprintf(inputTag, inputTagMaxLen, "[[INPUTNAME%d]]", i);
            newHtml = doReplacement(newHtml, inputTag, config.InputName[i]);
        }
        httpd_resp_send(request, newHtml, HTTPD_RESP_USE_STRLEN);
    }

    return ESP_OK;
}

esp_err_t onUriGet(httpd_req_t* request)
{
    Http& http = Http::GetInstance();
    char* toServe = nullptr;

    /* What page are we serving? */
    if(strstr(request->uri, "config") != nullptr)
        toServe = (char*)htmlConfig;
    else toServe = (char*)htmlRemote;

    /* Are we doing anything UART-wise? If so, check that the input number is valid before executing. */
    if(strstr(request->uri, "input") != nullptr)
    {
        httpd_req_get_url_query_str(request, http.Buffer, http.BufferSize);
        if((http.Buffer[0] >= '0') && (http.Buffer[0] <= '7'))
            uart.SwitchToInput(http.Buffer[0]);
    }

    return http.SendPage(request, toServe);
}

esp_err_t onUriPost(httpd_req_t* request)
{
    char* data = nullptr;
    int dataLength = 0;
    int err = 0;
    Http& http = Http::GetInstance();
    constexpr char zero = '\0';

    /* Get the POST data. */
    memset(http.Buffer, 0, http.BufferSize);
    err = httpd_req_recv(request, http.Buffer, request->content_len);

    /* If the socket was closed, kill the "connection" here (yeah yeah I know HTTP is supposed to be connectionless...). */
    if(err == 0)
        return ESP_FAIL;

    /* Handle timeout.*/
    else if(err == HTTPD_SOCK_ERR_TIMEOUT)
        httpd_resp_send_408(request);
    else
    {
        /* Replace '+' with ' '. */
        data = strchr(http.Buffer, '+');
        while(data != nullptr)
        {
            *data = ' ';
            data = strchr(data + 1, '+');
        }

        /* A bit cheeky, but replace '&' with '\0'. */
        data = strchr(http.Buffer, '&');
        while(data != nullptr)
        {
            *data = zero;
            data = strchr(data + 1, '&');
        }

        /* Parse input names, tokenising by '\0'. */
        data = strchr(http.Buffer, '=') + 1;
        for(int i = 0; i < 8; i++)
        {
            dataLength = strlen(data);
            strncpy(config.InputName[i], data, dataLength + 1);
            data += dataLength;
            data = strchr(data + 1, '=');
            if(data != nullptr)
                data += 1;
        }

        config.Save();

        /* Finally, redirect to remote.html. */
        httpd_resp_set_status(request, "303 See Other");
        httpd_resp_set_hdr(request, "Location", "/");
        httpd_resp_send(request, nullptr, 0);
    }

    return ESP_OK;
}