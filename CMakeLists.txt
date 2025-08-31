cmake_minimum_required(VERSION 3.20)
project(Tetris C)

# Use C11 standard
set(CMAKE_C_STANDARD 11)

# Use vcpkg toolchain if defined
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# Find SDL3 and SDL3 libraries
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

add_executable(Tetris
    src/main.c
    src/game.c
    src/graphics.c
    src/tetromino.c
    include/game.h
    include/graphics.h
    include/tetromino.h
    include/util.h
  "src/util.c")

# Include headers
target_include_directories(Tetris PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link libraries
target_link_libraries(Tetris PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf)

# Copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

# Provide resource path macro for code
target_compile_definitions(Tetris PRIVATE RESOURCE_PATH="${CMAKE_BINARY_DIR}/resources/")

# Enable strict warnings and treat them as errors
#if (MSVC)
#    target_compile_options(Tetris PRIVATE /W4 /WX)
#else()
#    target_compile_options(Tetris PRIVATE -Wall -Wextra -Wpedantic -Werror)
#endif()

# Recommended build type for profiling on release
set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)

