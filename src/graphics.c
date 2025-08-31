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
                SDL_Texture* blockTexture = GetTetrominoShapeByIdentifier(gameDataContext->arena[row][col])->texture;
                DrawBlock(graphicsDataContext, blockTexture, 255, col, row);
            }

            // Draw grid
            SDL_SetRenderDrawColor(graphicsDataContext->renderer, 32, 32, 32, 255); // Grey
            SDL_FRect rect = GenerateRect(graphicsDataContext, (float)col, (float)row, 1, 1);
            if (SDL_RenderRect(graphicsDataContext->renderer, &rect) == false)
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
    while (CheckDroppingTetrominoCollision(gameDataContext, 0, translationY++, 0) == false) { }
    translationY += droppingTetromino->y - (TETROMINO_MAX_SIZE / 2);

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
    // TODO Spruce up the sidebar with some textures, maybe some pixel art, some bounding boxes.

    // Draw sidebar background
    SDL_SetRenderDrawColor(graphicsDataContext->renderer, 20, 20, 20, 255); // Grey
    const SDL_FRect backgroundRect = GenerateRect(graphicsDataContext, ARENA_WIDTH, 0, SIDEBAR_GRID_WIDTH, WINDOW_GRID_HEIGHT);
    if (SDL_RenderRect(graphicsDataContext->renderer, &backgroundRect) == false)
        return false;

    // The sidebar margin
    const float margin = 0.25f;
    // Default positioning for text
    SDL_FRect rect = GenerateRect(graphicsDataContext, (float)ARENA_WIDTH + margin, 1, (float)SIDEBAR_GRID_WIDTH - (margin * 2), 1);

    // Draw title
    SDL_RenderTexture(graphicsDataContext->renderer, textures->titleTexture, NULL, &rect);

    const SDL_Color colorWhite = { 255, 255, 255, 255 };

    // Draw score
    char text[MAX_STRING_LENGTH];
    if (SDL_snprintf(text, 8, "%06d", gameDataContext->score) < 0) return false;
    rect.y += graphicsDataContext->gridSquareSize;

    // TODO Implement a hashmap so we don't need to manually define a cache entry
    static TextCache cache001 = { 0 };
    SDL_Texture* scoreTexture = GenerateTextTexture(graphicsDataContext, text, &cache001, fonts->secondaryFont, colorWhite);
    if (SDL_RenderTexture(graphicsDataContext->renderer, scoreTexture, NULL, &rect) == false) return false;

    // Draw level
    if (SDL_snprintf(text, 8, "LVL %03d", gameDataContext->level) < 0) return false;
    rect.y += graphicsDataContext->gridSquareSize;

    static TextCache cache002 = { 0 };
    SDL_Texture* levelTexture = GenerateTextTexture(graphicsDataContext, text, &cache002, fonts->secondaryFont, colorWhite);
    if (SDL_RenderTexture(graphicsDataContext->renderer, levelTexture, NULL, &rect) == false) return false;

    return true;
}

bool DrawGameOverScreen(GraphicsDataContext* graphicsDataContext, const Fonts* fonts, const Textures* textures, const GameDataContext* gameDataContext)
{
    const SDL_Color colorWhite = { 255, 255, 255, 255 };

    // Draw menu background
    SDL_SetRenderDrawColor(graphicsDataContext->renderer, 10, 10, 10, 200); // Grey
    const SDL_FRect backgroundRect = GenerateRect(graphicsDataContext, 0, 0, ARENA_WIDTH, ARENA_HEIGHT);
    if (SDL_RenderFillRect(graphicsDataContext->renderer, &backgroundRect) == false)
        return false;

    // Maybe abstract the centering of text within some grid squares
    const SDL_FRect rect = GenerateRect(graphicsDataContext, (float)ARENA_WIDTH / 2 - 4, (float)ARENA_HEIGHT / 2 - 2, (float)8, 2);

    // Draw title
    static TextCache cache001 = { 0 };
    SDL_Texture* scoreTexture = GenerateTextTexture(graphicsDataContext, "GAME OVER", &cache001, fonts->mainFont, colorWhite);
    if (SDL_RenderTexture(graphicsDataContext->renderer, scoreTexture, NULL, &rect) == false) return false;

    return true;
}  

SDL_FRect GenerateRect(const GraphicsDataContext* graphicsDataContext, const float x, const float y, const float w, const float h)
{
    // TODO Utilise a hashmap (same one you will implement for text) to cache rect sizes as well!
    const SDL_FRect rect = {
    x * graphicsDataContext->gridSquareSize,
    y * graphicsDataContext->gridSquareSize,
    w * graphicsDataContext->gridSquareSize,
    h * graphicsDataContext->gridSquareSize
    };

    return rect;
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
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, 0, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(graphicsDataContext->renderer, surface);
    SDL_DestroySurface(surface);

    memcpy(cache->text, text, MAX_STRING_LENGTH);
    cache->texture = texture;
    cache->valid = true;

    return texture;
}