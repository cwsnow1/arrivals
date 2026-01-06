cmake_minimum_required(VERSION 3.16)

project(lvgl_linux)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY  https://github.com/raysan5/raylib.git
    GIT_TAG         c1ab645ca298a2801097931d1079b10ff7eb9df8
)
FetchContent_Declare(
    lvgl
    GIT_REPOSITORY https://github.com/lvgl/lvgl.git
)
FetchContent_MakeAvailable(raylib lvgl)

add_executable(${PROJECT_NAME}
    main.c
    ../ui.c
    ../arimo_24.c
    ../arimo_32.c
    ../callbacks/x86_64/ui_callbacks.c
)
target_link_libraries(${PROJECT_NAME}
    raylib
    lvgl
)
target_include_directories(${PROJECT_NAME}
    PRIVATE
    ..
    ../callbacks
)
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    LCD_V=240
    LCD_H=320
)
