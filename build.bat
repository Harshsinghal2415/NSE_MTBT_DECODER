@echo off
REM NSE MTBT Decoder - Windows build script

setlocal

echo.
echo NSE MTBT Decoder - Windows Build Script
echo ===================================================

REM Set build type
set "BUILD_TYPE=%1"
if "%BUILD_TYPE%"=="" set "BUILD_TYPE=Release"

echo Build configuration: %BUILD_TYPE%

REM Check for CMake
where cmake >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: CMake not found
    exit /b 1
)

REM Check for compiler
where g++ >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo Found MinGW GCC compiler
    set "GENERATOR=MinGW Makefiles"
    goto :build_setup
)

where cl >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo Found Visual Studio compiler
    set "GENERATOR=Visual Studio 16 2019"
    goto :build_setup
)

echo Error: No C++ compiler found
exit /b 1

:build_setup
echo Creating build directory...
if not exist build mkdir build
cd build

echo Configuring with CMake...
cmake -G "%GENERATOR%" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..
if %ERRORLEVEL% neq 0 exit /b 1

echo Building...
cmake --build . --config %BUILD_TYPE%
if %ERRORLEVEL% neq 0 exit /b 1

if exist "NSE_MTBT_Decoder.exe" (
    echo Build completed successfully!
    echo Executable: build\NSE_MTBT_Decoder.exe
) else if exist "%BUILD_TYPE%\NSE_MTBT_Decoder.exe" (
    echo Build completed successfully!
    echo Executable: build\%BUILD_TYPE%\NSE_MTBT_Decoder.exe
) else (
    echo Build failed
    exit /b 1
)
