# NSE MTBT Decoder - PowerShell build script
# Supports: Windows PowerShell 5.1+ and PowerShell Core 6+

param(
    [ValidateSet("Debug", "Release")]
    [string]$BuildType = "Release",
    
    [switch]$Clean,
    [switch]$Verbose,
    [switch]$Help
)

# Display help
if ($Help) {
    Write-Host @"
🚀 NSE MTBT Decoder - PowerShell Build Script

USAGE:
    .\build.ps1 [-BuildType <Debug|Release>] [-Clean] [-Verbose] [-Help]

PARAMETERS:
    -BuildType    Build configuration (Debug or Release, default: Release)
    -Clean        Clean build directory before building
    -Verbose      Enable verbose output
    -Help         Show this help message

EXAMPLES:
    .\build.ps1                    # Build Release version
    .\build.ps1 -BuildType Debug   # Build Debug version
    .\build.ps1 -Clean             # Clean and build
    .\build.ps1 -Verbose           # Build with verbose output
"@
    exit 0
}

# Set console colors
$Host.UI.RawUI.ForegroundColor = "White"

function Write-ColoredOutput {
    param([string]$Message, [string]$Color = "White")
    
    $originalColor = $Host.UI.RawUI.ForegroundColor
    $Host.UI.RawUI.ForegroundColor = $Color
    Write-Host $Message
    $Host.UI.RawUI.ForegroundColor = $originalColor
}

function Test-Command {
    param([string]$Command)
    
    try {
        Get-Command $Command -ErrorAction Stop | Out-Null
        return $true
    }
    catch {
        return $false
    }
}

# Header
Write-ColoredOutput "`n🚀 NSE MTBT Decoder - PowerShell Build Script" "Cyan"
Write-ColoredOutput "═══════════════════════════════════════════════════════════" "Blue"
Write-ColoredOutput "🔨 Build configuration: $BuildType" "Yellow"
Write-ColoredOutput "💻 PowerShell version: $($PSVersionTable.PSVersion)" "Gray"

# Check for required tools
Write-ColoredOutput "🔍 Checking build dependencies..." "Blue"

if (-not (Test-Command "cmake")) {
    Write-ColoredOutput "❌ Error: CMake is not installed or not in PATH" "Red"
    Write-ColoredOutput "Please install CMake and try again" "Yellow"
    exit 1
}

$cmakeVersion = (cmake --version | Select-Object -First 1) -replace "cmake version ", ""
Write-ColoredOutput "✅ Found CMake: $cmakeVersion" "Green"

# Detect C++ compiler
$compilerFound = $false
$generator = ""

if (Test-Command "g++") {
    $gccVersion = (g++ --version | Select-Object -First 1)
    Write-ColoredOutput "✅ Found GCC compiler: $gccVersion" "Green"
    $generator = "MinGW Makefiles"
    $compilerFound = $true
}
elseif (Test-Command "cl") {
    Write-ColoredOutput "✅ Found Visual Studio compiler" "Green"
    $generator = "Visual Studio 16 2019"
    $compilerFound = $true
}
else {
    # Try to find Visual Studio
    $vs2019Paths = @(
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
    )
    
    foreach ($path in $vs2019Paths) {
        if (Test-Path $path) {
            Write-ColoredOutput "✅ Found Visual Studio 2019" "Green"
            $generator = "Visual Studio 16 2019"
            $compilerFound = $true
            break
        }
    }
}

if (-not $compilerFound) {
    Write-ColoredOutput "❌ Error: No suitable C++ compiler found" "Red"
    Write-ColoredOutput "`nPlease install one of the following:" "Yellow"
    Write-ColoredOutput "  - MinGW-w64 (recommended)" "White"
    Write-ColoredOutput "  - Visual Studio 2019 or later" "White"
    Write-ColoredOutput "  - Visual Studio Build Tools" "White"
    exit 1
}

# Clean build directory if requested
if ($Clean -and (Test-Path "build")) {
    Write-ColoredOutput "🧹 Cleaning build directory..." "Blue"
    Remove-Item -Recurse -Force "build"
}

# Create build directory
Write-ColoredOutput "📁 Creating build directory..." "Blue"
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Name "build" | Out-Null
}

Set-Location "build"

try {
    # Configure with CMake
    Write-ColoredOutput "⚙️  Configuring with CMake..." "Blue"
    
    $cmakeArgs = @(
        "-G", $generator,
        "-DCMAKE_BUILD_TYPE=$BuildType",
        "-DCMAKE_CXX_STANDARD=17",
        ".."
    )
    
    if ($Verbose) {
        & cmake @cmakeArgs
    } else {
        & cmake @cmakeArgs 2>&1 | Out-Null
    }
    
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed"
    }
    
    # Build the project
    Write-ColoredOutput "🔨 Building NSE MTBT Decoder..." "Blue"
    
    $buildArgs = @("--build", ".", "--config", $BuildType)
    if ($Verbose) {
        $buildArgs += "--verbose"
    }
    
    & cmake @buildArgs
    
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }
    
    # Check if build was successful
    $exePaths = @(
        "NSE_MTBT_Decoder.exe",
        "$BuildType\NSE_MTBT_Decoder.exe"
    )
    
    $exeFound = $false
    foreach ($path in $exePaths) {
        if (Test-Path $path) {
            Write-ColoredOutput "✅ Build completed successfully!" "Green"
            Write-ColoredOutput "📦 Executable: build\$path" "Cyan"
            
            $fileInfo = Get-Item $path
            Write-ColoredOutput "📊 Size: $($fileInfo.Length) bytes" "Gray"
            Write-ColoredOutput "🕒 Created: $($fileInfo.CreationTime)" "Gray"
            
            Write-ColoredOutput "`n🚀 To run the decoder:" "Blue"
            Write-ColoredOutput "   .\build\$path --count 10" "Yellow"
            Write-ColoredOutput "   .\build\$path --count 1000 --csv" "Yellow"
            Write-ColoredOutput "   .\build\$path --help" "Yellow"
            
            $exeFound = $true
            break
        }
    }
    
    if (-not $exeFound) {
        throw "Build failed - executable not found"
    }
    
    Write-ColoredOutput "`n🎉 NSE MTBT Decoder build completed successfully!" "Green"
    
} catch {
    Write-ColoredOutput "❌ Error: $_" "Red"
    exit 1
} finally {
    Set-Location ".."
}