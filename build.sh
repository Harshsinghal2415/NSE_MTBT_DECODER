#!/bin/bash
# Linux/macOS build script for NSE MTBT Decoder

echo "Building NSE MTBT Decoder..."

# Check if g++ is available
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ not found. Please install GCC"
    echo "Ubuntu/Debian: sudo apt-get install build-essential"
    echo "CentOS/RHEL: sudo yum install gcc-c++"
    echo "macOS: xcode-select --install"
    exit 1
fi

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