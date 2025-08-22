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
bool draw_arena(SDL_Renderer* renderer, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH]);

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
bool draw_block(SDL_Renderer* renderer, SDL_Color color, int8_t x, int8_t y);

/**
 * Draw an entire tetromino on the grid.
 *
 * \param renderer A pointer to the SDL renderer to draw to.
 * \param tetromino The tetromino object
 * \param x The x coordinate in the grid
 * \param y The y coordinate in the grid
 *
 * \returns True if success, false otherwise.
 */
bool draw_tetromino(SDL_Renderer* renderer, TetrominoShape tetromino, int16_t rotation, int8_t x, int8_t y);

#endif //GRAPHICS_H