set(TARGET_NAME DCP_test)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../DCP ${CMAKE_BINARY_DIR}/DCP)

target_include_directories(DCP PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/..
)

add_executable(${TARGET_NAME}
    main.c
    ${CMAKE_CURRENT_LIST_DIR}/../DCP/rp2350/port.c
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/..
)

target_link_libraries(${TARGET_NAME} PRIVATE
    pico_async_context_freertos
    FreeRTOS-Kernel-Heap4
    pico_stdlib
    DCP
)

pico_enable_stdio_usb(${TARGET_NAME} 1)
pico_enable_stdio_uart(${TARGET_NAME} 0)

pico_add_extra_outputs(${TARGET_NAME})
