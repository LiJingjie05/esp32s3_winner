set(requires "")
idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    list(APPEND requires esp_stubs esp-tls esp_http_server protocol_examples_common nvs_flash)
endif()
idf_component_register(SRCS "main.c" "lvgl_init.c" "lvgl_demo_ui.c" "http_get_hw_info.c" "esp_http_server.c"
                       INCLUDE_DIRS "."
                       REQUIRES ${requires})
