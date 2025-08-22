cmake_minimum_required(VERSION 3.20)
project(Tetris C)

# Use C11 standard
set(CMAKE_C_STANDARD 11)

# Let CMake know where vcpkg is
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# Find SDL3 and SDL3_image
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)

add_executable(Tetris "src/main.c" "src/game.c" "src/graphics.c" "src/tetromino.c" "include/game.h" "include/graphics.h" "include/tetromino.h" "include/util.h"  )

# Add include/ directory to search path
target_include_directories(Tetris PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link against SDL3 and SDL3_image
target_link_libraries(Tetris PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)