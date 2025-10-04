@echo off
REM Windows build script for NSE MTBT Decoder

echo Building NSE MTBT Decoder...

REM Check if modern g++ is available (GCC 15.2.0)
set "MODERN_GCC=C:\Users\harsh\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\g++.exe"

if exist "%MODERN_GCC%" (
    echo Using modern GCC 15.2.0 for C++17...
    set "COMPILER=%MODERN_GCC%"
) else (
    REM Fall back to system g++
    where g++ >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo Error: g++ not found. Please install GCC or MinGW-w64
        echo Recommended: winget install --id=MSYS2.MSYS2
        pause
        exit /b 1
    )
    echo Using system g++...
    set "COMPILER=g++"
)

REM Clean previous build
if exist NSE_MTBT_Decoder.exe del NSE_MTBT_Decoder.exe

REM Compile with C++17
echo Compiling with C++17...
"%COMPILER%" -std=c++17 -Wall -Wextra -O2 -I src src/*.cpp -o NSE_MTBT_Decoder.exe

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build successful!
echo.
echo Usage: NSE_MTBT_Decoder.exe --help
echo Example: NSE_MTBT_Decoder.exe --count 1000 --csv --output trades.csv
echo.
pause