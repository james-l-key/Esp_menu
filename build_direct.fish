#!/usr/bin/fish
source /home/phi/esp/v5.4.2/esp-idf/export.fish
set -x IDF_TOOLCHAIN clang
set -x CC /home/phi/.espressif/tools/esp-clang/esp-18.1.2_20240912/esp-clang/bin/clang
set -x CXX /home/phi/.espressif/tools/esp-clang/esp-18.1.2_20240912/esp-clang/bin/clang++
cd examples/basic_menu
idf.py build -DCMAKE_C_FLAGS="-Wno-logical-not-parentheses"
