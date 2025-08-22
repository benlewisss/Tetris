#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#include "../include/graphics.h"
#include "../include/util.h"
#include "../include/tetromino.h"

bool draw_arena(SDL_Renderer* renderer, const tetromino_identifier arena[ARENA_HEIGHT][ARENA_WIDTH])
{
    const SDL_Color grey = { 32, 32, 32, 255 };
    for (int row = 0; row < ARENA_HEIGHT; row++)
    {
        for (int col = 0; col < ARENA_WIDTH; col++)
        {
            // Draw grid
            SDL_SetRenderDrawColor(renderer, grey.r, grey.g, grey.b, grey.a);
            SDL_FRect rect = { (float)col * BLOCK_SIZE, (float)row * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
            if (SDL_RenderRect(renderer, &rect) == false) return false;

            // Draw filled squares
            if (arena[row][col])
            {
                const SDL_Color square_color = get_tetromino_shape_by_identifier(arena[row][col])->color;
                draw_block(renderer, square_color, col, row);
            }
        }
    }

    return true;
}

bool draw_block(SDL_Renderer* renderer, SDL_Color color, int8_t x, int8_t y)
{
    if (x >= ARENA_WIDTH || x < 0 || y >= ARENA_HEIGHT || y < 0) return false;

    SDL_FRect rect = { (float)x * BLOCK_SIZE, (float)y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    if (SDL_RenderFillRect(renderer, &rect) == false) return false;

    return true;
}

bool draw_tetromino(SDL_Renderer* renderer, TetrominoShape tetromino, int16_t rotation, int8_t x, int8_t y)
{
    // Iterate over the coordinate pairs of each block in the tetromino
    for (int i = 0; i < TETROMINO_SIZE * 2; i += 2)
    {
        if (draw_block(renderer, tetromino.color, x + tetromino.offsets[rotation % 4][i], y + tetromino.offsets[rotation % 4][i + 1]) == false) return false;
    }

    return true;
}