cmake_minimum_required(VERSION 3.16)

# Simple source list without code generation for now
set(srcs
    "src/esp_menu.c"
    "assets/user_actions.c"
    "assets/user_graphic.c"
)

set(requires
    "lvgl"
    "esp_lvgl_port" 
    "esp_driver_gpio"
    "driver"
    "esp_lcd"
    "button"
    "knob"
    "nvs_flash"
)

# Add NVS support if enabled
# (Now included directly in the requires list)

# Simple component registration without code generation
idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "include" "assets"
    REQUIRES ${requires}
)