idf_component_register(SRCS "Http.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES "Config" "esp_http_server"
                    EMBED_TXTFILES html/config.html html/remote.html html/styles.css)
