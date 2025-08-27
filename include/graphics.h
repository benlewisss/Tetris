#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "util.h"
#include "tetromino.h"


#include <SDL3_ttf/SDL_ttf.h>

struct
{
    float gridSquareSize; // We devise coordinates for all draw calls using this value, as it changes relative to the window size.
    int sideBarGridWidth;
    SDL_Texture* titleTexture;
    TTF_Font* mainFont;
    TTF_Font* secondaryFont;
} graphicsData;

bool InitGraphicsData();

bool ResizeGridSquares(Sint32 windowWidth, Sint32 windowHeight);

/** Loads resources
 *
 * TEXTURES:
 * Loads image resources into textures which are assigned to their respective tetrominoes.
 * 
 * @param renderer A pointer to the SDL renderer used to create the GPU texture.
 * @return True if success, false otherwise.
 */
bool LoadResources(SDL_Renderer* renderer);

/**
 * Draw the arena grid.
 *
 * @param renderer A pointer to the SDL renderer to draw to.
 * @param arena The matrix representation of the tetris arena.
 *
 * @returns True if success, false otherwise.
 */
bool DrawArena(SDL_Renderer* renderer, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH]);

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
 * @returns True if success, false otherwise.
 */
bool DrawBlock(SDL_Renderer* renderer, SDL_Texture* texture, Uint8 alpha, float blockSize, int x, int y);

/**
 * Draw an entire tetromino on the grid.
 *
 * @param renderer A pointer to the SDL renderer to draw to.
 * @param droppingTetromino A pointer to the dropping tetromino
 *
 * @returns True if success, false otherwise.
 */
bool DrawDroppingTetromino(SDL_Renderer* renderer, const DroppingTetromino* droppingTetromino);

bool DrawDroppingTetrominoGhost(SDL_Renderer* renderer, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH],
                                const DroppingTetromino* droppingTetromino);

bool DrawSideBar(SDL_Renderer* renderer, int score, int level);

#endif //GRAPHICS_H
