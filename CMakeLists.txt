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

# --- Copy resources at build time ---
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:Tetris>/resources
    COMMENT "Copying resources folder to output directory"
)
add_dependencies(Tetris copy_resources)

# --- Provide resource path macro for code ---
target_compile_definitions(Tetris PRIVATE RESOURCE_PATH="$<TARGET_FILE_DIR:Tetris>/resources/")

# --- Installation rules ---
install(TARGETS Tetris RUNTIME DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION bin)
install(FILES "${CMAKE_SOURCE_DIR}/LICENSE.txt" "${CMAKE_SOURCE_DIR}/README.md" DESTINATION ".")

# --- Windows: bundle DLLs in Release ---
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    include(BundleUtilities)
    install(CODE "
        include(BundleUtilities)

        # Installed executable
        set(EXECUTABLE \"\${CMAKE_INSTALL_PREFIX}/bin/Tetris${CMAKE_EXECUTABLE_SUFFIX}\")

        # Directories to search for DLLs
        set(DIRS_TO_SEARCH
            \"$<TARGET_FILE_DIR:SDL3>\"
            \"$<TARGET_FILE_DIR:SDL3_image>\"
            \"$<TARGET_FILE_DIR:SDL3_ttf>\"
        )

        # Copy DLLs next to executable
        fixup_bundle(\"\${EXECUTABLE}\" \"\" \"\${DIRS_TO_SEARCH}\")
    " COMPONENT Runtime)
endif()

# --- macOS: create .app bundle ---
if(APPLE)
    set(MACOS_BUNDLE_NAME "Tetris.app")
    set_target_properties(Tetris PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist"
    )
    install(TARGETS Tetris BUNDLE DESTINATION bin)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION bin/${MACOS_BUNDLE_NAME}/Contents/Resources)
endif()

# --- Linux: ensure resources installed alongside executable ---
if(UNIX AND NOT APPLE)
    # Already handled by install(RUNTIME DESTINATION bin) and install(DIRECTORY ...)
endif()

# --- CPack configuration ---
set(CPACK_PACKAGE_NAME "Tetris")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "Ben Lewis")
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
include(CPack)
