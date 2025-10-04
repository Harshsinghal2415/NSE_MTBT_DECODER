#!/bin/bash
# Linux/macOS build script for NSE MTBT Decoder

echo "Building NSE MTBT Decoder..."

# Find the best available C++ compiler
COMPILER=""

# Check for modern GCC versions first
for gcc_cmd in g++-13 g++-12 g++-11 g++-10 g++-9 g++-8 g++-7 g++; do
    if command -v "$gcc_cmd" &> /dev/null; then
        # Check if it supports C++17
        if "$gcc_cmd" -std=c++17 -x c++ -c /dev/null -o /dev/null 2>/dev/null; then
            COMPILER="$gcc_cmd"
            echo "Using compiler: $COMPILER ($(\"$gcc_cmd\" --version | head -n1))"
            break
        fi
    fi
done

# If no suitable compiler found
if [ -z "$COMPILER" ]; then
    echo "Error: No C++17 compatible compiler found!"
    echo
    echo "Please install a modern GCC compiler:"
    echo "  Ubuntu/Debian: sudo apt-get install build-essential"
    echo "  CentOS/RHEL:   sudo yum install gcc-c++"
    echo "  Fedora:        sudo dnf install gcc-c++"
    echo "  macOS:         xcode-select --install"
    echo "  Arch Linux:    sudo pacman -S gcc"
    echo
    echo "Minimum required: GCC 7.0 or later for C++17 support"
    exit 1
fi

# Clean previous build
rm -f NSE_MTBT_Decoder

# Compile with C++17
echo "Compiling with C++17..."
"$COMPILER" -std=c++17 -Wall -Wextra -O2 -I src src/*.cpp -o NSE_MTBT_Decoder

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"
echo
echo "Usage: ./NSE_MTBT_Decoder --help"
echo "Example: ./NSE_MTBT_Decoder --count 1000 --csv --output trades.csv"
echo