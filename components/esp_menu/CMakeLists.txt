
cmake_minimum_required(VERSION 3.16)

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

# Register the component (no generated files logic here)
idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "include" "assets"
    REQUIRES ${requires}
)