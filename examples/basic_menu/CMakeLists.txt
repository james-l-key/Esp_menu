cmake_minimum_required(VERSION 3.16)

# Add the parent directory components as available components
set(EXTRA_COMPONENT_DIRS "../../components")

# Add compiler flags to suppress external component warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-logical-not-parentheses")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-logical-not-parentheses")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(basic_menu)