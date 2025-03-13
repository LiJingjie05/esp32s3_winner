# main/ui_demo/CMakeLists.txt
idf_component_register(
    SRCS "esp_http_server.c"
    INCLUDE_DIRS "."
    REQUIRES esp_stubs esp-tls esp_http_server protocol_examples_common nvs_flash
)
