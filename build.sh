#!/bin/bash
# Linux/macOS build script for NSE MTBT Decoder

echo "Building NSE MTBT Decoder..."

# Check if g++ is available and supports C++17
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ not found. Please install GCC"
    echo "Ubuntu/Debian: sudo apt-get install build-essential"
    echo "CentOS/RHEL: sudo yum install gcc-c++"
    echo "macOS: xcode-select --install"
    exit 1
fi

# Check C++17 support
if ! g++ -std=c++17 -x c++ -c /dev/null -o /dev/null 2>/dev/null; then
    echo "Error: Your g++ version doesn't support C++17"
    echo "Please install GCC 7.0 or later"
    exit 1
fi

echo "Using g++ version: $(g++ --version | head -n1)"

# Clean previous build
rm -f NSE_MTBT_Decoder

# Compile with C++17
echo "Compiling with C++17..."
g++ -std=c++17 -Wall -Wextra -O2 -I src src/*.cpp -o NSE_MTBT_Decoder

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"
echo
echo "Usage: ./NSE_MTBT_Decoder --help"
echo "Example: ./NSE_MTBT_Decoder --count 1000 --csv --output trades.csv"
echo