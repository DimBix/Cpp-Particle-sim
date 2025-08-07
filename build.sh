#!/bin/bash
# Cross-platform build script for Unix-like systems (Linux, macOS)

set -e  # Exit on any error

echo "=== Building Particle Simulation ==="

# Check if GLFW3 is installed
if ! pkg-config --exists glfw3; then
    echo "Error: GLFW3 not found!"
    echo "Please install GLFW3:"
    echo "  Ubuntu/Debian: sudo apt install libglfw3-dev"
    echo "  Fedora/RHEL:   sudo dnf install glfw-devel"
    echo "  Arch:          sudo pacman -S glfw"
    echo "  macOS:         brew install glfw"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure and build
echo "Configuring for Release build (optimized performance)..."
cmake -DCMAKE_BUILD_TYPE=Release ..

echo "Building with maximum optimization..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "Build complete! Executable: ./build/particle_sim"
