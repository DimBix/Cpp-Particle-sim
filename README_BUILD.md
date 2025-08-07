# C++ Particle Simulation

A real-time particle physics simulation using OpenGL, GLFW, and GLAD.

## Prerequisites

This project requires GLFW3 to be installed on your system.

### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install libglfw3-dev cmake build-essential
```

### Linux (Fedora/CentOS/RHEL)
```bash
sudo dnf install glfw-devel cmake gcc-c++
```

### Linux (Arch)
```bash
sudo pacman -S glfw cmake gcc
```

### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install glfw cmake
```

### Windows

#### Option 1: Using vcpkg (Recommended)
```cmd
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# Install GLFW3
.\vcpkg install glfw3:x64-windows
```

#### Option 2: Manual Installation
1. Download GLFW3 from https://www.glfw.org/download.html
2. Extract to a folder (e.g., `C:\glfw`)
3. Set environment variable `GLFW3_ROOT` to the installation path
4. Ensure CMake can find GLFW3

## Building

### Quick Build (Linux/macOS)
```bash
chmod +x build.sh
./build.sh
```

### Quick Build (Windows)
```cmd
build.bat
```

### Manual Build
```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
make -j4          # Linux/macOS
# OR
cmake --build . --config Release  # Windows
```

## Running

### Linux/macOS
```bash
./build/particle_sim
```

### Windows
```cmd
.\build\Release\particle_sim.exe
```

## Controls

- **Arrow Keys/WASD**: Apply forces to particles
- **ESC**: Exit simulation

## Project Structure

```
├── src/
│   ├── main.cpp          # Main application
│   └── glad.c            # OpenGL loader
├── Dependencies/
│   └── include/
│       └── glad/         # GLAD headers
├── CMakeLists.txt        # Build configuration
├── build.sh             # Unix build script
├── build.bat            # Windows build script
└── README.md            # This file
```

## Troubleshooting

### CMake can't find GLFW3
- **Linux**: Ensure `libglfw3-dev` is installed
- **Windows**: Make sure vcpkg integration is enabled or GLFW3_ROOT is set
- **macOS**: Install via Homebrew: `brew install glfw`

### Compilation errors
- Ensure you have a C++11 compatible compiler
- Check that OpenGL drivers are installed
- Verify all dependencies are correctly installed

### Runtime errors
- Update your graphics drivers
- Ensure your system supports OpenGL 3.3 or higher

## License

See LICENSE.md for details.
