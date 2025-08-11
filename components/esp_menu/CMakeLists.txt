# ESP Menu component CMake

cmake_minimum_required(VERSION 3.16)

set(COMPONENT_DIR ${CMAKE_CURRENT_LIST_DIR})

# Locate project root (examples/basic_menu includes this component via EXTRA_COMPONENT_DIRS)
get_filename_component(PROJECT_DIR "${COMPONENT_DIR}/../.." ABSOLUTE)

# Paths
set(GENERATED_DIR ${COMPONENT_DIR}/generated)

# Prefer project-level assets; fall back to component-level copies
set(JSON_CONFIG "${PROJECT_DIR}/assets/menu.json")
if(NOT EXISTS "${JSON_CONFIG}")
	set(JSON_CONFIG "${COMPONENT_DIR}/assets/menu.json")
endif()

set(TEMPLATES_DIR "${PROJECT_DIR}/assets/templates")
if(NOT EXISTS "${TEMPLATES_DIR}/menu.c.j2")
	set(TEMPLATES_DIR "${COMPONENT_DIR}/assets/templates")
endif()

# Ensure generated dir exists
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Generated outputs
set(GENERATED_MENU_C  ${GENERATED_DIR}/menu.c)
set(GENERATED_MENU_H  ${GENERATED_DIR}/menu_data.h)

# Generator script
set(GENERATOR_PY "${PROJECT_DIR}/scripts/generate_menu_from_templates.py")

# Find Python from IDF or fallback
if(NOT DEFINED PYTHON)
	find_package(Python3 REQUIRED COMPONENTS Interpreter)
	set(PYTHON ${Python3_EXECUTABLE})
endif()

# Auto-generate menu sources when JSON or templates change (only in normal configure/generate, not script mode)
if(NOT CMAKE_SCRIPT_MODE_FILE)
	add_custom_command(
		OUTPUT ${GENERATED_MENU_C} ${GENERATED_MENU_H}
		COMMAND ${PYTHON} ${GENERATOR_PY} ${JSON_CONFIG} ${GENERATED_DIR} ${TEMPLATES_DIR}
		DEPENDS ${JSON_CONFIG}
				${TEMPLATES_DIR}/menu.c.j2
				${TEMPLATES_DIR}/menu.h.j2
				${GENERATOR_PY}
		WORKING_DIRECTORY ${PROJECT_DIR}
		COMMENT "Generating ESP Menu sources from templates"
		VERBATIM
	)

	add_custom_target(esp_menu_codegen DEPENDS ${GENERATED_MENU_C} ${GENERATED_MENU_H})
endif()

# Choose user graphics source from project or component
set(USER_GRAPHIC_SRC "")
if(EXISTS "${PROJECT_DIR}/assets/user_graphic.c")
	list(APPEND USER_GRAPHIC_SRC "${PROJECT_DIR}/assets/user_graphic.c")
elseif(EXISTS "${COMPONENT_DIR}/assets/user_graphic.c")
	list(APPEND USER_GRAPHIC_SRC "${COMPONENT_DIR}/assets/user_graphic.c")
endif()

# Core sources
set(ESP_MENU_SOURCES
	${COMPONENT_DIR}/src/esp_menu.c
	${COMPONENT_DIR}/src/user_actions.c
	${GENERATED_MENU_C}
	${USER_GRAPHIC_SRC}
)

# Headers
set(ESP_MENU_INCLUDE_DIRS
	${COMPONENT_DIR}/include
	${GENERATED_DIR}
	${PROJECT_DIR}/assets
	${COMPONENT_DIR}/assets
)

# Dependencies used by the component
set(ESP_MENU_REQUIRES
	esp_lcd
	lvgl
	esp_lvgl_port
	button
	knob
	nvs_flash
)

idf_component_register(
	SRCS ${ESP_MENU_SOURCES}
	INCLUDE_DIRS ${ESP_MENU_INCLUDE_DIRS}
	REQUIRES ${ESP_MENU_REQUIRES}
)

# Ensure generation runs before compiling this component (only when target exists)
if(TARGET esp_menu_codegen)
	add_dependencies(${COMPONENT_LIB} esp_menu_codegen)
endif()

# Mark generated files
set_source_files_properties(${GENERATED_MENU_C} PROPERTIES GENERATED TRUE)

