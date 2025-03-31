# menu/CMakeLists.txt
idf_component_register(SRCS "Esp_menu.c"
    INCLUDE_DIRS "include" "."
    REQUIRES esp_lcd driver esp_lvgl_port lvgl button knob)

# Generate the menu header using custom command
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/menu_data.h
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/generate_menu_header.py
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/menu.json
    VERBATIM
)

# Add the generated header as a dependency
add_custom_target(generate_menu_header ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/menu_data.h
)

# Make your component depend on the generated header
add_dependencies(${COMPONENT_LIB} generate_menu_header)