cmake_minimum_required(VERSION 3.16)

set(srcs
    "src/esp_menu.c"
    "generated/menu.c"
    "assets/user_actions.c"
)

set(requires
    "lvgl"
    "esp_lvgl_port" 
    "esp_driver_gpio"
    "esp_driver_i2c"
    "esp_lcd"
    "button"
    "knob"
)

# Add NVS support if enabled
if(CONFIG_ESPMENU_ENABLE_NVS)
    list(APPEND requires "nvs_flash")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "include" "generated" "assets"
    REQUIRES ${requires}
)