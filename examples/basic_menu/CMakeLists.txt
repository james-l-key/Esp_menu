cmake_minimum_required(VERSION 3.16)

# Make the component in ../../components visible to this project
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../components")

# Include IDF build system once
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(basic_menu)

# Optional: Suppress specific warnings when using Clang toolchain
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
	idf_build_set_property(COMPILE_OPTIONS "-Wno-logical-not-parentheses" APPEND)
	idf_build_set_property(COMPILE_OPTIONS "-Wno-error=logical-not-parentheses" APPEND)
endif()