# River Raid Enhanced Edition - PowerShell Build Script
# This script builds the game using GCC/MinGW on Windows

Write-Host "Building River Raid Enhanced Edition..." -ForegroundColor Green

# Check if GCC is available
try {
    $gccVersion = gcc --version 2>$null
    if ($LASTEXITCODE -ne 0) {
        throw "GCC not found"
    }
    Write-Host "GCC found: $($gccVersion[0])" -ForegroundColor Green
} catch {
    Write-Host "ERROR: GCC not found. Please install MinGW or another GCC compiler." -ForegroundColor Red
    Write-Host "You can download MinGW from: https://www.mingw-w64.org/" -ForegroundColor Yellow
    Write-Host "Or use MSYS2: https://www.msys2.org/" -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}

# Check if Raylib is available
Write-Host "Checking for Raylib..." -ForegroundColor Yellow
try {
    gcc -lraylib -o test_raylib test_raylib.c 2>$null
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Raylib found!" -ForegroundColor Green
        Remove-Item test_raylib.exe -ErrorAction SilentlyContinue
    } else {
        throw "Raylib not found"
    }
} catch {
    Write-Host "WARNING: Raylib not found. Attempting to build without it..." -ForegroundColor Yellow
    Write-Host "Please install Raylib for full functionality." -ForegroundColor Yellow
    Write-Host "Windows installation guide: https://github.com/raysan5/raylib/wiki/Installation-Guide" -ForegroundColor Cyan
}

# Clean previous build
Write-Host "Cleaning previous build..." -ForegroundColor Yellow
Remove-Item *.o -ErrorAction SilentlyContinue
Remove-Item RiverRaid.exe -ErrorAction SilentlyContinue

# Compile source files
Write-Host "Compiling source files..." -ForegroundColor Yellow

$compileArgs = @("-c", "-std=c11", "-O2", "-Wall", "-Wextra")

# Compile main.c
Write-Host "  Compiling main.c..." -ForegroundColor Gray
gcc @compileArgs main.c -o main.o
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Failed to compile main.c" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Compile game.c
Write-Host "  Compiling game.c..." -ForegroundColor Gray
gcc @compileArgs game.c -o game.o
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Failed to compile game.c" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Compile render.c
Write-Host "  Compiling render.c..." -ForegroundColor Gray
gcc @compileArgs render.c -o render.o
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Failed to compile render.c" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Link the executable
Write-Host "Linking executable..." -ForegroundColor Yellow
$linkArgs = @("main.o", "game.o", "render.o", "-std=c11", "-O2", "-Wall", "-Wextra", "-lraylib", "-lopengl32", "-lgdi32", "-lwinmm", "-o", "RiverRaid.exe")
gcc @linkArgs
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Failed to link executable" -ForegroundColor Red
    Write-Host "This might be due to missing Raylib library." -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}

# Clean up object files
Remove-Item *.o -ErrorAction SilentlyContinue

Write-Host ""
Write-Host "Build completed successfully!" -ForegroundColor Green
Write-Host "Run the game with: .\RiverRaid.exe" -ForegroundColor Cyan
Write-Host ""

# Check if executable was created
if (Test-Path "RiverRaid.exe") {
    $fileSize = (Get-Item "RiverRaid.exe").Length
    Write-Host "Executable size: $([math]::Round($fileSize / 1KB, 2)) KB" -ForegroundColor Gray
}

Read-Host "Press Enter to exit" 