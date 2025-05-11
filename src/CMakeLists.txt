set(srcs
    "Esp_menu.c"
    "generated/menu.c"
)
set(requires
    lvgl
    esp_lvgl_port
    espressif__knob
    espressif__button
)
if(CONFIG_ESPMENU_ENABLE_NVS)
    list(APPEND requires nvs_flash)
endif()

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
    SRCS "${srcs}"
    INCLUDE_DIRS "." "include" "generated" "../../main" "../../main/user_menu"
    REQUIRES "${requires}"
)

# After component registration, run the setup script if Python is available
find_program(PYTHON "python3")
if(PYTHON)
    # First run the original script to generate framework files
    add_custom_target(menu_data_generation ALL
        COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/generate_menu.py 
                ${PROJECT_SOURCE_DIR}/main/user_menu/menu.json
                ${CMAKE_CURRENT_SOURCE_DIR}/generated/menu_data.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating menu data from JSON"
        VERBATIM
    )
    
    # Then run the new template-based menu generation script
    add_custom_target(menu_template_generation ALL
        COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/generate_menu_from_templates.py
                ${PROJECT_SOURCE_DIR}/main/user_menu/menu.json
                ${CMAKE_CURRENT_SOURCE_DIR}/generated
                ${CMAKE_CURRENT_SOURCE_DIR}/templates
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating menu files from templates"
        VERBATIM
        DEPENDS menu_data_generation
    )
endif()