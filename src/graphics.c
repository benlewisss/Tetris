#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>

#include "graphics.h"

#include <stdio.h>

#include "util.h"
#include "tetromino.h"

bool LoadResources(SDL_Renderer* renderer)
{
	char path[512];

	// Load tetromino textures
	//if (snprintf(path, sizeof(path), "%simages/squares/orange.png", RESOURCE_PATH) < 0) return false;
	if (!(GetTetrominoShapeByIdentifier(I)->texture = IMG_LoadTexture(renderer, "resources/images/squares/orange.png"))) return false;

	if (snprintf(path, sizeof(path), "%simages/squares/red.png", RESOURCE_PATH) < 0) return false;
	if (!(GetTetrominoShapeByIdentifier(O)->texture = IMG_LoadTexture(renderer, path))) return false;

	if (snprintf(path, sizeof(path), "%simages/squares/yellow.png", RESOURCE_PATH) < 0) return false;
	if (!(GetTetrominoShapeByIdentifier(T)->texture = IMG_LoadTexture(renderer, path))) return false;

	if (snprintf(path, sizeof(path), "%simages/squares/green.png", RESOURCE_PATH) < 0) return false;
	if (!(GetTetrominoShapeByIdentifier(Z)->texture = IMG_LoadTexture(renderer, path))) return false;

	if (snprintf(path, sizeof(path), "%simages/squares/cyan.png", RESOURCE_PATH) < 0) return false;
	if (!(GetTetrominoShapeByIdentifier(S)->texture = IMG_LoadTexture(renderer, path))) return false;

	if (snprintf(path, sizeof(path), "%simages/squares/blue.png", RESOURCE_PATH) < 0) return false;
	if (!(GetTetrominoShapeByIdentifier(L)->texture = IMG_LoadTexture(renderer, path))) return false;

	if (snprintf(path, sizeof(path), "%simages/squares/purple.png", RESOURCE_PATH) < 0) return false;
	if (!(GetTetrominoShapeByIdentifier(J)->texture = IMG_LoadTexture(renderer, path))) return false;

	return true;
}

bool DrawArena(SDL_Renderer* renderer, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH])
{
	for (int row = 0; row < ARENA_HEIGHT; row++)
	{
		for (int col = 0; col < ARENA_WIDTH; col++)
		{
			// Draw filled squares
			if (arena[row][col])
			{
				SDL_Texture* squareTexture = GetTetrominoShapeByIdentifier(arena[row][col])->texture;
				DrawBlock(renderer, squareTexture, col, row);
			}

			// Draw grid
			SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255); // Grey
			SDL_FRect rect = {(float)col * BLOCK_SIZE, (float)row * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
			if (SDL_RenderRect(renderer, &rect) == false)
				return false;

			
		}
	}

	return true;
}

bool DrawBlock(SDL_Renderer* renderer, SDL_Texture* texture, const int8_t x, const int8_t y)
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
	const int8_t droppingTetrominoX = droppingTetromino->x;
	const int8_t droppingTetrominoY = droppingTetromino->y;
	const uint8_t* droppingTetrominoRotatedOffsets = droppingTetromino->shape.offsets[droppingTetromino->rotation];

	// Iterate over the coordinate pairs of each block in the tetromino
	for (int i = 0; i < TETROMINO_SIZE * 2; i += 2)
	{
		if (DrawBlock(renderer,
		              droppingTetrominoTexture,
		              droppingTetrominoX + droppingTetrominoRotatedOffsets[i],
		              droppingTetrominoY + droppingTetrominoRotatedOffsets[i + 1]) == false)
			return false;
	}

	return true;
}
