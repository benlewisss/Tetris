# Tetris

A modern Tetris clone written in **C** using **SDL3**, fully conforming to the official [Tetris Guidelines](https://tetris.wiki/Tetris_Guideline).

This README serves both as a **usage guide** and a **code walkthrough** for anyone learning SDL.

---

## 1. Building the project

The project uses **CMake** + **vcpkg (in manifest mode)** and depends on:

* `sdl3`
* `sdl3-image` *(With explicit PNG support)*
* `sdl3-ttf`

### Project Structure

```
project-root/
├── cmake/
│   └── WinFixupBundle.cmake.in
├── include/
├── src/
├── resources/
├── CMakeLists.txt
├── CMakePresets.json
├── vcpkg.json
├── vcpkg-configuration.json
```

### Prerequisites

* A C toolchain (MSVC, clang, or GCC)
* CMake ≥ 3.20
* vcpkg installed and `VCPKG_ROOT` set

NOTE: CMake automatically picks up vcpkg if the VCPKG_ROOT environment variable is set.

### Building with CMake Presets

From the project root:

```sh
cmake --preset vcpkg
cmake --build --preset build-debug
cmake --build --preset build-release
cmake --build --preset package-release
```

The resulting Tetris executable appears in the build directory for your chosen preset.

### Running the Game

Run the built executable from its build folder.
Make sure the **resources/** directory (fonts, textures) is available relative to the executable, as the game loads assets from that path.

---

## 2. How to Play

### Keyboard Controls

* **Move left**: `A` or `←`
* **Move right**: `D` or `→`
* **Rotate clockwise**: `W` or `↑`
* **Soft drop**: `S` or `↓`
* **Hard drop**: `SPACE`
* **Pause / resume**: `P`
* **Quit**: `ESC`

---

## 3. Code Architecture Overview

### `main.c` – SDL3 Application

Implements the SDL3 app-callback pattern:

* **`SDL_AppInit`**
  Sets metadata, initializes SDL and fonts, allocates contexts, calls `GAME_Init` and `GFX_Init`.

* **`SDL_AppEvent`**
  Handles window events, mouse input, and all keyboard actions.

* **`SDL_AppIterate`**
  Performs one frame: render → present → update game logic.

* **`SDL_AppQuit`**
  Cleans up everything.

This keeps rendering, input, and game logic cleanly separated.

---

### `game.c` / `game.h` – Game State & Rules

`GameDataContext` holds all authoritative game state.

Key responsibilities:

* Initialization, reset, and arena management
* Gravity and level progression
* Line clearing and row dropping
* Collision detection
* Soft/hard drops and horizontal movement
* Lock down timing logic

Notable helpers:

* **`WillDroppingTetrominoCollide`**: Predicts collisions against walls/stack
* **`ClearLines` / `DropRows`**: Efficient bottom-up scanning with a sliding-window approach
* **`WallKickDroppingTetromino`**: Full SRS implementation including I-piece tables

---

### `tetromino.c` / `tetromino.h` – Shapes, Rotations, RNG

Each tetromino type stores:

* An identifier
* A texture pointer for the square
* Precomputed orientation grids (`coordinates[4][4][4]`)

The 7-bag logic:

* Create a bag `[1..7]` of the different tetromino types, which is then shuffled
* Consume pieces in order; reshuffle when empty

Rotation uses the classic trick:

```c
orientation = (orientation + rotationAmount) & 3;
```

Bitmasking avoids the pitfalls of modulo with negative values.

---

### `graphics.c` / `graphics.h` – Rendering, Layout, UI

Handles everything visual:

* Opening fonts
* Creating sidebar UI and buttons
* Loading textures
* Window sizing based on desktop resolution
* Grid layout system (everything is placed in grid units, converted to pixels)
* Rendering pipeline: arena → falling piece → ghost → sidebar → overlay

Ghost rendering repeatedly tests downward offsets until a collision is detected, then draws the piece with low alpha.

Buttons have hover/pressed states and call the appropriate game callbacks.

Text is cached so SDL_ttf is only invoked when actual text changes.

---

### `util.c` / `util.h` – Assertions & Error Handling

* `Assert()` is a simple wrapper used throughout the code
* `FatalError()` logs errors, shows a message box, shuts down SDL, and exits

This provides uniform, explicit failure handling across the project.

---

## 4. Interesting Implementation Details

A few techniques worth calling out:

1. **Orientation wrapping via bitmask (`& 3`)**
   Clean, branch-free, modulo-4 rotation.

2. **Proper 7-bag with Fisher–Yates**
   Matches Guideline behavior and avoids piece droughts.

3. **Sliding-window line clearing**
   Efficient bottom-up scanning

4. **SRS wall-kick tables baked into code**
   Mirrors the official rotation rules.

5. **Grid-first rendering**
   Layout code never cares about pixel sizes; window resize only changes `gridSquareSize`.

6. **Cached text rendering for HUD**
   Prevents SDL_ttf from doing unnecessary work every frame.

---

## 5. Ideas for Extensions

If you want to expand the project, here are natural next steps:

* **Music** (Is it really tetris without music?)
* **Next-piece preview** (bag already supports it)
* **Hold piece**
* **T-Spin detection**
* **Persistent high scores**
* **Settings menu** with key remapping 

The modules are intentionally decoupled making this a great starting point for learning or building a more fully-featured Tetris clone.
