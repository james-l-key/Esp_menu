idf_component_register(
    SRCS
        "esp_menu.c"
        "../generated/menu.c"
        "../assets/user_actions.c"
    INCLUDE_DIRS
        "../include"
        "../generated"
        "../assets"
    REQUIRES
        driver
        lvgl
        button
        esp_lvgl_port
)

# Define paths
set(JSON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../${CONFIG_ESP_MENU_JSON_PATH}")
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../generated")
set(TEMPLATES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../assets/templates")
set(GENERATED_C "${OUTPUT_DIR}/menu.c")
set(GENERATED_H "${OUTPUT_DIR}/menu_data.h")
set(PYTHON_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/generate_menu_from_templates.py")

# Custom command to run Python script
add_custom_command(
    OUTPUT ${GENERATED_C} ${GENERATED_H}
    COMMAND python3 ${PYTHON_SCRIPT} ${JSON_FILE} ${OUTPUT_DIR} ${TEMPLATES_DIR}
    DEPENDS ${JSON_FILE} ${PYTHON_SCRIPT} ${TEMPLATES_DIR}/menu.c.j2 ${TEMPLATES_DIR}/menu.h.j2
    COMMENT "Generating menu code from JSON"
    VERBATIM
)

# Ensure generated files are built
add_custom_target(generate_menu DEPENDS ${GENERATED_C} ${GENERATED_H})
add_dependencies(${COMPONENT_LIB} generate_menu)