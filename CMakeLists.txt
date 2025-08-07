cmake_minimum_required(VERSION 3.10)
project(Particle_sim LANGUAGES C CXX)

# =================== PERFORMANCE OPTIMIZATION ===================
# Set default build type to Release for maximum performance
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# High-performance compiler flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Additional performance optimizations
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s") # Strip symbols

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(EXE particle_sim)
set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/include)

add_executable(${EXE} ${SOURCE_DIR}/main.cpp ${SOURCE_DIR}/glad.c)

if(WIN32)
    target_link_libraries(${EXE} glfw3 opengl32)
else()
    target_link_libraries(${EXE} glfw GL X11 pthread Xrandr Xi dl m)
endif()

target_include_directories(${EXE} PRIVATE ${INCLUDE_DIRS})


