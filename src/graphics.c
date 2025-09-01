#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#include "graphics.h"

#include "util.h"
#include "game.h"
#include "tetromino.h"

//TODO This should return an SDL_APP_FAILURE OR SDL_APP_SUCCESS AND PASS IT TO OUTER CALLER
bool InitGraphicsData(GraphicsDataContext* graphicsDataContext)
{
    // Programmer assert
    if (Assert(WINDOW_GRID_WIDTH >= (int)ARENA_WIDTH + SIDEBAR_GRID_WIDTH, "Invalid window grid width!\n")) return SDL_APP_FAILURE;
    if (Assert(WINDOW_GRID_HEIGHT >= (int)ARENA_HEIGHT, "Invalid window grid width!\n")) return SDL_APP_FAILURE;

    // Default size of 60
    graphicsDataContext->gridSquareSize = 60;

    // Set default game size based on monitor resolution
    const SDL_DisplayID displayId = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* displayMode = SDL_GetDesktopDisplayMode(displayId);
    ResizeGridSquares(graphicsDataContext, (Sint32)(displayMode->w * 0.8), (Sint32)(displayMode->h * 0.8));

    // TODO Create window based off resolution
    const int width = (int)((float)WINDOW_GRID_WIDTH * graphicsDataContext->gridSquareSize);
    const int height = (int)((float)WINDOW_GRID_HEIGHT * graphicsDataContext->gridSquareSize);
    SDL_CreateWindowAndRenderer("TETRIS", width, height, SDL_WINDOW_RESIZABLE, &graphicsDataContext->window, &graphicsDataContext->renderer);

    const float ratio = (float)width / (float)height;

    SDL_SetWindowAspectRatio(graphicsDataContext->window, ratio, ratio);

    SDL_SetRenderDrawBlendMode(graphicsDataContext->renderer, SDL_BLENDMODE_BLEND);

    if (Assert(graphicsDataContext->window, "Window creation failed!\n")) return SDL_APP_FAILURE;
    if (Assert(graphicsDataContext->renderer, "Renderer creation failed!\n")) return SDL_APP_FAILURE;

    return true;
}

bool LoadResources(GraphicsDataContext* graphicsDataContext, Fonts* fonts, Textures* textures)
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

    const SDL_Color colorWhite = { 255, 255, 255, 255 };
    static TextCache cache001 = { 0 };
    textures->titleTexture = GenerateTextTexture(graphicsDataContext, "TETRIS", &cache001, fonts->mainFont, colorWhite);

    return true;
}

bool DrawBlock(GraphicsDataContext* graphicsDataContext, SDL_Texture* texture, const Uint8 alpha, const int x, const int y)
{
    if (x >= ARENA_WIDTH || x < 0 || y >= ARENA_HEIGHT || y < 0)
        return false;

    const SDL_FRect rect = GenerateRect(graphicsDataContext, (float)x, (float)y, 1, 1);

    if (!SDL_SetTextureAlphaMod(texture, alpha)) return false;
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
                const TetrominoShape* shape = GetTetrominoShapeByIdentifier(gameDataContext->arena[row][col]);
                SDL_Texture* blockTexture = shape->texture;
                if (!blockTexture) return false;

                DrawBlock(graphicsDataContext, blockTexture, 255, col, row);
            }

            // Draw grid
            SDL_SetRenderDrawColor(graphicsDataContext->renderer, 32, 32, 32, 255); // Grey
            SDL_FRect rect = GenerateRect(graphicsDataContext, (float)col, (float)row, 1, 1);
            if (!SDL_RenderRect(graphicsDataContext->renderer, &rect))
                return false;
        }
    }
    return true;
}

bool DrawDroppingTetromino(GraphicsDataContext* graphicsDataContext, const GameDataContext* gameDataContext)
{
    DroppingTetromino* droppingTetromino = gameDataContext->droppingTetromino;
    if (!droppingTetromino)
    {
        return false;
    }

    SDL_Texture* droppingTetrominoTexture = droppingTetromino->shape->texture;
    const int droppingTetrominoX = droppingTetromino->x;
    const int droppingTetrominoY = droppingTetromino->y;
    const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape->coordinates[gameDataContext->droppingTetromino->orientation];

    for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
    {
        for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
        {
            if (!droppingTetrominoRotatedCoordinates[i][j]) continue;
            if (!DrawBlock(graphicsDataContext,
                          droppingTetrominoTexture,
                          255,
                          droppingTetrominoX + j,
                          droppingTetrominoY + i))
                return false;
        }
    }

    return true;
}

bool DrawDroppingTetrominoGhost(GraphicsDataContext* graphicsDataContext, GameDataContext* gameDataContext)
{

    DroppingTetromino* droppingTetromino = gameDataContext->droppingTetromino;
    if (!droppingTetromino)
    {
        return false;
    }

    SDL_Texture* droppingTetrominoTexture = droppingTetromino->shape->texture;
    const int droppingTetrominoX = droppingTetromino->x;
    const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape->coordinates[gameDataContext->droppingTetromino->orientation];

    // Iterate vertically translated collision checks until the tetromino would collide, then modify
    // this translation given the known size of the tetromino representations
    int translationY = 1;
    while (!WillDroppingTetrominoCollide(gameDataContext, 0, translationY++, 0)) { }
    translationY += droppingTetromino->y - (TETROMINO_MAX_SIZE / 2);

    for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
    {
        for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
        {
            if (!droppingTetrominoRotatedCoordinates[i][j]) continue;
            if (!DrawBlock(graphicsDataContext,
                          droppingTetrominoTexture,
                          50,
                          droppingTetrominoX + j,
                          translationY + i)) return false;
        }
    }

    return true;
}

bool DrawSidebar(GraphicsDataContext* graphicsDataContext, const Fonts* fonts, const Textures* textures, const GameDataContext* gameDataContext)
{
    // TODO Spruce up the sidebar with some textures, maybe some pixel art, some bounding boxes.

    // Draw sidebar background
    SDL_SetRenderDrawColor(graphicsDataContext->renderer, 20, 20, 20, 255); // Grey
    const SDL_FRect backgroundRect = GenerateRect(graphicsDataContext, ARENA_WIDTH, 0, SIDEBAR_GRID_WIDTH, WINDOW_GRID_HEIGHT);
    if (!SDL_RenderRect(graphicsDataContext->renderer, &backgroundRect))
        return false;

    const SDL_Color colorWhite = { 255, 255, 255, 255 };

    static TextCache cache001 = { 0 };
    if (!RenderText(graphicsDataContext, ARENA_WIDTH, 0, SIDEBAR_GRID_WIDTH, 1, 0.1f, "TETRIS", &cache001, fonts->mainFont, colorWhite)) return false;

    // Draw score
    char text[MAX_STRING_LENGTH];
    if (SDL_snprintf(text, 8, "%06d", gameDataContext->score) < 0) return false;

    static TextCache cache002 = { 0 };
    if (!RenderText(graphicsDataContext, ARENA_WIDTH, 2, SIDEBAR_GRID_WIDTH, 1, 0.1f, text, &cache002, fonts->secondaryFont, colorWhite)) return false;

    // Draw level
    if (SDL_snprintf(text, 8, "LVL %03d", gameDataContext->level) < 0) return false;

    static TextCache cache003 = { 0 };
    if (!RenderText(graphicsDataContext, ARENA_WIDTH, 3, SIDEBAR_GRID_WIDTH, 1, 0.1f, text, &cache003, fonts->secondaryFont, colorWhite)) return false;

    return true;
}

bool DrawGameOverScreen(GraphicsDataContext* graphicsDataContext, const Fonts* fonts, const Textures* textures, const GameDataContext* gameDataContext)
{
    const SDL_Color colorWhite = { 255, 255, 255, 255 };

    // Draw menu background
    SDL_SetRenderDrawColor(graphicsDataContext->renderer, 10, 10, 10, 200); // Grey
    const SDL_FRect backgroundRect = GenerateRect(graphicsDataContext, 0, 0, ARENA_WIDTH, ARENA_HEIGHT);
    if (!SDL_RenderFillRect(graphicsDataContext->renderer, &backgroundRect)) return false;

    // Draw title
    static TextCache cache001 = { 0 };
    if (!RenderText(graphicsDataContext, 0, 0, ARENA_WIDTH, ARENA_HEIGHT, 0.5f, "GAME OVER", &cache001, fonts->mainFont, colorWhite)) return false;

    return true;
}

bool ResizeGridSquares(GraphicsDataContext* graphicsDataContext, const Sint32 windowWidth, const Sint32 windowHeight)
{
    const float widthBasedSize = (float)windowWidth / ((float)ARENA_WIDTH + (float)SIDEBAR_GRID_WIDTH);
    const float heightBasedSize = (float)windowHeight / (float)ARENA_HEIGHT;

    graphicsDataContext->gridSquareSize = (widthBasedSize < heightBasedSize) ? widthBasedSize : heightBasedSize;

    return true;
}

bool RenderText(GraphicsDataContext* graphicsDataContext, const float x, const float y, const float width, const float height, const float margin, char* text, TextCache* cache, TTF_Font* font, const SDL_Color color)
{
    // TODO Maybe delegate the caching system to RenderText, and include the generation of the Rect as well?
    SDL_Texture* texture = GenerateTextTexture(graphicsDataContext, text, cache, font, color);
    if (!texture) return false;

    const float innerWidth = (width - margin * 2) * graphicsDataContext->gridSquareSize;
    const float innerHeight = (height - margin * 2) * graphicsDataContext->gridSquareSize;
    const float innerX = (x + margin) * graphicsDataContext->gridSquareSize;
    const float innerY = (y + margin) * graphicsDataContext->gridSquareSize;

    // Aspect ratio
    const float widthRatio = innerWidth / (float)texture->w;
    const float heightRatio = innerHeight / (float)texture->h;
    const float ratio = (widthRatio <= heightRatio) ? widthRatio : heightRatio;

    const float scaledWidth = (float)texture->w * ratio;
    const float scaledHeight = (float)texture->h * ratio;

    // Center inside inner-rect
    const float centeredX = innerX + (innerWidth - scaledWidth) / 2;
    const float centeredY = innerY + (innerHeight - scaledHeight) / 2;

    const SDL_FRect rect = {
        centeredX,
        centeredY,
        scaledWidth,
        scaledHeight,
    };

    if (!SDL_RenderTexture(graphicsDataContext->renderer, texture, NULL, &rect)) return false;
    return true;
}

SDL_FRect GenerateRect(const GraphicsDataContext* graphicsDataContext, const float x, const float y, const float width, const float height)
{
    const SDL_FRect rect = {
    x * graphicsDataContext->gridSquareSize,
    y * graphicsDataContext->gridSquareSize,
    width * graphicsDataContext->gridSquareSize,
    height * graphicsDataContext->gridSquareSize
    };

    return rect;
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