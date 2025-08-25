cmake_minimum_required(VERSION 3.20)
project(Tetris C)

# Use C11 standard
set(CMAKE_C_STANDARD 11)

# Use vcpkg toolchain if defined
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# Find SDL3 and SDL3_image (with PNG support via vcpkg)
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)

add_executable(Tetris
    src/main.c
    src/game.c
    src/graphics.c
    src/tetromino.c
    include/game.h
    include/graphics.h
    include/tetromino.h
    include/util.h
 )

# Include headers
target_include_directories(Tetris PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link libraries
target_link_libraries(Tetris PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

# Copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

# Provide resource path macro for code
target_compile_definitions(Tetris PRIVATE RESOURCE_PATH="${CMAKE_BINARY_DIR}/resources/")

# Recommended build type for profiling on release
set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)

# Ensure debug information is generated for profiling
if (MSVC)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Zi /Ob0 /Od /RTC1")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /Zi /O2")
    target_link_options(Tetris PRIVATE /DEBUG)
endif()

