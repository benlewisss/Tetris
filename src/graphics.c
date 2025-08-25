#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>

#include "graphics.h"

#include "util.h"
#include "game.h"
#include "tetromino.h"

bool LoadResources(SDL_Renderer* renderer)
{
	// Load tetromino textures
	if (!(GetTetrominoShapeByIdentifier(I)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/cyan.png"))) return false;
	if (!(GetTetrominoShapeByIdentifier(O)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/yellow.png"))) return false;
	if (!(GetTetrominoShapeByIdentifier(T)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/purple.png"))) return false;
	if (!(GetTetrominoShapeByIdentifier(Z)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/red.png"))) return false;
	if (!(GetTetrominoShapeByIdentifier(S)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/green.png"))) return false;
	if (!(GetTetrominoShapeByIdentifier(L)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/orange.png"))) return false;
	if (!(GetTetrominoShapeByIdentifier(J)->texture = IMG_LoadTexture(renderer, "resources/images/blocks/blue.png"))) return false;

	return true;
}

bool DrawArena(SDL_Renderer* renderer, const float blockSize, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH])
{
	for (int row = 0; row < ARENA_HEIGHT; row++)
	{
		for (int col = 0; col < ARENA_WIDTH; col++)
		{
			// Draw filled blocks
			if (arena[row][col])
			{
				SDL_Texture* blockTexture = GetTetrominoShapeByIdentifier(arena[row][col])->texture;
				DrawBlock(renderer, blockSize, blockTexture, 255, col, row);
			}

			// Draw grid (This is the most performance hungry operation, can optimise by drawing images instead).
			SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255); // Grey
			SDL_FRect rect = {(float)col * blockSize, (float)row * blockSize, blockSize, blockSize };
			if (SDL_RenderRect(renderer, &rect) == false)
				return false;
		}
	}

	return true;
}

bool DrawBlock(SDL_Renderer* renderer, const float blockSize, SDL_Texture* texture, const Uint8 alpha, const int x, const int y)
{
	if (x >= ARENA_WIDTH || x < 0 || y >= ARENA_HEIGHT || y < 0)
		return false; 

	const SDL_FRect rect = {(float)x * blockSize, (float)y * blockSize, blockSize, blockSize};

	if (!SDL_SetTextureAlphaMod(texture, alpha)) return false;
	return SDL_RenderTexture(renderer, texture, NULL, &rect);
}

bool DrawDroppingTetromino(SDL_Renderer* renderer, const float blockSize, const DroppingTetromino* droppingTetromino)
{
	SDL_Texture* droppingTetrominoTexture = droppingTetromino->shape.texture;
	const int droppingTetrominoX = droppingTetromino->x;
	const int droppingTetrominoY = droppingTetromino->y;
	const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape.coordinates[droppingTetromino->rotation];

	for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
	{
		for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
		{
			if (droppingTetrominoRotatedCoordinates[i][j] == false) continue;

			if (DrawBlock(renderer,
				blockSize,
				droppingTetrominoTexture,
				255,
				droppingTetrominoX + j,
				droppingTetrominoY + i) == false)
				return false;
		}
	}

	return true;
}

bool DrawDroppingTetrominoGhost(SDL_Renderer* renderer, const float blockSize, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], const DroppingTetromino* droppingTetromino)
{
	SDL_Texture* droppingTetrominoTexture = droppingTetromino->shape.texture;
	const int droppingTetrominoX = droppingTetromino->x;
	const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape.coordinates[droppingTetromino->rotation];

	int translationY = 1;
	while (!CheckDroppingTetrominoCollision(arena, droppingTetromino, 0, translationY++, 0)) {}
	translationY += droppingTetromino->y - (TETROMINO_MAX_SIZE/2);

	for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
	{
		for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
		{
			if (droppingTetrominoRotatedCoordinates[i][j] == false) continue;
			if (DrawBlock(renderer, blockSize, droppingTetrominoTexture,50, droppingTetrominoX + j, translationY + i) == false) return false;
		}
	}

	return true;
}
