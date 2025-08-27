#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdbool.h>

#include "graphics.h"

#include <stdio.h>

#include "util.h"
#include "game.h"
#include "tetromino.h"

bool InitGraphicsData()
{
    graphicsData.gridSquareSize = 60;
    graphicsData.sideBarGridWidth = 5;

    return true;
}

bool ResizeGridSquares(const Sint32 windowWidth, const Sint32 windowHeight)
{
    const float widthBasedSize = windowWidth / ((float)ARENA_WIDTH + (float)graphicsData.sideBarGridWidth);
    const float heightBasedSize = windowHeight / (float)ARENA_HEIGHT;

    graphicsData.gridSquareSize = (widthBasedSize < heightBasedSize) ? widthBasedSize : heightBasedSize;

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
    graphicsData.mainFont = TTF_OpenFont("resources/fonts/doto_extra_bold.ttf", 150);
    graphicsData.secondaryFont = TTF_OpenFont("resources/fonts/doto_regular.ttf", 150);

    // Initialise static text
    SDL_Color colorWhite = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(graphicsData.mainFont, "TETRIS", 0, colorWhite);
    graphicsData.titleTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

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
                DrawBlock(renderer, blockTexture, 255, graphicsData.gridSquareSize, col, row);
            }

            // Draw grid (This is the most performance hungry operation, can optimise by drawing images instead).
            SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255); // Grey
            SDL_FRect rect = {
                (float)col * graphicsData.gridSquareSize, (float)row * graphicsData.gridSquareSize,
                graphicsData.gridSquareSize, graphicsData.gridSquareSize
            };
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

    const SDL_FRect rect = {(float)x * blockSize, (float)y * blockSize, blockSize, blockSize};

    if (SDL_SetTextureAlphaMod(texture, alpha) == false) return false;
    return SDL_RenderTexture(renderer, texture, NULL, &rect);
}

bool DrawDroppingTetromino(SDL_Renderer* renderer, const DroppingTetromino* droppingTetromino)
{
    SDL_Texture* droppingTetrominoTexture = droppingTetromino->shape->texture;
    const int droppingTetrominoX = droppingTetromino->x;
    const int droppingTetrominoY = droppingTetromino->y;
    const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape->coordinates[droppingTetromino->orientation];

    for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
    {
        for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
        {
            if (droppingTetrominoRotatedCoordinates[i][j] == false) continue;
            if (DrawBlock(renderer,
                          droppingTetrominoTexture,
                          255,
                          graphicsData.gridSquareSize,
                          droppingTetrominoX + j,
                          droppingTetrominoY + i) == false)
                return false;
        }
    }

    return true;
}

bool DrawDroppingTetrominoGhost(SDL_Renderer* renderer, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH],
                                const DroppingTetromino* droppingTetromino)
{
    SDL_Texture* droppingTetrominoTexture = droppingTetromino->shape->texture;
    const int droppingTetrominoX = droppingTetromino->x;
    const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape->coordinates[droppingTetromino
        ->orientation];

    int translationY = 1;
    while (CheckDroppingTetrominoCollision(arena, droppingTetromino, 0, translationY++, 0) == false)
    {
    }
    translationY += droppingTetromino->y - (TETROMINO_MAX_SIZE / 2);

    for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
    {
        for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
        {
            if (droppingTetrominoRotatedCoordinates[i][j] == false) continue;
            if (DrawBlock(renderer,
                          droppingTetrominoTexture,
                          50,
                          graphicsData.gridSquareSize,
                          droppingTetrominoX + j,
                          translationY + i) == false) return false;
        }
    }

    return true;
}

bool DrawSideBar(SDL_Renderer* renderer, const int score, const int level)
{
    // Draw sidebar background
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255); // Grey
    const SDL_FRect backgroundRect = {
        ARENA_WIDTH * graphicsData.gridSquareSize, 0, graphicsData.sideBarGridWidth * graphicsData.gridSquareSize,
        ARENA_HEIGHT * graphicsData.gridSquareSize
    };
    if (SDL_RenderRect(renderer, &backgroundRect) == false)
        return false;


    // The sidebar margin and the default positioning for text
    static const float MARGIN = 1.0f;
    SDL_FRect rect = {
        ((float)ARENA_WIDTH + MARGIN) * graphicsData.gridSquareSize,
        graphicsData.gridSquareSize,
        graphicsData.gridSquareSize * ((float)graphicsData.sideBarGridWidth - MARGIN * 2),
        graphicsData.gridSquareSize * 1
    };

    // Draw title
    SDL_RenderTexture(renderer, graphicsData.titleTexture, NULL, &rect);

    // Draw score
    char text[10];
    if (SDL_snprintf(text, 8, "%06d", score) < 0) return false;
    rect.y += graphicsData.gridSquareSize;

    const SDL_Color colorWhite = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(graphicsData.secondaryFont, text, 0, colorWhite);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (SDL_RenderTexture(renderer, texture, NULL, &rect) == false) return false;

    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);

    // Draw level
    if (SDL_snprintf(text, 8, "LVL %03d", level) < 0) return false;
    rect.y += graphicsData.gridSquareSize;

    surface = TTF_RenderText_Blended(graphicsData.secondaryFont, text, 0, colorWhite);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (SDL_RenderTexture(renderer, texture, NULL, &rect) == false) return false;

    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);

    return true;
}
