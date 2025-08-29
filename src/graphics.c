#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#include "graphics.h"

#include "util.h"
#include "game.h"
#include "tetromino.h"

bool InitGraphicsData(GraphicsDataContext* graphicsDataContext)
{
    // Default size of 60
    graphicsDataContext->gridSquareSize = 60;

    // Set default game size based on monitor resolution
    const SDL_DisplayID displayId = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* displayMode = SDL_GetDesktopDisplayMode(displayId);
    ResizeGridSquares(graphicsDataContext, (Sint32)(displayMode->w * 0.8), (Sint32)(displayMode->h * 0.8));

    // TODO Create window based off resolution
    const int width = (int)((float)(ARENA_WIDTH + SIDEBAR_GRID_WIDTH) * graphicsDataContext->gridSquareSize);
    const int height = (int)((float)ARENA_HEIGHT * graphicsDataContext->gridSquareSize);
    SDL_CreateWindowAndRenderer("TETRIS", width, height, SDL_WINDOW_RESIZABLE, &graphicsDataContext->window, &graphicsDataContext->renderer);

    if (Assert(graphicsDataContext->window, "Window creation failed!\n")) return SDL_APP_FAILURE;
    if (Assert(graphicsDataContext->renderer, "Renderer creation failed!\n")) return SDL_APP_FAILURE;

    return true;
}

bool LoadResources(const GraphicsDataContext* graphicsDataContext, Fonts* fonts, Textures* textures)
{
    // Load tetromino textures
    if (!(GetTetrominoShapeByIdentifier(I)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/cyan.png"))) return false;
    if (!(GetTetrominoShapeByIdentifier(O)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/yellow.png"))) return false;
    if (!(GetTetrominoShapeByIdentifier(T)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/purple.png"))) return false;
    if (!(GetTetrominoShapeByIdentifier(Z)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/red.png"))) return false;
    if (!(GetTetrominoShapeByIdentifier(S)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/green.png"))) return false;
    if (!(GetTetrominoShapeByIdentifier(L)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/orange.png"))) return false;
    if (!(GetTetrominoShapeByIdentifier(J)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/blue.png"))) return false;

    // Load fonts
    fonts->mainFont = TTF_OpenFont("resources/fonts/doto_extra_bold.ttf", 150);
    fonts->secondaryFont = TTF_OpenFont("resources/fonts/doto_regular.ttf", 150);

    // Initialise static text
    const SDL_Color colorWhite = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(fonts->mainFont, "TETRIS", 0, colorWhite);
    textures->titleTexture = SDL_CreateTextureFromSurface(graphicsDataContext->renderer, textSurface);
    SDL_DestroySurface(textSurface);

    return true;
}

bool DrawBlock(const GraphicsDataContext* graphicsDataContext, SDL_Texture* texture, const Uint8 alpha, const int x, const int y)
{
    if (x >= ARENA_WIDTH || x < 0 || y >= ARENA_HEIGHT || y < 0)
        return false;

    const SDL_FRect rect = { (float)x * graphicsDataContext->gridSquareSize, (float)y * graphicsDataContext->gridSquareSize, graphicsDataContext->gridSquareSize, graphicsDataContext->gridSquareSize };

    if (SDL_SetTextureAlphaMod(texture, alpha) == false) return false;
    return SDL_RenderTexture(graphicsDataContext->renderer, texture, NULL, &rect);
}

bool DrawArena(GraphicsDataContext* graphicsDataContext, const GameDataContext* gameDataContext)
{
    for (int row = 0; row < ARENA_HEIGHT; row++)
    {
        for (int col = 0; col < ARENA_WIDTH; col++)
        {
            // Draw filled blocks
            if (gameDataContext->arena[row][col])
            {
                SDL_Texture* blockTexture = GetTetrominoShapeByIdentifier(gameDataContext->arena[row][col])->texture;
                DrawBlock(graphicsDataContext, blockTexture, 255, col, row);
            }

            // Draw grid
            SDL_SetRenderDrawColor(graphicsDataContext->renderer, 32, 32, 32, 255); // Grey
            SDL_FRect rect = {
                (float)col * graphicsDataContext->gridSquareSize,
                (float)row * graphicsDataContext->gridSquareSize,
                graphicsDataContext->gridSquareSize,
                graphicsDataContext->gridSquareSize
            };
            if (SDL_RenderRect(graphicsDataContext->renderer, &rect) == false)
                return false;
        }
    }

    return true;
}

bool DrawDroppingTetromino(GraphicsDataContext* graphicsDataContext, const GameDataContext* gameDataContext)
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
            if (DrawBlock(graphicsDataContext,
                          droppingTetrominoTexture,
                          255,
                          droppingTetrominoX + j,
                          droppingTetrominoY + i) == false)
                return false;
        }
    }

    return true;
}

bool DrawDroppingTetrominoGhost(GraphicsDataContext* graphicsDataContext, GameDataContext* gameDataContext)
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
            if (DrawBlock(graphicsDataContext,
                          droppingTetrominoTexture,
                          50,
                          droppingTetrominoX + j,
                          translationY + i) == false) return false;
        }
    }

    return true;
}

bool DrawSidebar(GraphicsDataContext* graphicsDataContext, const Fonts* fonts, const Textures* textures, const GameDataContext* gameDataContext)
{
    // Draw sidebar background
    SDL_SetRenderDrawColor(graphicsDataContext->renderer, 20, 20, 20, 255); // Grey
    const SDL_FRect backgroundRect = {
        (float)ARENA_WIDTH * graphicsDataContext->gridSquareSize, 0, (float)SIDEBAR_GRID_WIDTH * graphicsDataContext->gridSquareSize,
        (float)ARENA_HEIGHT * graphicsDataContext->gridSquareSize
    };
    if (SDL_RenderRect(graphicsDataContext->renderer, &backgroundRect) == false)
        return false;

    // The sidebar margin
    static const float MARGIN = 1.0f;
    // Default positioning for text
    SDL_FRect rect = {
        ((float)ARENA_WIDTH + MARGIN) * graphicsDataContext->gridSquareSize,
        graphicsDataContext->gridSquareSize,
        graphicsDataContext->gridSquareSize* ((float)SIDEBAR_GRID_WIDTH - MARGIN * 2),
       graphicsDataContext->gridSquareSize * 1
    };

    // Draw title
    SDL_RenderTexture(graphicsDataContext->renderer, textures->titleTexture, NULL, &rect);

    const SDL_Color colorWhite = { 255, 255, 255, 255 };

    // Draw score
    char text[MAX_STRING_LENGTH];
    if (SDL_snprintf(text, 8, "%06d", gameDataContext->score) < 0) return false;
    rect.y += graphicsDataContext->gridSquareSize;

    static TextCache cache001 = { {0}, NULL, false };
    SDL_Texture* scoreTexture = GenerateTextTexture(graphicsDataContext, text, &cache001, fonts->secondaryFont, colorWhite);
    if (SDL_RenderTexture(graphicsDataContext->renderer, scoreTexture, NULL, &rect) == false) return false;
    //SDL_DestroyTexture(scoreTexture);

    // Draw level
    if (SDL_snprintf(text, 8, "LVL %03d", gameDataContext->level) < 0) return false;
    rect.y += graphicsDataContext->gridSquareSize;

    static TextCache cache002 = { {0}, NULL, false };
    SDL_Texture* levelTexture = GenerateTextTexture(graphicsDataContext, text, &cache002, fonts->secondaryFont, colorWhite);
    if (SDL_RenderTexture(graphicsDataContext->renderer, levelTexture, NULL, &rect) == false) return false;
    //SDL_DestroyTexture(levelTexture);

    return true;
}

bool ResizeGridSquares(GraphicsDataContext* graphicsDataContext, const Sint32 windowWidth, const Sint32 windowHeight)
{
    const float widthBasedSize = (float)windowWidth / ((float)ARENA_WIDTH + (float)SIDEBAR_GRID_WIDTH);
    const float heightBasedSize = (float)windowHeight / (float)ARENA_HEIGHT;

    graphicsDataContext->gridSquareSize = (widthBasedSize < heightBasedSize) ? widthBasedSize : heightBasedSize;

    return true;
}

SDL_Texture* GenerateTextTexture(const GraphicsDataContext* graphicsDataContext, const char* text, TextCache* cache, TTF_Font* font, const SDL_Color color)
{
    if (cache->valid && !strcmp(text, cache->text))
    {
        return cache->texture;
    }

    // Cache miss
    SDL_DestroyTexture(cache->texture);
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, 0, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(graphicsDataContext->renderer, surface);
    SDL_DestroySurface(surface);

    memcpy(cache->text, text, MAX_STRING_LENGTH);
    cache->texture = texture;
    cache->valid = true;

    return texture;
}