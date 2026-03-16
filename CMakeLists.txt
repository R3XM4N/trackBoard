cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(usb_keyboard C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(usb_keyboard
    main.cpp
    usb_descriptors.c
)

pico_add_extra_outputs(usb_keyboard)

target_include_directories(usb_keyboard PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(usb_keyboard
    pico_stdlib
    tinyusb_device
    tinyusb_board
)

pico_enable_stdio_usb(usb_keyboard 0)
pico_enable_stdio_uart(usb_keyboard 0)