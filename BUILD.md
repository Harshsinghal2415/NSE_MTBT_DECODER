# NSE MTBT Decoder - Cross-Platform Build Guide

This document explains how to build the NSE MTBT Decoder on different operating systems.

## 🔧 Prerequisites

### All Platforms
- **CMake 3.16+** - [Download CMake](https://cmake.org/download/)
- **C++17 compatible compiler**

### Platform-Specific Requirements

#### Windows
- **MinGW-w64** (recommended) - [Download](https://www.mingw-w64.org/)
- **Visual Studio 2019+** - [Download](https://visualstudio.microsoft.com/)
- **MSYS2** - [Download](https://www.msys2.org/)

#### Linux
- **GCC 7+** or **Clang 6+**
- Install via package manager:
  ```bash
  # Ubuntu/Debian
  sudo apt install build-essential cmake
  
  # CentOS/RHEL/Fedora
  sudo yum install gcc-c++ cmake
  # or
  sudo dnf install gcc-c++ cmake
  
  # Arch Linux
  sudo pacman -S base-devel cmake
  ```

#### macOS
- **Xcode Command Line Tools**
  ```bash
  xcode-select --install
  ```
- **Homebrew** (optional but recommended)
  ```bash
  brew install cmake
  ```

## 🚀 Building the Project

### Method 1: Using Build Scripts (Recommended)

#### Windows

**Option A: PowerShell Script (Recommended)**
```powershell
# Build Release version
.\build.ps1

# Build Debug version
.\build.ps1 -BuildType Debug

# Clean build
.\build.ps1 -Clean

# Verbose output
.\build.ps1 -Verbose

# Show help
.\build.ps1 -Help
```

**Option B: Batch Script**
```cmd
# Build Release version
.\build.bat

# Build Debug version
.\build.bat Debug
```

#### Linux/macOS/Unix
```bash
# Make script executable (first time only)
chmod +x build.sh

# Build Release version
./build.sh

# Build Debug version
./build.sh Debug
```

### Method 2: Direct CMake Commands

#### All Platforms
```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Or specify build type explicitly
cmake --build build --config Release
```

#### Platform-Specific CMake Options

**Windows with MinGW:**
```cmd
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

**Windows with Visual Studio:**
```cmd
cmake -B build -G "Visual Studio 16 2019" -A x64
cmake --build build --config Release
```

**Linux:**
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
cmake --build build -j$(nproc)
```

**macOS:**
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.14
cmake --build build -j$(sysctl -n hw.ncpu)
```

## 🏃 Running the Decoder

After building, the executable will be located at:
- **Windows**: `build\NSE_MTBT_Decoder.exe`
- **Linux/macOS**: `build/NSE_MTBT_Decoder`

### Usage Examples
```bash
# Basic usage
./build/NSE_MTBT_Decoder --count 10

# Generate CSV output
./build/NSE_MTBT_Decoder --count 1000 --csv --output trades.csv

# Test with errors
./build/NSE_MTBT_Decoder --test-errors --seed 42

# Show help
./build/NSE_MTBT_Decoder --help
```

## 🛠️ Troubleshooting

### Common Issues

#### "CMake not found"
- Install CMake and ensure it's in your PATH
- On Windows, restart your terminal after installation

#### "No C++ compiler found"
- **Windows**: Install MinGW-w64 or Visual Studio
- **Linux**: Install `build-essential` or equivalent package
- **macOS**: Install Xcode Command Line Tools

#### "Permission denied" (Linux/macOS)
```bash
chmod +x build.sh
```

#### Build fails with "C++17 not supported"
- Update your compiler:
  - **GCC**: Version 7 or later
  - **Clang**: Version 6 or later
  - **MSVC**: Visual Studio 2017 or later

#### MinGW linking issues on Windows
```cmd
# Try using static linking
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXE_LINKER_FLAGS="-static"
```

### Platform-Specific Notes

#### Windows
- PowerShell execution policy might block scripts:
  ```powershell
  Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
  ```
- Use PowerShell 5.1+ or PowerShell Core 6+

#### Linux
- Some distributions may need additional packages:
  ```bash
  # For older systems
  sudo apt install libc6-dev
  ```

#### macOS
- Xcode updates may require reinstalling Command Line Tools
- Use Homebrew for easier dependency management

## 📦 Distribution

### Creating Portable Builds

#### Windows (Static Build)
```cmd
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXE_LINKER_FLAGS="-static"
cmake --build build
```

#### Linux (Static Build)
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++"
cmake --build build
```

## 🧪 Testing the Build

Quick verification that the build works:

```bash
# Run with minimal output
./build/NSE_MTBT_Decoder --count 3

# Expected output should show binary decoding details
```

## 📚 Additional Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [C++17 Compiler Support](https://en.cppreference.com/w/cpp/compiler_support/17)
- [MinGW-w64 Installation Guide](https://www.mingw-w64.org/docs/)
- [Visual Studio Documentation](https://docs.microsoft.com/en-us/cpp/)

---

For more information, see the main [README.md](README.md) file.