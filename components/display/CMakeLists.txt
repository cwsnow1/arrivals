
set(COMPONENT_SRCDIRS
    "."
)

set(COMPONENT_ADD_INCLUDEDIRS
    "."
)

set(COMPONENT_REQUIRES
    esp_driver_i2c
)

set(COMPONENT_PRIV_REQUIRES
    esp_lcd
    esp_driver_gpio
    esp_driver_ledc
    esp_driver_spi
    lvgl
    esp_timer
)

register_component()
