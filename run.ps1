# NSE MTBT Decoder - Complete Build and Run Script
# This script ensures the project is always built and ready to run

param(
    [string]$Count = "10",
    [switch]$Debug,
    [switch]$CSV,
    [string]$Output = "",
    [switch]$Help,
    [switch]$Force
)

Write-Host "🚀 NSE MTBT Decoder - Build & Run" -ForegroundColor Green
Write-Host "=================================" -ForegroundColor Green

# Function to check if executable exists and is up to date
function Test-ExecutableUpToDate {
    $exePath = "build\NSE_MTBT_Decoder.exe"
    if (-not (Test-Path $exePath)) {
        return $false
    }
    
    $exeTime = (Get-Item $exePath).LastWriteTime
    $sourceFiles = Get-ChildItem "src\*.cpp", "src\*.h" -Recurse
    $newestSource = ($sourceFiles | Sort-Object LastWriteTime -Descending | Select-Object -First 1).LastWriteTime
    
    return $exeTime -gt $newestSource
}

# Setup environment
Write-Host "🔧 Setting up GCC 15.2.0 environment..." -ForegroundColor Yellow
& ".\setup_environment.ps1"

# Check if build is needed
$needsBuild = $Force -or (-not (Test-ExecutableUpToDate))

if ($needsBuild) {
    Write-Host "🔨 Building project..." -ForegroundColor Yellow
    
    # Create build directory if it doesn't exist
    if (-not (Test-Path "build")) {
        New-Item -ItemType Directory -Path "build" | Out-Null
    }
    
    # Build the project
    $buildCommand = "g++ -std=c++17 -Wall -Wextra -O2 -I src src/main.cpp src/MessageTypes.cpp src/Decoder.cpp src/FeedSimulator.cpp src/Utils.cpp -o build/NSE_MTBT_Decoder"
    
    Write-Host "Command: $buildCommand" -ForegroundColor Gray
    Invoke-Expression $buildCommand
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "❌ Build failed!" -ForegroundColor Red
        exit 1
    }
    
    Write-Host "✅ Build successful!" -ForegroundColor Green
} else {
    Write-Host "✅ Executable is up to date, skipping build." -ForegroundColor Green
}

# Build command arguments
$args = @("--count", $Count)
if ($Debug) { $args += "--debug" }
if ($CSV) { $args += "--csv" }
if ($Output) { $args += @("--output", $Output) }
if ($Help) { $args += "--help" }

# Run the executable
Write-Host "🏃 Running NSE MTBT Decoder..." -ForegroundColor Yellow
Write-Host "Command: .\build\NSE_MTBT_Decoder.exe $($args -join ' ')" -ForegroundColor Gray

& ".\build\NSE_MTBT_Decoder.exe" @args

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Execution failed!" -ForegroundColor Red
    exit 1
}

Write-Host "🎉 Completed successfully!" -ForegroundColor Green