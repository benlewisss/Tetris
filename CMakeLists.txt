cmake_minimum_required(VERSION 3.20)
project(Tetris C)

# Use C11 standard
set(CMAKE_C_STANDARD 11)

# Let CMake know where vcpkg is
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# Find SDL3
find_package(SDL3 REQUIRED CONFIG)

add_executable(Tetris src/main.c "src/graphics.c" "src/tetromino.c" "include/tetromino.h" "include/util.h" "src/game.c" "include/game.h")

target_link_libraries(Tetris PRIVATE SDL3::SDL3)
