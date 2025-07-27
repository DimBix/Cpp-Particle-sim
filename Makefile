# Makefile for compiling a C++ program with GLFW3

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -I$(INCLUDE_DIR) -std=c++11 -Wall -Wextra -O3

# Source files
SRC = $(SRC_DIR)/*.cpp $(SRC_DIR)/*.c

# Output executable
TARGET = $(BIN_DIR)/gravity_sim

ifeq ($(OS),Windows_NT)
	LIBS = -glfw3
	LDFLAGS = -lm -lGL -lX11 -lpthread -lXrandr -lXi -ldl -L$(LIBS_DIR) $(LIBS)
else
	LDFLAGS = -lglfw -lm -lGL -lX11 -lpthread -lXrandr -lXi -ldl
endif

# -------- Directories --------

BIN_DIR = bin
SRC_DIR = src
INCLUDE_DIR = Dependencies/include
LIBS_DIR = Dependencies/libs

# -------- Directories --------

# Build target
all: create_bin_dir $(TARGET)

create_bin_dir:
	mkdir -p $(BIN_DIR)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean target
clean:
	rm -f $(TARGET)
