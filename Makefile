# Makefile for compiling a C++ program with GLFW3

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -O3

# Linker flags
LDFLAGS = -lglfw -lm

# Source files
SRC = main.cpp

# Output executable
TARGET = $(BIN_DIR)/solar_system

# Output directory
BIN_DIR = bin

# Build target
all: create_bin_dir $(TARGET)

create_bin_dir:
	mkdir -p $(BIN_DIR)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean target
clean:
	rm -f $(TARGET)
