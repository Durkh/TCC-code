cmake_minimum_required(VERSION 3.24)

if (__ESP32__)
    add_definitions(-D__ESP32__=1)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
elseif(__RPICO__)
    add_definitions(-D__RPICO__=1)
    include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
endif()

set(PROJECT_NAME DCP_Test)

project(${PROJECT_NAME} LANGUAGES C)

if(__RPICO__)
    pico_sdk_init()

    target_link_libraries(${PROJECT_NAME} pico_stdlib)

    if (PICO_CYW43_SUPPORTED)
        target_link_libraries(${PROJECT_NAME} pico_cyw43_arch_none)
    endif()

    # create map/bin/hex file etc.
    pico_add_extra_outputs(${PROJECT_NAME})
endif()

