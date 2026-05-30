//
//  Http.hpp
//  remote_board
//
//  Created by michaelobed on 29/05/2026.
//  
//  Copyright © 2026 Michael Obed.

#ifndef Http_hpp
#define Http_hpp

#include "esp_http_server.h"

class Http
{
    public:
        Http();

        static Http& GetInstance()
        {
            static Http h;
            return h;
        }

        static constexpr int BufferSize = 8192;
        char Buffer[BufferSize];

        char* DoReplacement(char* html, const char* toLookFor, const char* toReplaceItWith, bool htmlIsStatic = false);
        esp_err_t Init();
        void OnOops(httpd_req_t* request);

    private:
        httpd_handle_t handle;
        
        /* URIs. */
        httpd_uri_t uriIndexConfig =
        {
            .uri = "/config",
            .method = HTTP_GET,
            .handler = nullptr,
            .user_ctx = nullptr
        };
        
        httpd_uri_t uriIndexConfigSubmit =
        {
            .uri = "/config",
            .method = HTTP_POST,
            .handler = nullptr,
            .user_ctx = nullptr
        };

        httpd_uri_t uriIndexInput =
        {
            .uri = "/input",
            .method = HTTP_GET,
            .handler = nullptr,
            .user_ctx = nullptr
        };

        httpd_uri_t uriIndexRemote =
        {
            .uri = "/",
            .method = HTTP_GET,
            .handler = nullptr,
            .user_ctx = nullptr
        };
};

#endif