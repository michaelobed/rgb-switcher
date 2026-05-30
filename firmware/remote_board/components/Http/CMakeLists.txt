idf_component_register(SRCS "Http.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES "esp_http_server"
                    EMBED_TXTFILES html/remote.html html/styles.css)
