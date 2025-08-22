#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>

#include "graphics.h"
#include "util.h"
#include "tetromino.h"

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
	// TODO Draw rendered image instead of drawing a square, it's much more performant - THIS NEEDS THE SDL_IMAGE LIBRARY. We need to figure out how to include this using cmake.
	// TODO Instead of storing an RGB value in the tetromino structs, we can store a pointer to a surface that is generated from the images which we will load during init()
	if (x >= ARENA_WIDTH || x < 0 || y >= ARENA_HEIGHT || y < 0)
		return false;

	const SDL_FRect rect = {(float)x * BLOCK_SIZE, (float)y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	if (SDL_RenderFillRect(renderer, &rect) == false)
		return false;


	// Draw corners
	const SDL_FRect topRect = { (float)x * BLOCK_SIZE, (float)y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE / 5 };
	const SDL_FRect bottomRect = { (float)x * BLOCK_SIZE, ((float)y * BLOCK_SIZE) + ((BLOCK_SIZE / 5) * 4), BLOCK_SIZE, BLOCK_SIZE / 5 };
	const SDL_FRect leftRect = { (float)x * BLOCK_SIZE, (float)y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE / 5 };
	const SDL_FRect rightRect = { (float)x * BLOCK_SIZE, (float)y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE / 5 };

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 255, 128, 128, 50);
	if (SDL_RenderFillRect(renderer, &topRect) == false)
		return false;
	if (SDL_RenderFillRect(renderer, &bottomRect) == false)
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
