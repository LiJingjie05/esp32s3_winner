# main/ui_demo/CMakeLists.txt
idf_component_register(
    SRCS "ui.c" "ui_Screen1.c" "ui_comp_hook.c" "ui_helpers.c"
    INCLUDE_DIRS "."
    REQUIRES lvgl
)
