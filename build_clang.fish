#!/usr/bin/fish
# ESP Menu GCC Build Script

# Source ESP-IDF environment directly (non-interactive)
source /home/phi/esp/v5.5/esp-idf/export.fish

echo "Using default GCC toolchain..."
echo "Toolchain: $IDF_TOOLCHAIN"

# Get script directory and navigate to build directory
set script_dir (dirname (status --current-filename))
echo "Navigating to build directory: $script_dir/examples/basic_menu"
cd $script_dir/examples/basic_menu

echo "Building with GCC..."
idf.py build

echo "Build script completed."
