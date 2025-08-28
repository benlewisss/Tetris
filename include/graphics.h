#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL3_ttf/SDL_ttf.h>

#include "util.h"
#include "tetromino.h"

struct
{
    float gridSquareSize; // We devise coordinates for all draw calls using this value, as it changes relative to the window size.
    int sideBarGridWidth;
    SDL_Texture* titleTexture;
    TTF_Font* mainFont;
    TTF_Font* secondaryFont;
} graphicsData;

/**
 * Initialises the graphicsData values.
 *
 * @return True on success, false otherwise.
 */
bool InitGraphicsData(void);

/**
 * Loads resources into memory, including tetromino square textures, fonts, and const text.
 * 
 * @param renderer A pointer to the SDL renderer used to create the GPU texture.
 * @return True on success, false otherwise.
 */
bool LoadResources(SDL_Renderer* renderer);

/**
 * Draw a single block on the grid.
 *
 * @param renderer A pointer to the SDL renderer to draw to.
 * @param texture A pointer to the texture of the block.
 * @param alpha The alpha of the texture of the block.
 * @param blockSize The size of the block.
 * @param x The x coordinate in the grid.
 * @param y The y coordinate in the grid.
 *
 * @returns True on success, false otherwise.
 */
bool DrawBlock(SDL_Renderer* renderer, SDL_Texture* texture, Uint8 alpha, float blockSize, int x, int y);


/**
 * Draw the arena grid.
 *
 * @param renderer A pointer to the SDL renderer to draw to.
 * @param arena The matrix representation of the tetris arena.
 *
 * @returns True on success, false otherwise.
 */
bool DrawArena(SDL_Renderer* renderer, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH]);

/**
 * Draw an entire tetromino on the grid.
 *
 * @param renderer A pointer to the SDL renderer to draw to.
 * @param droppingTetromino A pointer to the dropping tetromino.
 *
 * @returns True on success, false otherwise.
 */
bool DrawDroppingTetromino(SDL_Renderer* renderer, const DroppingTetromino* droppingTetromino);

/**
 * Draw the ghost of an entire tetromino on the grid.
 *
 * @note A ghost is where the dropping tetromino would be placed if the user hard-dropped it at that point.
 * @note See "Ghost Piece": https://tetris.wiki/Tetris_Guideline
 *
 * @param renderer A pointer to the SDL renderer to draw to.
 * @param arena The matrix representation of the tetris arena.
 * @param droppingTetromino A pointer to the dropping tetromino.
 * @return 
 */
bool DrawDroppingTetrominoGhost(SDL_Renderer* renderer, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], const DroppingTetromino* droppingTetromino);

/**
 * Draw the sidebar next to the arena.
 *
 * @param renderer A pointer to the SDL renderer to draw to.
 * @param score The score to draw on the sidebar.
 * @param level The level to draw on the sidebar.
 * @return 
 */
bool DrawSidebar(SDL_Renderer* renderer, int score, int level);

/**
 * Resizes the grid square (used as a standard alignment unit) based on what would fit in the given window
 * size.
 *
 * @param windowWidth The new width of the game window.
 * @param windowHeight The new height of the game window.
 * @return
 */
bool ResizeGridSquares(Sint32 windowWidth, Sint32 windowHeight);

#endif //GRAPHICS_H
