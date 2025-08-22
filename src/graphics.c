#include <SDL3/SDL.h>
#include <stdbool.h>

#include "../include/graphics.h"
#include "../include/util.h"
#include "../include/tetromino.h"

bool DrawArena(SDL_Renderer* renderer, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH])
{
	for (int row = 0; row < ARENA_HEIGHT; row++)
	{
		for (int col = 0; col < ARENA_WIDTH; col++)
		{
			// Draw grid
			SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255); // Grey
			SDL_FRect rect = {(float)col * BLOCK_SIZE, (float)row * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
			if (SDL_RenderRect(renderer, &rect) == false)
				return false;

			// Draw filled squares
			if (arena[row][col])
			{
				const SDL_Color square_color = GetTetrominoShapeByIdentifier(arena[row][col])->color;
				DrawBlock(renderer, square_color, col, row);
			}
		}
	}

	return true;
}

bool DrawBlock(SDL_Renderer* renderer, const SDL_Color color, const int8_t x, const int8_t y)
{
	if (x >= ARENA_WIDTH || x < 0 || y >= ARENA_HEIGHT || y < 0)
		return false;

	const SDL_FRect rect = {(float)x * BLOCK_SIZE, (float)y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};

	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	if (SDL_RenderFillRect(renderer, &rect) == false)
		return false;

	return true;
}

bool DrawDroppingTetromino(SDL_Renderer* renderer, const DroppingTetromino* droppingTetromino)
{
	const SDL_Color droppingTetrominoColor = droppingTetromino->shape.color;
	const int8_t droppingTetrominoX = droppingTetromino->x;
	const int8_t droppingTetrominoY = droppingTetromino->y;
	const uint8_t* droppingTetrominoRotatedOffsets = droppingTetromino->shape.offsets[droppingTetromino->rotation];

	// Iterate over the coordinate pairs of each block in the tetromino
	for (int i = 0; i < TETROMINO_SIZE * 2; i += 2)
	{
		if (DrawBlock(renderer,
		              droppingTetrominoColor,
		              droppingTetrominoX + droppingTetrominoRotatedOffsets[i],
		              droppingTetrominoY + droppingTetrominoRotatedOffsets[i + 1]) == false)
			return false;
	}

	return true;
}
