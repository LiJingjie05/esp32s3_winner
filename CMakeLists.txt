cmake_minimum_required(VERSION 3.16)

# set(EXTRA_COMPONENT_DIRS "./main/ui_demo")
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common "./main/ui_demo")

if("${IDF_TARGET}" STREQUAL "linux")
    list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/freertos/"
                    "$ENV{IDF_PATH}/examples/protocols/linux_stubs/esp_stubs")
    set(COMPONENTS main)
endif()
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(spi_lcd_touch)
