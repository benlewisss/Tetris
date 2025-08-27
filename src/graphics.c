#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdbool.h>

#include "graphics.h"

#include "util.h"
#include "game.h"
#include "tetromino.h"

// Maybe we should make a fake "enum" or global struct that we can use to house all of these utility values, maybe also store the grid square size so we don't need to pass this through
static TTF_Font* g_dotoExtraBold;
static TTF_Font* g_dotoRegular;

bool InitGraphicsConfig()
{
	GraphicsConfig.gridSquareSize = 60;
	GraphicsConfig.sideBarGridWidth = 5;

	return true;
}

bool ResizeWindow(const Sint32 windowWidth, const Sint32 windowHeight)
{
	const float widthBasedSize = windowWidth / ((float)ARENA_WIDTH + (float) GraphicsConfig.sideBarGridWidth);
	const float heightBasedSize = windowHeight / (float)ARENA_HEIGHT;

	GraphicsConfig.gridSquareSize = (widthBasedSize < heightBasedSize) ? widthBasedSize : heightBasedSize;

	return true;
}

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

	// Load fonts
	g_dotoExtraBold = TTF_OpenFont("resources/fonts/doto_extra_bold.ttf", 150);
	g_dotoRegular = TTF_OpenFont("resources/fonts/doto_regular.ttf", 150);

	// Initialise static text
	SDL_Color colorWhite = { 255,255,255,255 };
	SDL_Surface* textSurface = TTF_RenderText_Blended(g_dotoExtraBold, "TETRIS", 0, colorWhite);
	GraphicsConfig.titleTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	
	SDL_DestroySurface(textSurface);

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
				DrawBlock(renderer, blockTexture, 255, GraphicsConfig.gridSquareSize, col, row);
			}

			// Draw grid (This is the most performance hungry operation, can optimise by drawing images instead).
			SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255); // Grey
			SDL_FRect rect = {(float)col * GraphicsConfig.gridSquareSize, (float)row * GraphicsConfig.gridSquareSize, GraphicsConfig.gridSquareSize, GraphicsConfig.gridSquareSize };
			if (SDL_RenderRect(renderer, &rect) == false)
				return false;
		}
	}

	return true;
}

bool DrawBlock(SDL_Renderer* renderer, SDL_Texture* texture, const Uint8 alpha, const float blockSize, const int x, const int y)
{
	if (x >= ARENA_WIDTH || x < 0 || y >= ARENA_HEIGHT || y < 0)
		return false; 

	const SDL_FRect rect = {(float)x * blockSize, (float)y * blockSize, blockSize, blockSize };

	if (SDL_SetTextureAlphaMod(texture, alpha) == false) return false;
	return SDL_RenderTexture(renderer, texture, NULL, &rect);
}

bool DrawDroppingTetromino(SDL_Renderer* renderer, const DroppingTetromino* droppingTetromino)
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
				droppingTetrominoTexture,
				255,
				GraphicsConfig.gridSquareSize,
				droppingTetrominoX + j,
				droppingTetrominoY + i) == false)
				return false;
		}
	}

	return true;
}

bool DrawDroppingTetrominoGhost(SDL_Renderer* renderer, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], const DroppingTetromino* droppingTetromino)
{
	SDL_Texture* droppingTetrominoTexture = droppingTetromino->shape.texture;
	const int droppingTetrominoX = droppingTetromino->x;
	const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape.coordinates[droppingTetromino->rotation];

	int translationY = 1;
	while (CheckDroppingTetrominoCollision(arena, droppingTetromino, 0, translationY++, 0) == false) {}
	translationY += droppingTetromino->y - (TETROMINO_MAX_SIZE/2);

	for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
	{
		for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
		{
			if (droppingTetrominoRotatedCoordinates[i][j] == false) continue;
			if (DrawBlock(renderer, droppingTetrominoTexture,50, GraphicsConfig.gridSquareSize, droppingTetrominoX + j, translationY + i) == false) return false;
		}
	}

	return true;
}

bool DrawSideBar(SDL_Renderer* renderer, const int score, const int level)
{
	// Draw sidebar background
	SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255); // Grey
	const SDL_FRect backgroundRect = { ARENA_WIDTH * GraphicsConfig.gridSquareSize, 0, GraphicsConfig.sideBarGridWidth * GraphicsConfig.gridSquareSize, ARENA_HEIGHT * GraphicsConfig.gridSquareSize };
	if (SDL_RenderRect(renderer, &backgroundRect) == false)
		return false;


	// The sidebar margin and the default positioning for text
	static const float MARGIN = 1.0f;
	SDL_FRect rect = { 
		((float) ARENA_WIDTH + MARGIN) * GraphicsConfig.gridSquareSize,
		GraphicsConfig.gridSquareSize,
		GraphicsConfig.gridSquareSize * ((float) GraphicsConfig.sideBarGridWidth - MARGIN * 2),
		GraphicsConfig.gridSquareSize * 1};

	// Draw title
	SDL_RenderTexture(renderer, GraphicsConfig.titleTexture, NULL, &rect);

	// Draw score
	char text[10];
	if (SDL_snprintf(text, 8, "%06d", score) < 0) return false;
	rect.y += GraphicsConfig.gridSquareSize;

	const SDL_Color colorWhite = { 255,255,255,255 };
	SDL_Surface* surface = TTF_RenderText_Blended(g_dotoRegular, text, 0, colorWhite);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (SDL_RenderTexture(renderer, texture, NULL, &rect) == false) return false;

	// Draw level
	if (SDL_snprintf(text, 8, "LVL %03d", level) < 0) return false;
	rect.y += GraphicsConfig.gridSquareSize;

	surface = TTF_RenderText_Blended(g_dotoRegular, text, 0, colorWhite);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (SDL_RenderTexture(renderer, texture, NULL, &rect) == false) return false;

	SDL_DestroySurface(surface);
	SDL_DestroyTexture(texture);

	return true;
}