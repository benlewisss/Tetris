cmake_minimum_required(VERSION 3.20)
project(Tetris C)

# --- C standard ---
set(CMAKE_C_STANDARD 11)

# --- vcpkg toolchain ---
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# --- Find dependencies ---
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

# --- Executable ---
add_executable(Tetris
    src/main.c
    src/game.c
    src/graphics.c
    src/tetromino.c
    src/util.c
    include/game.h
    include/graphics.h
    include/tetromino.h
    include/util.h
)

# --- Include directories ---
target_include_directories(Tetris PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- Link libraries ---
target_link_libraries(Tetris PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
)

# --- Resource path macro (relative; works in build + install trees) ---
target_compile_definitions(Tetris PRIVATE RESOURCE_PATH="resources/")

# --- Copy resources next to the built exe for local runs (build tree) ---
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:Tetris>/resources
    COMMENT "Copying resources folder to output directory"
)
add_dependencies(Tetris copy_resources)

# --- Installation rules ---
# Put exe/.app at the package root; resources under resources/
install(TARGETS Tetris
    RUNTIME DESTINATION .
    BUNDLE  DESTINATION .
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION resources)
install(FILES "${CMAKE_SOURCE_DIR}/LICENSE.txt" "${CMAKE_SOURCE_DIR}/README.md" DESTINATION .)

# --- macOS bundle specifics (optional Info.plist) ---
if(APPLE)
    set_target_properties(Tetris PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist"
    )
    # Place .app at root already via BUNDLE DESTINATION .
    # Place resources inside the app for runtime if you prefer:
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION Tetris.app/Contents/Resources)
endif()

# --- Windows: bundle DLLs in Release using BundleUtilities ---
if(WIN32)
    # These variables come from vcpkg toolchain; keep them empty-safe
    set(_VCPKG_INSTALLED_DIR "${VCPKG_INSTALLED_DIR}")
    set(_VCPKG_TARGET_TRIPLET "${VCPKG_TARGET_TRIPLET}")

    # Generate a post-install fixup script with concrete paths
    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/WinFixupBundle.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/WinFixupBundle.cmake
        @ONLY
    )

    # Only run the fixup for Release zips (prevents Debug CRT surprises)
    install(CODE "
        if(\"\${CMAKE_INSTALL_CONFIG_NAME}\" STREQUAL \"Release\")
            message(STATUS \"Running BundleUtilities fixup (Release)...\")
            include(\"${CMAKE_CURRENT_BINARY_DIR}/WinFixupBundle.cmake\")
        else()
            message(STATUS \"Skipping BundleUtilities fixup (config=\${CMAKE_INSTALL_CONFIG_NAME})\")
        endif()
    " COMPONENT Runtime)
endif()

# --- CPack configuration ---
set(CPACK_PACKAGE_NAME "Tetris")
set(CPACK_PACKAGE_VERSION "1.1.0")
set(CPACK_PACKAGE_VENDOR "Ben Lewis")

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

include(CPack)
