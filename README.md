# Packaging Tetris

How to build, install, and package the **Tetris** project into a ZIP archive.

---

## Project layout
```sh
project-root/
├── cmake/
│ └── WinFixupBundle.cmake.in # Windows DLL bundling script template
├── include/ # Public headers
├── src/ # Source files
├── resources/ # Fonts, images, etc.
├── CMakeLists.txt
├── CMakePresets.json
├── vcpkg.json
├── vcpkg-configuration.json
```

---

## 1. Dependencies

This project uses [vcpkg](https://github.com/microsoft/vcpkg) in **manifest mode** to handle dependencies.

Required libraries (declared in `vcpkg.json`):

- `sdl3`
- `sdl3-image` (with PNG support)
- `sdl3-ttf`

NOTE: CMake automatically picks up vcpkg if the `VCPKG_ROOT` environment variable is set.

---

## 2. Build presets

CMake presets are provided for convenience:

- `build-debug` – build Debug
- `build-release` – build Release
- `package-release` – build Release and run CPack to generate a ZIP

Example:

```sh
# Configure
cmake --preset vcpkg

# Build Debug
cmake --build --preset build-debug

# Build Release
cmake --build --preset build-release

# Build and package Release (ZIP)
cmake --build --preset package-release
```