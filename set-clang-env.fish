#!/usr/bin/fish

# Set ESP Clang compiler paths
set -x CC /home/phi/.espressif/tools/esp-clang/esp-18.1.2_20240912/esp-clang/bin/clang
set -x CXX /home/phi/.espressif/tools/esp-clang/esp-18.1.2_20240912/esp-clang/bin/clang++

# Source ESP-IDF environment
source $HOME/esp/v5.4.2/esp-idf/export.fish

echo "ESP Clang environment ready"
