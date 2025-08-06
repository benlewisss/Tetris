#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

#include "util.h"
#include "../include/tetromino.h"

/**
 * Initialise the SDL renderer, window and Tetris graphics
 *
 * @param window A pointer to the SDL window
 * @param renderer A pointer to the SDL renderer
 *
 * @return True if success, false otherwise.
 */
bool init_renderer(SDL_Window* window, SDL_Renderer* renderer);

/**
 * Draw the arena grid.
 *
 * \param renderer A pointer to the SDL renderer to draw to.
 * \param grid_color The color of the grid.
 * \param arena A pointer to a matrix representation of the tetris arena.
 *
 * \returns True if success, false otherwise.
 */
bool draw_arena(SDL_Renderer* renderer, SDL_Color grid_color, uint8_t arena[ARENA_WIDTH][ARENA_HEIGHT]);

/**
 * Draw a single block on the grid.
 *
 * \param renderer A pointer to the SDL renderer to draw to.
 * \param color The color of the block
 *
 * \returns True if success, false otherwise.
 */
bool draw_block(SDL_Renderer* renderer, SDL_Color color, uint8_t x, uint8_t y);

/**
 * Draw an entire tetromino on the grid.
 *
 * \param renderer A pointer to the SDL renderer to draw to.
 * \param tetromino The shape of the tetromino
 * \param x The x coordinate in the grid
 * \param y The y coordinate in the grid
 *
 * \returns True if success, false otherwise.
 */
bool draw_tetromino(SDL_Renderer* renderer, TetrominoShape tetromino, uint8_t x, uint8_t y);

#endif //GRAPHICS_H