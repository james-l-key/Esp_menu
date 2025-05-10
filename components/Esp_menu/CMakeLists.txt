# Ensure Python dependencies are installed
execute_process(
    COMMAND ${CMAKE_COMMAND} -E env "PATH=$ENV{PATH}"
        python3 -m pip install jinja2 jsonschema
    RESULT_VARIABLE pip_result
)
if(NOT pip_result EQUAL "0")
    message(FATAL_ERROR "Failed to install Python dependencies: jinja2, jsonschema")
endif()

# Run the Python script to generate menu_data.h and menu.c
execute_process(
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/generate_menu.py
        ${CMAKE_CURRENT_SOURCE_DIR}/menu.json
        ${CMAKE_BINARY_DIR}/esp-idf/Esp_menu/menu_data.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE script_result
)
if(NOT script_result EQUAL "0")
    message(FATAL_ERROR "Code generation failed")
endif()

# Include generated files
set(srcs
    "Esp_menu.c"
    "user_actions.c"
    "generated/menu.c"
)
idf_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS "." "include" "generated" "${CMAKE_BINARY_DIR}/esp-idf/Esp_menu"
    PRIV_REQUIRES lvgl espressif__lvgl_port espressif__knob espressif__button
)