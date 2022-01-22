
set(_EMBED_FILES
        "web_camera/www/index_ov2640.html.gz"
        "web_camera/www/index_ov3660.html.gz"
        "web_camera/www/index_ov5640.html.gz"
        "web_camera/www/monitor.html.gz")

idf_component_register(SRC_DIRS "." "web_camera"
                    INCLUDE_DIRS "." "web_camera/include"
                    PRIV_REQUIRES console spi_flash nvs_flash fatfs esp32-camera mdns esp_http_server
                    EMBED_TXTFILES ${_EMBED_FILES} )
