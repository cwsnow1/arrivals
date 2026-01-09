
set(COMPONENT_SRCDIRS
    .
    callbacks/esp32
    fonts
)

set(COMPONENT_ADD_INCLUDEDIRS
    .
)

set(COMPONENT_PRIV_INCLUDEDIRS
    callbacks
)

set(COMPONENT_REQUIRES
    lvgl
)

set(COMPONENT_PRIV_REQUIRES
    wifi
)

register_component()
