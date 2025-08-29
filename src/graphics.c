#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#include "graphics.h"

#include "util.h"
#include "game.h"
#include "tetromino.h"

static SDL_Texture* graphicsBlackBlock;

bool InitGraphicsData(void)
{
    graphicsData.gridSquareSize = 60;
    graphicsData.sideBarGridWidth = 5;

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

    graphicsBlackBlock = IMG_LoadTexture(renderer, "resources/images/blocks/black.png");

    // Load fonts
    graphicsData.mainFont = TTF_OpenFont("resources/fonts/doto_extra_bold.ttf", 150);
    graphicsData.secondaryFont = TTF_OpenFont("resources/fonts/doto_regular.ttf", 150);

    // Initialise static text
    const SDL_Color colorWhite = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(graphicsData.mainFont, "TETRIS", 0, colorWhite);
    graphicsData.titleTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_DestroySurface(textSurface);

    return true;
}

bool DrawBlock(SDL_Renderer* renderer, SDL_Texture* texture, const Uint8 alpha, const float blockSize, const int x, const int y)
{
    if (x >= ARENA_WIDTH || x < 0 || y >= ARENA_HEIGHT || y < 0)
        return false;

    const SDL_FRect rect = { (float)x * blockSize, (float)y * blockSize, blockSize, blockSize };

    if (SDL_SetTextureAlphaMod(texture, alpha) == false) return false;
    return SDL_RenderTexture(renderer, texture, NULL, &rect);
}

bool DrawArena(SDL_Renderer* renderer, GameDataContext* gameDataContext)
{
    for (int row = 0; row < ARENA_HEIGHT; row++)
    {
        for (int col = 0; col < ARENA_WIDTH; col++)
        {
            // Draw filled blocks
            if (gameDataContext->arena[row][col])
            {
                SDL_Texture* blockTexture = GetTetrominoShapeByIdentifier(gameDataContext->arena[row][col])->texture;
                DrawBlock(renderer, blockTexture, 255, graphicsData.gridSquareSize, col, row);
            }

            // Draw grid
            SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255); // Grey
            SDL_FRect rect = {
                (float)col * graphicsData.gridSquareSize,
                (float)row * graphicsData.gridSquareSize,
                graphicsData.gridSquareSize,
                graphicsData.gridSquareSize
            };
            if (SDL_RenderRect(renderer, &rect) == false)
                return false;
        }
    }

    return true;
}

bool DrawDroppingTetromino(SDL_Renderer* renderer, GameDataContext* gameDataContext)
{
    SDL_Texture* droppingTetrominoTexture = gameDataContext->droppingTetromino->shape->texture;
    const int droppingTetrominoX = gameDataContext->droppingTetromino->x;
    const int droppingTetrominoY = gameDataContext->droppingTetromino->y;
    const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = gameDataContext->droppingTetromino->shape->coordinates[gameDataContext->droppingTetromino->orientation];

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

bool DrawDroppingTetrominoGhost(SDL_Renderer* renderer, GameDataContext* gameDataContext)
{
    SDL_Texture* droppingTetrominoTexture = gameDataContext->droppingTetromino->shape->texture;
    const int droppingTetrominoX = gameDataContext->droppingTetromino->x;
    const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = gameDataContext->droppingTetromino->shape->coordinates[gameDataContext->droppingTetromino->orientation];

    // Iterate vertically translated collision checks until the tetromino would collide, then modify
    // this translation given the known size of the tetromino representations
    int translationY = 1;
    while (CheckDroppingTetrominoCollision(gameDataContext, 0, translationY++, 0) == false) { }
    translationY += gameDataContext->droppingTetromino->y - (TETROMINO_MAX_SIZE / 2);

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

bool DrawSidebar(SDL_Renderer* renderer, GameDataContext* gameDataContext)
{
    // Draw sidebar background
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255); // Grey
    const SDL_FRect backgroundRect = {
        ARENA_WIDTH * graphicsData.gridSquareSize, 0, graphicsData.sideBarGridWidth * graphicsData.gridSquareSize,
        ARENA_HEIGHT * graphicsData.gridSquareSize
    };
    if (SDL_RenderRect(renderer, &backgroundRect) == false)
        return false;

    // The sidebar margin
    static const float MARGIN = 1.0f;
    // Default positioning for text
    SDL_FRect rect = {
        ((float)ARENA_WIDTH + MARGIN) * graphicsData.gridSquareSize,
        graphicsData.gridSquareSize,
        graphicsData.gridSquareSize * ((float)graphicsData.sideBarGridWidth - MARGIN * 2),
        graphicsData.gridSquareSize * 1
    };

    // Draw title
    SDL_RenderTexture(renderer, graphicsData.titleTexture, NULL, &rect);

    const SDL_Color colorWhite = { 255, 255, 255, 255 };

    // Draw score
    char text[MAX_STRING_LENGTH];
    if (SDL_snprintf(text, 8, "%06d", gameDataContext->score) < 0) return false;
    rect.y += graphicsData.gridSquareSize;

    static TextCache cache001 = { {0}, NULL, false };
    SDL_Texture* scoreTexture = GenerateTextTexture(renderer, text, &cache001, graphicsData.secondaryFont, colorWhite);
    if (SDL_RenderTexture(renderer, scoreTexture, NULL, &rect) == false) return false;
    //SDL_DestroyTexture(scoreTexture);

    // Draw level
    if (SDL_snprintf(text, 8, "LVL %03d", gameDataContext->level) < 0) return false;
    rect.y += graphicsData.gridSquareSize;

    static TextCache cache002 = { {0}, NULL, false };
    SDL_Texture* levelTexture = GenerateTextTexture(renderer, text, &cache002, graphicsData.secondaryFont, colorWhite);
    if (SDL_RenderTexture(renderer, levelTexture, NULL, &rect) == false) return false;
    //SDL_DestroyTexture(levelTexture);

    return true;
}

bool ResizeGridSquares(const Sint32 windowWidth, const Sint32 windowHeight)
{
    const float widthBasedSize = (float)windowWidth / ((float)ARENA_WIDTH + (float)graphicsData.sideBarGridWidth);
    const float heightBasedSize = (float)windowHeight / (float)ARENA_HEIGHT;

    graphicsData.gridSquareSize = (widthBasedSize < heightBasedSize) ? widthBasedSize : heightBasedSize;

    return true;
}

SDL_Texture* GenerateTextTexture(SDL_Renderer* renderer, const char* text, TextCache* cache, TTF_Font* font, const SDL_Color color)
{
    if (cache->valid && !strcmp(text, cache->text))
    {
        return cache->texture;
    }

    // Cache miss
    SDL_DestroyTexture(cache->texture);
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, 0, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    memcpy(cache->text, text, MAX_STRING_LENGTH);
    cache->texture = texture;
    cache->valid = true;

    return texture;
}