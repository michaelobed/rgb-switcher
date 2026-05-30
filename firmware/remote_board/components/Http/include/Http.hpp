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

        esp_err_t Init();

    private:
        httpd_handle_t handle;
        httpd_uri_t uriIndexRemote =
        {
            .uri = "/",
            .method = HTTP_GET,
            .handler = nullptr,
            .user_ctx = nullptr
        };
};

#endif