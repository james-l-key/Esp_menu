cmake_minimum_required(VERSION 3.16)


# List of generated files
set(GENERATED_MENU_C "${CMAKE_CURRENT_LIST_DIR}/generated/menu.c")
set(GENERATED_MENU_DATA_H "${CMAKE_CURRENT_LIST_DIR}/generated/menu_data.h")

# List of sources
set(srcs
    "src/esp_menu.c"
    ${GENERATED_MENU_C}
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


# Ensure generated directory exists
file(MAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/generated")

# Custom command to generate menu.c and menu_data.h from templates
find_program(PYTHON "python3")
if(PYTHON)
    add_custom_command(
        OUTPUT ${GENERATED_MENU_C} ${GENERATED_MENU_DATA_H}
        COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/generate_menu_from_templates.py
                ${CMAKE_CURRENT_SOURCE_DIR}/assets/menu.json
                ${CMAKE_CURRENT_SOURCE_DIR}/generated
                ${CMAKE_CURRENT_SOURCE_DIR}/assets/templates
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/menu.json
                ${CMAKE_CURRENT_SOURCE_DIR}/assets/templates/menu.c.j2
                ${CMAKE_CURRENT_SOURCE_DIR}/assets/templates/menu.h.j2
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating menu files from templates"
        VERBATIM
    )
    add_custom_target(menu_template_generation ALL
        DEPENDS ${GENERATED_MENU_C} ${GENERATED_MENU_DATA_H}
    )
endif()


# Register the component and make it depend on the generated files
idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "include" "generated" "assets"
    REQUIRES ${requires}
)
add_dependencies(${COMPONENT_LIB} menu_template_generation)

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