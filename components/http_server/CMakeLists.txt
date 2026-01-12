
set(COMPONENT_SRCDIRS
    .
    webpage
)

set(COMPONENT_ADD_INCLUDEDIRS
    .
)

set(COMPONENT_PRIV_REQUIRES
    esp_http_server
    esp_timer
    wifi
    config
    esp_wifi
    cta
)

set(COMPONENT_REQUIRES
)

register_component()
