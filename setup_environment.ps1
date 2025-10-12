# Setup WinLibs GCC 15.2.0 Environment
# This script ensures GCC 15.2.0 is used instead of any older versions

Write-Host "🔧 Setting up WinLibs GCC 15.2.0 environment..." -ForegroundColor Green

# WinLibs GCC 15.2.0 path
$winlibsPath = "C:\Users\$env:USERNAME\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.MSVCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin"

# Remove any old MinGW paths from current session
$env:PATH = ($env:PATH -split ';' | Where-Object { $_ -notlike "*MinGW*" -and $_ -ne "C:\MinGW\bin" }) -join ';'

# Add WinLibs GCC to the front of PATH
$env:PATH = "$winlibsPath;$env:PATH"

# Verify setup
Write-Host "✅ Environment configured!" -ForegroundColor Green
Write-Host "Current GCC version:" -ForegroundColor Yellow
gcc --version

# Update user PATH permanently (requires restart)
try {
    $userPath = [Environment]::GetEnvironmentVariable("PATH", "User")
    $cleanPath = ($userPath -split ';' | Where-Object { $_ -notlike "*MinGW*" -and $_ -ne "C:\MinGW\bin" }) -join ';'
    if ($cleanPath -notlike "*$winlibsPath*") {
        $newPath = "$winlibsPath;$cleanPath"
        [Environment]::SetEnvironmentVariable("PATH", $newPath, "User")
        Write-Host "✅ User PATH updated permanently. Restart shell to apply." -ForegroundColor Green
    }
} catch {
    Write-Host "⚠️  Could not update permanent PATH. You may need to run as administrator." -ForegroundColor Yellow
}

Write-Host "🚀 Ready to build and run NSE MTBT Decoder with GCC 15.2.0!" -ForegroundColor Green