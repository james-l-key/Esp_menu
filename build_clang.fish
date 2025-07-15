#!/usr/bin/fish
# ESP Menu Clang Build Script

# Source ESP-IDF environment directly (non-interactive)
source /home/phi/esp/v5.4.2/esp-idf/export.fish

echo "Setting up Clang environment..."
set -x IDF_TOOLCHAIN clang
set -x CC /home/phi/.espressif/tools/esp-clang/esp-18.1.2_20240912/esp-clang/bin/clang
set -x CXX /home/phi/.espressif/tools/esp-clang/esp-18.1.2_20240912/esp-clang/bin/clang++

echo "Environment configured:"
echo "IDF_TOOLCHAIN: $IDF_TOOLCHAIN"
echo "CC: $CC"

# Get script directory and navigate to build directory
set script_dir (dirname (status --current-filename))
echo "Navigating to build directory: $script_dir/examples/basic_menu"
cd $script_dir/examples/basic_menu

echo "Building with relaxed warnings for external components..."
idf.py build -DCMAKE_C_FLAGS="-Wno-logical-not-parentheses" -DCMAKE_CXX_FLAGS="-Wno-logical-not-parentheses"

echo "Build script completed."
