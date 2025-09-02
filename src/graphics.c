#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#include "graphics.h"

#include "util.h"
#include "game.h"
#include "tetromino.h"

//TODO This should return an SDL_APP_FAILURE OR SDL_APP_SUCCESS AND PASS IT TO OUTER CALLER
bool GFX_Init(GraphicsDataContext* graphicsDataContext, Fonts* fonts, GameDataContext* gameDataContext)
{
    // Load fonts
    if (!(fonts->mainFont = TTF_OpenFont("resources/fonts/doto_extra_bold.ttf", 150))) return false;
    if (!(fonts->secondaryFont = TTF_OpenFont("resources/fonts/doto_regular.ttf", 150))) return false;

    SidebarUI* sidebar = SDL_calloc(1, sizeof(SidebarUI));

    // TODO Store all other sidebar data here, i.e. the size of the bar and its location etc.

    sidebar->restartButton = (Button){
        .gridRect = {(float)ARENA_WIDTH, 7, 3, 1},
        .color = {40, 40, 40, 255},
        .hoverColor = {80, 80, 80, 255},
        .textColor = {255, 255, 255, 255},
        .font = fonts->mainFont,
        .text = "RESTART",
        .cache = {0},
        .onClick = GAME_Restart,
        .userData = gameDataContext,
    };

    sidebar->pauseButton = (Button){
        .gridRect = {(float)ARENA_WIDTH, 9, 3, 1},
        .color = {40, 40, 40, 255},
        .hoverColor = {80, 80, 80, 255},
        .textColor = {255, 255, 255, 255},
        .font = fonts->mainFont,
        .text = "PAUSE",
        .cache = {0},
        .onClick = GAME_TogglePause,
        .userData = gameDataContext,
    };

    sidebar->quitButton = (Button){
        .gridRect = {(float)ARENA_WIDTH, 11, 3, 1},
        .color = {40, 40, 40, 255},
        .hoverColor = {80, 80, 80, 255},
        .textColor = {255, 255, 255, 255},
        .font = fonts->mainFont,
        .text = "QUIT",
        .cache = {0},
        .onClick = GAME_Quit,
        .userData = gameDataContext,
    };

    sidebar->width = 3;

    // Programmer assert
    Assert(WINDOW_GRID_WIDTH >= (int)ARENA_WIDTH + sidebar->width, "Invalid window grid width!\n");
    Assert(WINDOW_GRID_HEIGHT >= (int)ARENA_HEIGHT, "Invalid window grid width!\n");

    // Initialise UI objects
    graphicsDataContext->sidebarUI = sidebar;

    // Default size of 60
    graphicsDataContext->gridSquareSize = 60;

    // Set default game size based on monitor resolution
    const SDL_DisplayID displayId = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* displayMode = SDL_GetDesktopDisplayMode(displayId);
    ResizeGridSquares(graphicsDataContext, (Sint32)(displayMode->w * 0.8), (Sint32)(displayMode->h * 0.8));

    const int width = (int)((float)WINDOW_GRID_WIDTH * graphicsDataContext->gridSquareSize);
    const int height = (int)((float)WINDOW_GRID_HEIGHT * graphicsDataContext->gridSquareSize);
    SDL_CreateWindowAndRenderer("TETRIS", width, height, SDL_WINDOW_RESIZABLE, &graphicsDataContext->window, &graphicsDataContext->renderer);

    const float ratio = (float)width / (float)height;

    SDL_SetWindowAspectRatio(graphicsDataContext->window, ratio, ratio);
    SDL_SetRenderDrawBlendMode(graphicsDataContext->renderer, SDL_BLENDMODE_BLEND);

    Assert(graphicsDataContext->window, "Window creation failed!\n");
    Assert(graphicsDataContext->renderer, "Renderer creation failed!\n");

    return true;
}

bool GFX_LoadTetrominoTextures(const GraphicsDataContext* graphicsDataContext)
{
    // Load tetromino textures
    if (!(GetTetrominoShapeByIdentifier(I)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/cyan.png"))) return false;
    if (!(GetTetrominoShapeByIdentifier(O)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/yellow.png"))) return false;
    if (!(GetTetrominoShapeByIdentifier(T)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/purple.png"))) return false;
    if (!(GetTetrominoShapeByIdentifier(Z)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/red.png"))) return false;
    if (!(GetTetrominoShapeByIdentifier(S)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/green.png"))) return false;
    if (!(GetTetrominoShapeByIdentifier(L)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/orange.png"))) return false;
    if (!(GetTetrominoShapeByIdentifier(J)->texture = IMG_LoadTexture(graphicsDataContext->renderer, "resources/images/blocks/blue.png"))) return false;

    return true;
}

bool GFX_RenderGame(GraphicsDataContext* graphicsDataContext, GameDataContext* gameDataContext, Fonts* fonts)
{
    // Clear screen
    SDL_SetRenderDrawColor(graphicsDataContext->renderer, 17, 17, 17, 255);
    SDL_RenderClear(graphicsDataContext->renderer);

    // TODO Move all of these graphics and rendering calls into one method in graphics.c
    Assert(DrawDroppingTetromino(graphicsDataContext, gameDataContext), "Failed to draw dropping tetromino!\n");
    Assert(DrawDroppingTetrominoGhost(graphicsDataContext, gameDataContext), "Failed to draw dropping tetromino ghost!\n");
    Assert(DrawArena(graphicsDataContext, gameDataContext), "Failed to draw arena!\n");
    Assert(DrawSidebar(graphicsDataContext, fonts, gameDataContext), "Failed to draw sidebar!\n");

    if (gameDataContext->isGameOver)
    {
        DrawGameOverScreen(graphicsDataContext, fonts, gameDataContext);
    }

    return true;
}


bool DrawBlock(GraphicsDataContext* graphicsDataContext, SDL_Texture* texture, const Uint8 alpha, const int x, const int y)
{
    if (x >= ARENA_WIDTH || x < 0 || y >= ARENA_HEIGHT || y < 0)
        return false;

    const SDL_FRect rect = FGridRectToFRect(graphicsDataContext, (FGridRect){ (float)x, (float)y, 1, 1 }, 0);

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
            SDL_FRect rect = FGridRectToFRect(graphicsDataContext, (FGridRect){ (float)col, (float)row, 1, 1 }, 0);
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

bool DrawSidebar(GraphicsDataContext* graphicsDataContext, const Fonts* fonts, const GameDataContext* gameDataContext)
{
    // TODO Spruce up the sidebar with some textures, maybe some pixel art, some bounding boxes.

    // Draw sidebar background
    SDL_SetRenderDrawColor(graphicsDataContext->renderer, 20, 20, 20, 255); // Grey
    FGridRect gridRect = { ARENA_WIDTH, 0, (float)graphicsDataContext->sidebarUI->width, WINDOW_GRID_HEIGHT };
    const SDL_FRect backgroundRect = FGridRectToFRect(graphicsDataContext, gridRect, 0);
    if (!SDL_RenderRect(graphicsDataContext->renderer, &backgroundRect))
        return false;

    const SDL_Color colorWhite = { 255, 255, 255, 255 };
    gridRect = (FGridRect){ ARENA_WIDTH, 0, (float)graphicsDataContext->sidebarUI->width, 1 };

    static TextCache cache001 = { 0 };
    if (!RenderText(graphicsDataContext, gridRect, 0.1f, "TETRIS", &cache001, fonts->mainFont, colorWhite)) return false;

    // Draw score
    char text[MAX_STRING_LENGTH];
    if (SDL_snprintf(text, 8, "%06d", gameDataContext->score) < 0) return false;

    gridRect.y++;
    static TextCache cache002 = { 0 };
    if (!RenderText(graphicsDataContext, gridRect, 0.1f, text, &cache002, fonts->secondaryFont, colorWhite)) return false;

    // Draw level
    if (SDL_snprintf(text, 8, "LVL %03d", gameDataContext->level) < 0) return false;

    gridRect.y++;
    static TextCache cache003 = { 0 };
    if (!RenderText(graphicsDataContext, gridRect, 0.1f, text, &cache003, fonts->secondaryFont, colorWhite)) return false;

    if (!RenderButton(graphicsDataContext, &graphicsDataContext->sidebarUI->restartButton)) return false;

    if (gameDataContext->isPaused) {
        memcpy(&graphicsDataContext->sidebarUI->pauseButton.text, "RESUME", 7);
    }
    else {
        memcpy(&graphicsDataContext->sidebarUI->pauseButton.text, "PAUSE", 6);
    }
    if (!RenderButton(graphicsDataContext, &graphicsDataContext->sidebarUI->pauseButton)) return false;

    if (!RenderButton(graphicsDataContext, &graphicsDataContext->sidebarUI->quitButton)) return false;

    return true;
}

bool DrawGameOverScreen(GraphicsDataContext* graphicsDataContext, const Fonts* fonts, GameDataContext* gameDataContext)
{
    const SDL_Color colorWhite = { 255, 255, 255, 255 };

    // Draw menu background
    SDL_SetRenderDrawColor(graphicsDataContext->renderer, 10, 10, 10, 200); // Grey
    const SDL_FRect backgroundRect = FGridRectToFRect(graphicsDataContext, (FGridRect){ 0, 0, ARENA_WIDTH, ARENA_HEIGHT }, 0);
    if (!SDL_RenderFillRect(graphicsDataContext->renderer, &backgroundRect)) return false;

    // Draw title
    static TextCache cache001 = { 0 };
    if (!RenderText(graphicsDataContext, (FGridRect){ 0, 0, ARENA_WIDTH, ARENA_HEIGHT }, 0.5f, "GAME OVER", &cache001, fonts->mainFont, colorWhite)) return false;

    return true;
}

bool ResizeGridSquares(GraphicsDataContext* graphicsDataContext, const Sint32 windowWidth, const Sint32 windowHeight)
{
    const float widthBasedSize = (float)windowWidth / ((float)ARENA_WIDTH + (float)graphicsDataContext->sidebarUI->width);
    const float heightBasedSize = (float)windowHeight / (float)ARENA_HEIGHT;

    graphicsDataContext->gridSquareSize = (widthBasedSize < heightBasedSize) ? widthBasedSize : heightBasedSize;

    return true;
}

bool RenderText(GraphicsDataContext* graphicsDataContext, const FGridRect gridRect, const float margin, char* text, TextCache* cache, TTF_Font* font, const SDL_Color color)
{
    SDL_Texture* texture = GenerateTextTexture(graphicsDataContext, text, cache, font, color);
    if (!texture) return false;
    const float innerWidth = (gridRect.w - margin * 2) * graphicsDataContext->gridSquareSize;
    const float innerHeight = (gridRect.h - margin * 2) * graphicsDataContext->gridSquareSize;
    const float innerX = (gridRect.x + margin) * graphicsDataContext->gridSquareSize;
    const float innerY = (gridRect.y + margin) * graphicsDataContext->gridSquareSize;

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

bool RenderButton(GraphicsDataContext* graphicsDataContext, Button* button)
{
    const SDL_Color buttonColor = button->isHovered ? button->hoverColor : button->color;

    if (!SDL_SetRenderDrawColor(graphicsDataContext->renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a)) return false;
    const SDL_FRect rect = FGridRectToFRect(graphicsDataContext, button->gridRect, 0.1f);
    if (!SDL_RenderFillRect(graphicsDataContext->renderer, &rect)) return false;

    if (!RenderText(graphicsDataContext, button->gridRect, 0.25f, button->text, &button->cache, button->font, button->textColor)) return false;

    return true;
}

void HandleButtonEvent(GraphicsDataContext* graphicsDataContext, SDL_Event* event, Button* button)
{
    const SDL_FRect rect = FGridRectToFRect(graphicsDataContext, button->gridRect, 0);

    if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        button->isHovered = SDL_PointInRectFloat(&(SDL_FPoint) { event->motion.x, event->motion.y }, &rect);
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && button->isHovered)
    {
        button->isPressed = true;
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        // Callback
        if (button->isHovered && button->isPressed && button->onClick) button->onClick(button->userData);
        button->isPressed = false;
    }
}

SDL_FRect FGridRectToFRect(const GraphicsDataContext* graphicsDataContext, const FGridRect gridRect, const float margin)
{
    // TODO Check that margin*2 < width/height

    const SDL_FRect rect = {
    (gridRect.x + margin) * graphicsDataContext->gridSquareSize,
    (gridRect.y + margin) * graphicsDataContext->gridSquareSize,
    (gridRect.w - margin * 2) * graphicsDataContext->gridSquareSize,
    (gridRect.h - margin * 2) * graphicsDataContext->gridSquareSize
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