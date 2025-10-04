@echo off
REM Windows build script for NSE MTBT Decoder

echo Building NSE MTBT Decoder...

REM Check for modern GCC in common locations
set "COMPILER="

REM Try WinLibs in user profile first
for /f "delims=" %%i in ('dir /b /s "%USERPROFILE%\AppData\Local\Microsoft\WinGet\Packages" 2^>nul ^| findstr /i winlibs 2^>nul') do (
    if exist "%%i\mingw64\bin\g++.exe" (
        set "COMPILER=%%i\mingw64\bin\g++.exe"
        echo Found WinLibs GCC: %%i\mingw64\bin\g++.exe
        goto :compiler_found
    )
)

REM Try other common locations
if exist "C:\winlibs\mingw64\bin\g++.exe" (
    set "COMPILER=C:\winlibs\mingw64\bin\g++.exe"
    echo Found GCC: C:\winlibs\mingw64\bin\g++.exe
    goto :compiler_found
)

if exist "C:\mingw64\bin\g++.exe" (
    set "COMPILER=C:\mingw64\bin\g++.exe"
    echo Found GCC: C:\mingw64\bin\g++.exe
    goto :compiler_found
)

if exist "C:\msys64\mingw64\bin\g++.exe" (
    set "COMPILER=C:\msys64\mingw64\bin\g++.exe"
    echo Found GCC: C:\msys64\mingw64\bin\g++.exe
    goto :compiler_found
)

REM Fall back to system g++
where g++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    set "COMPILER=g++"
    echo Using system g++ (may not support all C++17 features)...
) else (
    echo Error: No C++ compiler found!
    echo.
    echo Please install one of the following:
    echo   1. WinLibs: winget install --id=BrechtSanders.WinLibs.POSIX.UCRT
    echo   2. MSYS2: winget install --id=MSYS2.MSYS2
    echo   3. MinGW-w64: https://www.mingw-w64.org/
    echo.
    pause
    exit /b 1
)

:compiler_found

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