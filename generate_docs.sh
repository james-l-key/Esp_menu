#!/bin/bash
# Generate documentation for ESP Menu project using Doxygen

# Check if doxygen is installed
if ! command -v doxygen &> /dev/null; then
    echo "Doxygen not found. Please install it first:"
    echo "  sudo apt-get install doxygen graphviz"
    exit 1
fi

# Set the current directory to the project root
cd "$(dirname "$0")" || exit 1

# Create the docs directory if it doesn't exist
mkdir -p docs

# Run doxygen
echo "Generating documentation with Doxygen..."
doxygen Doxyfile

# Check if generation was successful
if [ $? -eq 0 ]; then
    echo "Documentation generated successfully in the 'docs' directory"
    echo "Open docs/html/index.html in a web browser to view"
else
    echo "Error: Doxygen failed to generate documentation"
    exit 1
fi

exit 0
