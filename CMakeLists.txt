cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ENV{IDF_TOOLCHAIN})
	set(ENV{IDF_TOOLCHAIN} "clang")
endif()
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Add global Clang-only suppression before project() so it applies to all components.
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
	idf_build_set_property(COMPILE_OPTIONS "-Wno-logical-not-parentheses" APPEND)
	idf_build_set_property(COMPILE_OPTIONS "-Wno-error=logical-not-parentheses" APPEND)
	# Also add as global compile options to ensure third-party/managed targets pick it up
	add_compile_options($<$<C_COMPILER_ID:Clang>:-Wno-logical-not-parentheses>)
	add_compile_options($<$<C_COMPILER_ID:Clang>:-Wno-error=logical-not-parentheses>)
endif()
# Ensure generated directory exists
file(MAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/generated")

# Paths for generated files
set(GENERATED_MENU_C "${CMAKE_CURRENT_LIST_DIR}/generated/menu.c")
set(GENERATED_MENU_DATA_H "${CMAKE_CURRENT_LIST_DIR}/generated/menu_data.h")

# Custom command to generate menu.c and menu_data.h from templates
find_program(PYTHON "python3")
if(PYTHON)
    add_custom_command(
        OUTPUT ${GENERATED_MENU_C} ${GENERATED_MENU_DATA_H}
        COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_menu_from_templates.py
                ${CMAKE_CURRENT_SOURCE_DIR}/assets/menu.json
                ${CMAKE_CURRENT_LIST_DIR}/generated
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

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "include" "assets" "generated"
    REQUIRES ${requires}
)

add_dependencies(${COMPONENT_LIB} menu_template_generation)