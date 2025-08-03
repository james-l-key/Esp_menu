cmake_minimum_required(VERSION 3.16)

# ESP Menu Component
set(COMPONENT_SRCS 
    "src/esp_menu.c" 
    "assets/user_actions.c"
    "assets/user_graphic.c"
    "generated/menu.c"
)

set(COMPONENT_ADD_INCLUDEDIRS 
    "include" 
    "generated"
)

# Register the component with required dependencies
idf_component_register(
    SRCS 
        "src/esp_menu.c"
        "assets/user_actions.c"
        "assets/user_graphic.c"
        "generated/menu.c"
    INCLUDE_DIRS 
        "include"
        "assets"
        "generated"
    REQUIRES 
        esp_driver_i2c
        esp_lcd
        esp_lvgl_port
        button
        knob
        nvs_flash
        driver
)