#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#include "../include/graphics.h"
#include "../include/util.h"
#include "../include/tetromino.h"

bool draw_arena(SDL_Renderer* renderer, SDL_Color color, const uint8_t arena[ARENA_WIDTH][ARENA_HEIGHT])
{
    // Draw grid
    for (int row = 0; row < ARENA_HEIGHT; row++)
    {
        for (int col = 0; col < ARENA_WIDTH; col++)
        {
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_FRect rect = { (float)col * BLOCK_SIZE, (float)row * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
            if (SDL_RenderRect(renderer, &rect) == false) return false;

            // Draw filled squares
            if (arena[col][row])
            {
                //TODO Store colour data in the arena (0-7 representing colours) and grab that here
                const SDL_Color grey = { 50, 50, 50, 255 };
                draw_block(renderer, grey, col, row);
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