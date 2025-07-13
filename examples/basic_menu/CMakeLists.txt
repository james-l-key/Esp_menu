cmake_minimum_required(VERSION 3.16)
# Add the parent directory components as available components
set(EXTRA_COMPONENT_DIRS "../../components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(basic_menu)