
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

idf_build_get_property(python EXECUTABLE_PYTHON)
set(SCRIPT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/webpage/minify.py")
set(OUTPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/webpage/index.html.gz")

add_custom_command(
    OUTPUT ${OUTPUT_FILE}
    COMMAND ${python} ${SCRIPT_PATH}
    COMMENT "Generating ${OUTPUT_FILE}"
)

add_custom_target(generate_webpage DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/webpage/index.html.gz")
add_dependencies(${COMPONENT_LIB} generate_webpage)
