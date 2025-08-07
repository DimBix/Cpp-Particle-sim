@echo off
REM Cross-platform build script for Windows

echo === Building Particle Simulation ===

REM Check if vcpkg is available (common way to install GLFW3 on Windows)
where vcpkg >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Warning: vcpkg not found in PATH
    echo Please ensure GLFW3 is installed via:
    echo   vcpkg install glfw3
    echo   vcpkg integrate install
    echo Or download precompiled GLFW3 libraries
)

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with Visual Studio (adjust generator as needed)
echo Configuring...
cmake .. -G "Visual Studio 16 2019" -A x64

REM Build
echo Building...
cmake --build . --config Release

echo Build complete! Executable: .\build\Release\particle_sim.exe
pause
