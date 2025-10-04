@echo off
setlocal enabledelayedexpansion
REM Run script for NSE MTBT Decoder - automatically sets up MinGW environment

echo 🚀 NSE MTBT Decoder Runner
echo ═══════════════════════════════════

REM Check if executable exists
if not exist "NSE_MTBT_Decoder.exe" (
    echo ❌ NSE_MTBT_Decoder.exe not found!
    echo Please run build.bat first to compile the project.
    echo.
    pause
    exit /b 1
)

REM Find MinGW installation and add to PATH
set "MINGW_PATH="

REM Try WinLibs in user profile first
for /f "delims=" %%i in ('dir /b /s "%USERPROFILE%\AppData\Local\Microsoft\WinGet\Packages" 2^>nul ^| findstr /i winlibs 2^>nul') do (
    if exist "%%i\mingw64\bin\libgcc_s_seh-1.dll" (
        set "MINGW_PATH=%%i\mingw64\bin"
        echo Found MinGW runtime: %%i\mingw64\bin
        goto :mingw_found
    )
)

REM Try other common locations
if exist "C:\winlibs\mingw64\bin\libgcc_s_seh-1.dll" (
    set "MINGW_PATH=C:\winlibs\mingw64\bin"
    goto :mingw_found
)

if exist "C:\mingw64\bin\libgcc_s_seh-1.dll" (
    set "MINGW_PATH=C:\mingw64\bin"
    goto :mingw_found
)

if exist "C:\msys64\mingw64\bin\libgcc_s_seh-1.dll" (
    set "MINGW_PATH=C:\msys64\mingw64\bin"
    goto :mingw_found
)

echo ⚠️  MinGW runtime not found in PATH. Executable may fail to run.
goto :run_executable

:mingw_found
set "PATH=!MINGW_PATH!;%PATH%"
echo ✅ Added MinGW to PATH

:run_executable
echo.
echo Running: NSE_MTBT_Decoder.exe %*
echo ═══════════════════════════════════
echo.

REM Run the executable with all passed arguments
NSE_MTBT_Decoder.exe %*

echo.
echo ═══════════════════════════════════
echo Execution completed.
echo.
pause