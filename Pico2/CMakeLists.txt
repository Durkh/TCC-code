cmake_minimum_required(VERSION 3.13)

set(FREERTOS_KERNEL_PATH "${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel")

if (NOT FREERTOS_KERNEL_PATH AND NOT DEFINED ENV{FREERTOS_KERNEL_PATH})
    message("Skipping FreeRTOS examples as FREERTOS_KERNEL_PATH not defined")
    return()
endif()

include(pico_sdk_import.cmake)
include(FreeRTOS_Kernel_import.cmake)

project(teste C CXX ASM)

pico_sdk_init()

add_subdirectory(hello_freertos)
