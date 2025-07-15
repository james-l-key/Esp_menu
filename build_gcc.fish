#!/usr/bin/fish
# ESP Menu GCC Build Script

# Source ESP-IDF environment directly (non-interactive)
source /home/phi/esp/v5.4.2/esp-idf/export.fish

echo "Setting up GCC environment..."
# Use default GCC toolchain (remove any Clang settings)
set -e IDF_TOOLCHAIN
set -e CC
set -e CXX

echo "Environment configured for GCC:"
echo "Using ESP-IDF default GCC toolchain"

# Get script directory and navigate to build directory
set script_dir (dirname (status --current-filename))
echo "Navigating to build directory: $script_dir/examples/basic_menu"
cd $script_dir/examples/basic_menu

echo "Building with GCC..."
idf.py build

echo "Build script completed."
