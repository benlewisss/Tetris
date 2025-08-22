#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

#include "util.h"
#include "../include/tetromino.h"

/**
 * Draw the arena grid.
 *
 * \param renderer A pointer to the SDL renderer to draw to.
 * \param arena A pointer to a matrix representation of the tetris arena.
 *
 * \returns True if success, false otherwise.
 */
bool DrawArena(SDL_Renderer* renderer, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH]);

/**
 * Draw a single block on the grid.
 *
 * \param renderer A pointer to the SDL renderer to draw to.
 * \param color The color of the block
 * \param x The x coordinate in the grid
 * \param y The y coordinate in the grid
 *
 * \returns True if success, false otherwise.
 */
bool DrawBlock(SDL_Renderer* renderer, const SDL_Color color, const int8_t x, const int8_t y);

/**
 * Draw an entire tetromino on the grid.
 *
 * \param renderer A pointer to the SDL renderer to draw to.
 * \param droppingTetromino A pointer to the dropping tetromino
 *
 * \returns True if success, false otherwise.
 */
bool DrawDroppingTetromino(SDL_Renderer* renderer, const DroppingTetromino* droppingTetromino);

#endif //GRAPHICS_H