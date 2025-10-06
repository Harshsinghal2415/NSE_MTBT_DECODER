#!/bin/bash#!/bin/bash

# Linux/macOS build script for NSE MTBT Decoder

# NSE MTBT Decoder - Cross-platform build script for Unix-like systems

# Supports: Linux, macOS, FreeBSD, and other Unix variantsecho "Building NSE MTBT Decoder..."



set -e  # Exit on any error# Find the best available C++ compiler

COMPILER=""

# Colors for output

RED='\033[0;31m'# Check for modern GCC versions first

GREEN='\033[0;32m'for gcc_cmd in g++-13 g++-12 g++-11 g++-10 g++-9 g++-8 g++-7 g++; do

YELLOW='\033[1;33m'    if command -v "$gcc_cmd" &> /dev/null; then

BLUE='\033[0;34m'        # Check if it supports C++17

CYAN='\033[0;36m'        if "$gcc_cmd" -std=c++17 -x c++ -c /dev/null -o /dev/null 2>/dev/null; then

NC='\033[0m' # No Color            COMPILER="$gcc_cmd"

            echo "Using compiler: $COMPILER ($(\"$gcc_cmd\" --version | head -n1))"

# Detect OS and architecture            break

OS=$(uname -s)        fi

ARCH=$(uname -m)    fi

done

echo -e "${CYAN}🚀 NSE MTBT Decoder - Build Script${NC}"

echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}"# If no suitable compiler found

echo -e "${YELLOW}Detected system: ${OS} (${ARCH})${NC}"if [ -z "$COMPILER" ]; then

    echo "Error: No C++17 compatible compiler found!"

# Check for required tools    echo

check_tool() {    echo "Please install a modern GCC compiler:"

    if ! command -v "$1" &> /dev/null; then    echo "  Ubuntu/Debian: sudo apt-get install build-essential"

        echo -e "${RED}❌ Error: $1 is not installed or not in PATH${NC}"    echo "  CentOS/RHEL:   sudo yum install gcc-c++"

        echo -e "${YELLOW}Please install $1 and try again${NC}"    echo "  Fedora:        sudo dnf install gcc-c++"

        exit 1    echo "  macOS:         xcode-select --install"

    fi    echo "  Arch Linux:    sudo pacman -S gcc"

}    echo

    echo "Minimum required: GCC 7.0 or later for C++17 support"

echo -e "${BLUE}🔍 Checking build dependencies...${NC}"    exit 1

check_tool cmakefi

check_tool make

# Clean previous build

# Detect C++ compilerrm -f NSE_MTBT_Decoder

if command -v g++ &> /dev/null; then

    CXX_COMPILER="g++"# Compile with C++17

    echo -e "${GREEN}✅ Found GCC compiler: $(g++ --version | head -n1)${NC}"echo "Compiling with C++17..."

elif command -v clang++ &> /dev/null; then"$COMPILER" -std=c++17 -Wall -Wextra -O2 -I src src/*.cpp -o NSE_MTBT_Decoder

    CXX_COMPILER="clang++"

    echo -e "${GREEN}✅ Found Clang compiler: $(clang++ --version | head -n1)${NC}"if [ $? -ne 0 ]; then

else    echo "Build failed!"

    echo -e "${RED}❌ Error: No C++ compiler found (g++ or clang++)${NC}"    exit 1

    exit 1fi

fi

echo "Build successful!"

# Set build typeecho

BUILD_TYPE=${1:-Release}echo "Usage: ./NSE_MTBT_Decoder --help"

if [[ "$BUILD_TYPE" != "Debug" && "$BUILD_TYPE" != "Release" ]]; thenecho "Example: ./NSE_MTBT_Decoder --count 1000 --csv --output trades.csv"

    echo -e "${YELLOW}⚠️  Invalid build type '$BUILD_TYPE'. Using 'Release'${NC}"echo
    BUILD_TYPE="Release"
fi

echo -e "${BLUE}🔨 Build configuration: ${BUILD_TYPE}${NC}"

# Create build directory
echo -e "${BLUE}📁 Creating build directory...${NC}"
mkdir -p build
cd build

# Configure with CMake
echo -e "${BLUE}⚙️  Configuring with CMake...${NC}"
case "$OS" in
    "Linux")
        cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
              -DCMAKE_CXX_COMPILER="$CXX_COMPILER" \
              -DCMAKE_CXX_STANDARD=17 \
              ..
        ;;
    "Darwin")  # macOS
        cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
              -DCMAKE_CXX_COMPILER="$CXX_COMPILER" \
              -DCMAKE_CXX_STANDARD=17 \
              -DCMAKE_OSX_DEPLOYMENT_TARGET=10.14 \
              ..
        ;;
    *)  # Other Unix-like systems
        cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
              -DCMAKE_CXX_COMPILER="$CXX_COMPILER" \
              -DCMAKE_CXX_STANDARD=17 \
              ..
        ;;
esac

# Build the project
echo -e "${BLUE}🔨 Building NSE MTBT Decoder...${NC}"
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Check if build was successful
if [[ -f "NSE_MTBT_Decoder" ]]; then
    echo -e "${GREEN}✅ Build completed successfully!${NC}"
    echo -e "${CYAN}📦 Executable: build/NSE_MTBT_Decoder${NC}"
    
    # Show file info
    ls -la NSE_MTBT_Decoder
    
    echo -e "\n${BLUE}🚀 To run the decoder:${NC}"
    echo -e "${YELLOW}   ./build/NSE_MTBT_Decoder --count 10${NC}"
    echo -e "${YELLOW}   ./build/NSE_MTBT_Decoder --count 1000 --csv${NC}"
    echo -e "${YELLOW}   ./build/NSE_MTBT_Decoder --help${NC}"
else
    echo -e "${RED}❌ Build failed!${NC}"
    exit 1
fi

echo -e "${GREEN}🎉 NSE MTBT Decoder build completed successfully!${NC}"