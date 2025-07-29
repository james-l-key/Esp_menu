cmake_minimum_required(VERSION 3.16)

set(srcs
    "src/esp_menu.c"
    "generated/menu.c"
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

# Create the generated directory if it doesn't exist
file(MAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/generated")

# Create an empty menu_data.h if it doesn't exist
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/generated/menu_data.h")
    file(WRITE "${CMAKE_CURRENT_LIST_DIR}/generated/menu_data.h" "
    // Auto-generated menu data
    #ifndef MENU_DATA_H
    #define MENU_DATA_H
    void menu_init(void);
    #endif
    ")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "include" "generated" "assets"
    REQUIRES ${requires}
)

# After component registration, run the setup script if Python is available
find_program(PYTHON "python3")
if(PYTHON)
    # Run the template-based menu generation script
    add_custom_target(menu_template_generation ALL
        COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/generate_menu_from_templates.py
                ${CMAKE_CURRENT_SOURCE_DIR}/assets/menu.json
                ${CMAKE_CURRENT_SOURCE_DIR}/generated
                ${CMAKE_CURRENT_SOURCE_DIR}/assets/templates
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating menu files from templates"
        VERBATIM
    )
endif()