idf_component_register(
    SRCS "arrivals.c"
    INCLUDE_DIRS "."
    REQUIRES wifi
             api
             nvs_flash
             cta
             display
             lvgl
             ui
             esp_netif
             config
             esp_driver_gpio
)
