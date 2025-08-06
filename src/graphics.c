#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "../include/graphics.h"
#include "../include/util.h"
#include "../include/tetromino.h"

bool draw_arena(SDL_Renderer* renderer, SDL_Color color, uint8_t arena[ARENA_WIDTH][ARENA_HEIGHT])
{
    // Draw grid
    for (int j = 0; j < ARENA_HEIGHT; j++)
    {
        for (int i = 0; i < ARENA_WIDTH; i++)
        {
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_FRect rect = { (float)i * BLOCK_SIZE, (float)j * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
            if (SDL_RenderRect(renderer, &rect) == false) return false;

            // Draw filled squares
            if (arena[i][j])
            {
                //TODO Store colour data in the arena (0-7 representing colours) and grab that here
                const SDL_Color grey = { 50, 50, 50, 255 };
                draw_block(renderer, grey, i, j);
            }
        }
    }

    return true;
}

bool draw_block(SDL_Renderer* renderer, SDL_Color color, uint8_t x, uint8_t y)
{
    SDL_FRect rect = { (float)x * BLOCK_SIZE, (float)y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    if (SDL_RenderFillRect(renderer, &rect) == false) return false;

    return true;
}

bool draw_tetromino(SDL_Renderer* renderer, TetrominoShape tetromino, uint8_t x, uint8_t y)
{
    // Iterate over the coordinate pairs of each block in the tetromino
    for (int i = 0; i < TETROMINO_SIZE * 2; i += 2)
    {
        if (draw_block(renderer, tetromino.color, x + tetromino.offsets[i], y + tetromino.offsets[i + 1]) == false) return false;
    }

    return true;
}