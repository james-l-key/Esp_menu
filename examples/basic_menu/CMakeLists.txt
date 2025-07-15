cmake_minimum_required(VERSION 3.16)

# Force Clang compiler usage
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

# Add the parent directory components as available components
set(EXTRA_COMPONENT_DIRS "../../components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(basic_menu)