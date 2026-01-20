# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD seeed_xiao_rp2040 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(airled C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(airled)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)

# generate the header file into the source tree as it is included in the RP2040 datasheet
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/airled.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

target_sources(${PROJECT_NAME} PRIVATE
    airled.c
    ws2812.c
    patterns.c
    wing_plane.c
    button.c
    led-controller.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE pico_stdlib hardware_pio hardware_dma)
pico_add_extra_outputs(${PROJECT_NAME})

# Additionally generate python and hex pioasm outputs for inclusion in the RP2040 datasheet
add_custom_target(airled_datasheet DEPENDS ${CMAKE_CURRENT_LIST_DIR}/generated/airled.py)
add_custom_command(OUTPUT ${CMAKE_CURRENT_LIST_DIR}/generated/airled.py
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/airled.pio
        COMMAND pioasm -o python ${CMAKE_CURRENT_LIST_DIR}/airled.pio ${CMAKE_CURRENT_LIST_DIR}/generated/airled.py
        VERBATIM)
add_dependencies(${PROJECT_NAME} airled_datasheet)