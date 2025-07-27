@echo off
REM River Raid Enhanced Edition - Windows Build Script
REM This script builds the game using GCC/MinGW

echo Building River Raid Enhanced Edition...

REM Check if GCC is available
gcc --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: GCC not found. Please install MinGW or another GCC compiler.
    echo You can download MinGW from: https://www.mingw-w64.org/
    pause
    exit /b 1
)

REM Check if Raylib is available
echo Checking for Raylib...
gcc -lraylib -o test_raylib test_raylib.c 2>nul
if errorlevel 1 (
    echo WARNING: Raylib not found. Attempting to build without it...
    echo Please install Raylib for full functionality.
    echo Windows installation guide: https://github.com/raysan5/raylib/wiki/Installation-Guide
)

REM Clean previous build
echo Cleaning previous build...
if exist *.o del *.o
if exist RiverRaid.exe del RiverRaid.exe

REM Compile source files
echo Compiling source files...
gcc -c -std=c11 -O2 -Wall -Wextra main.c -o main.o
if errorlevel 1 (
    echo ERROR: Failed to compile main.c
    pause
    exit /b 1
)

gcc -c -std=c11 -O2 -Wall -Wextra game.c -o game.o
if errorlevel 1 (
    echo ERROR: Failed to compile game.c
    pause
    exit /b 1
)

gcc -c -std=c11 -O2 -Wall -Wextra render.c -o render.o
if errorlevel 1 (
    echo ERROR: Failed to compile render.c
    pause
    exit /b 1
)

REM Link the executable
echo Linking executable...
gcc main.o game.o render.o -std=c11 -O2 -Wall -Wextra -lraylib -lopengl32 -lgdi32 -lwinmm -o RiverRaid.exe
if errorlevel 1 (
    echo ERROR: Failed to link executable
    echo This might be due to missing Raylib library.
    pause
    exit /b 1
)

REM Clean up object files
del *.o

echo.
echo Build completed successfully!
echo Run the game with: RiverRaid.exe
echo.
pause 