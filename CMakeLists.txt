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
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(picow_http_client C CXX ASM)

set(WIFI_SSID "")
set(WIFI_PASSWORD "")

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(http_client_mod)

# Add executable. Default name is the project name, version 0.1

add_library(example_lwip_http_util INTERFACE)
target_sources(example_lwip_http_util INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/example_http_client_util.c
        )
target_link_libraries(example_lwip_http_util INTERFACE
        pico_lwip_http_client_mod
        pico_lwip_mbedtls
        pico_mbedtls
        )
target_include_directories(example_lwip_http_util INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        )

add_executable(picow_http_client
        picow_http_client.c
        )
target_compile_definitions(picow_http_client PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )
target_include_directories(picow_http_client PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts and mbedtls_config
        )
target_link_libraries(picow_http_client
        pico_cyw43_arch_lwip_threadsafe_background
        example_lwip_http_util
        pico_stdlib
        )
pico_add_extra_outputs(picow_http_client)

# Ignore warnings from lwip code
set_source_files_properties(
        ${PICO_LWIP_PATH}/src/apps/altcp_tls/altcp_tls_mbedtls.c
        PROPERTIES
        COMPILE_OPTIONS "-Wno-unused-result"
        )
