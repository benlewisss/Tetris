#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>

#include "graphics.h"
#include "util.h"
#include "tetromino.h"

bool LoadResources(SDL_Renderer* renderer)
{
	// Load tetromino textures
	if (!(GetTetrominoShapeByIdentifier(I)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/orange.png"))) return false;
	if (!(GetTetrominoShapeByIdentifier(O)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/red.png"))) return false;
	if (!(GetTetrominoShapeByIdentifier(T)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/yellow.png"))) return false;
	if (!(GetTetrominoShapeByIdentifier(Z)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/green.png"))) return false;
	if (!(GetTetrominoShapeByIdentifier(S)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/cyan.png"))) return false;
	if (!(GetTetrominoShapeByIdentifier(L)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/blue.png"))) return false;
	if (!(GetTetrominoShapeByIdentifier(J)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/purple.png"))) return false;

	return true;
}

bool DrawArena(SDL_Renderer* renderer, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH])
{
	for (int row = 0; row < ARENA_HEIGHT; row++)
	{
		for (int col = 0; col < ARENA_WIDTH; col++)
		{
			// Draw filled blocks
			if (arena[row][col])
			{
				SDL_Texture* blockTexture = GetTetrominoShapeByIdentifier(arena[row][col])->texture;
				DrawBlock(renderer, blockTexture, col, row);
			}

			// Draw grid (This is the most performance hungry operation, can optimise by drawing images instead).
			SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255); // Grey
			SDL_FRect rect = {(float)col * BLOCK_SIZE, (float)row * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
			if (SDL_RenderRect(renderer, &rect) == false)
				return false;

			
		}
	}

	return true;
}

bool DrawBlock(SDL_Renderer* renderer, SDL_Texture* texture, const int x, const int y)
{
	// TODO Draw rendered image instead of drawing a square, it's much more performant - THIS NEEDS THE SDL_IMAGE LIBRARY. We need to figure out how to include this using cmake.
	// TODO Instead of storing an RGB value in the tetromino structs, we can store a pointer to a surface that is generated from the images which we will load during init()
	if (x >= ARENA_WIDTH || x < 0 || y >= ARENA_HEIGHT || y < 0)
		return false;

	const SDL_FRect rect = {(float)x * BLOCK_SIZE, (float)y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};

	SDL_RenderTexture(renderer, texture, NULL, &rect);

	return true;
}

bool DrawDroppingTetromino(SDL_Renderer* renderer, const DroppingTetromino* droppingTetromino)
{
	SDL_Texture* droppingTetrominoTexture = droppingTetromino->shape.texture;
	const int droppingTetrominoX = droppingTetromino->x;
	const int droppingTetrominoY = droppingTetromino->y;
	const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape.coordinates[droppingTetromino->rotation];

	// Iterate over the coordinate pairs of each block in the tetromino
	for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
	{
		for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
		{
			if (droppingTetrominoRotatedCoordinates[i][j] == false) continue;

			if (DrawBlock(renderer,
				droppingTetrominoTexture,
				droppingTetrominoX + j,
				droppingTetrominoY + i) == false)
				return false;
		}
	}

	return true;
}
