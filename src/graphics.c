#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#include "graphics.h"

#include "util.h"
#include "game.h"
#include "tetromino.h"

bool GFX_Init(GraphicsDataContext* graphicsDataContext, GameDataContext* gameDataContext, Fonts* fonts)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "Calling %s...", __func__);

    // Load fonts
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Loading fonts...");
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
        .gridRect = {(float)ARENA_WIDTH, 8, 3, 1},
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
        .gridRect = {(float)ARENA_WIDTH, 9, 3, 1},
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

    // Assert that enums are set correctly
    Assert(WINDOW_GRID_WIDTH >= (int)ARENA_WIDTH + sidebar->width, "Invalid window grid width!\n");
    Assert(WINDOW_GRID_HEIGHT >= (int)ARENA_HEIGHT, "Invalid window grid width!\n");

    // Initialise UI objects
    graphicsDataContext->sidebarUI = sidebar;

    // Default size of 60
    graphicsDataContext->gridSquareSize = 60;

    // Set default game size based on monitor resolution
    const SDL_DisplayID displayId = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* displayMode = SDL_GetDesktopDisplayMode(displayId);
    SDL_LogDebug(SDL_LOG_CATEGORY_VIDEO, "Setting default window resolution to: %dx%d", displayMode->w, displayMode->h);
    ResizeGridSquares(graphicsDataContext, (Sint32)(displayMode->w * 0.8), (Sint32)(displayMode->h * 0.8));

    const int width = (int)((float)WINDOW_GRID_WIDTH * graphicsDataContext->gridSquareSize);
    const int height = (int)((float)WINDOW_GRID_HEIGHT * graphicsDataContext->gridSquareSize);

    SDL_CreateWindowAndRenderer("TETRIS", width, height, SDL_WINDOW_RESIZABLE, &graphicsDataContext->window, &graphicsDataContext->renderer);

    const float ratio = (float)width / (float)height;
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calculated window aspect ratio as %f!", ratio);

    SDL_SetWindowAspectRatio(graphicsDataContext->window, ratio, ratio);
    SDL_SetRenderDrawBlendMode(graphicsDataContext->renderer, SDL_BLENDMODE_BLEND);

    Assert(graphicsDataContext->window, "Window creation failed!\n");
    Assert(graphicsDataContext->renderer, "Renderer creation failed!\n");

    return true;
} 

bool GFX_LoadTetrominoTextures(const GraphicsDataContext* graphicsDataContext)
{
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calling %s...", __func__);

    // Load tetromino textures
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Loading tetromino textures...");
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
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calling %s...", __func__);

    SDL_LogTrace(SDL_LOG_CATEGORY_RENDER, "Clearing screen...");
    SDL_SetRenderDrawColor(graphicsDataContext->renderer, 17, 17, 17, 255);
    SDL_RenderClear(graphicsDataContext->renderer);

    Assert(DrawDroppingTetromino(graphicsDataContext, gameDataContext), "Failed to draw dropping tetromino!\n");
    Assert(DrawDroppingTetrominoGhost(graphicsDataContext, gameDataContext), "Failed to draw dropping tetromino ghost!\n");
    Assert(DrawArena(graphicsDataContext, gameDataContext), "Failed to draw arena!\n");
    Assert(DrawSidebar(graphicsDataContext, fonts, gameDataContext), "Failed to draw sidebar!\n");

    if (gameDataContext->isGameOver)
    {
        SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "Detected game over...");
        if (!DrawGameOverScreen(graphicsDataContext, fonts, gameDataContext)) return false;
    }

    return true;
}


bool DrawBlock(GraphicsDataContext* graphicsDataContext, SDL_Texture* texture, const Uint8 alpha, const int x, const int y)
{
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calling %s...", __func__);

    if (!texture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Attempted to draw invalid block texture!");
        return false;
    }

    if (x >= ARENA_WIDTH || x < 0 || y >= ARENA_HEIGHT || y < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Attempted to draw a block outside of the alignment grid!");
        return false;
    }

    const SDL_FRect rect = FGridRectToFRect(graphicsDataContext, (FGridRect){ (float)x, (float)y, 1, 1 }, 0);

    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Drawing block on grid @ (%d, %d)...", x, y);
    if (!SDL_SetTextureAlphaMod(texture, alpha)) return false;
    if (!SDL_RenderTexture(graphicsDataContext->renderer, texture, NULL, &rect)) return false;
    return true;
}

bool DrawArena(GraphicsDataContext* graphicsDataContext, const GameDataContext* gameDataContext)
{
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calling %s...", __func__);

    for (int row = 0; row < ARENA_HEIGHT; row++)
    {
        for (int col = 0; col < ARENA_WIDTH; col++)
        {
            // Draw only filled blocks
            if (gameDataContext->arena[row][col])
            {
                const TetrominoShape* shape = GetTetrominoShapeByIdentifier(gameDataContext->arena[row][col]);
                DrawBlock(graphicsDataContext, shape->texture, 255, col, row);
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
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calling %s...", __func__);

    DroppingTetromino* droppingTetromino = gameDataContext->droppingTetromino;
    if (!droppingTetromino)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Attempted to draw invalid dropping tetromino!");
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
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calling %s...", __func__);

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
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calling %s...", __func__);

    // TODO Spruce up the sidebar with some textures, maybe some pixel art, some bounding boxes.
    // Draw sidebar background
    SDL_SetRenderDrawColor(graphicsDataContext->renderer, 20, 20, 20, 255); // Grey
    FGridRect gridRect = { ARENA_WIDTH, 0, (float)graphicsDataContext->sidebarUI->width, WINDOW_GRID_HEIGHT };
    const SDL_FRect backgroundRect = FGridRectToFRect(graphicsDataContext, gridRect, 0);
    if (!SDL_RenderRect(graphicsDataContext->renderer, &backgroundRect)) return false;

    const SDL_Color colorWhite = { 255, 255, 255, 255 };
    gridRect.h = 2;

    static TextCache cache001 = { 0 };
    if (!RenderText(graphicsDataContext, gridRect, 0.1f, "TETRIS", &cache001, fonts->mainFont, colorWhite)) return false;

    // Draw score
    char text[MAX_STRING_LENGTH];
    if (SDL_snprintf(text, 8, "%06d", gameDataContext->score) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to convert score into text!");
        return false;
    }

    gridRect.h = 1;
    gridRect.y += 2;
    static TextCache cache002 = { 0 };
    if (!RenderText(graphicsDataContext, gridRect, 0.1f, text, &cache002, fonts->secondaryFont, colorWhite)) return false;

    // Draw level
    if (SDL_snprintf(text, 8, "LVL %03d", gameDataContext->level) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to convert score into text!");
        return false;
    }

    gridRect.y++;
    static TextCache cache003 = { 0 };
    if (!RenderText(graphicsDataContext, gridRect, 0.1f, text, &cache003, fonts->secondaryFont, colorWhite)) return false;

    if (!RenderButton(graphicsDataContext, &graphicsDataContext->sidebarUI->restartButton)) return false;

    if (gameDataContext->isPaused) {
        SDL_LogTrace(SDL_LOG_CATEGORY_RENDER, "Changing pauseButton text to 'RESUME'...");
        memcpy(&graphicsDataContext->sidebarUI->pauseButton.text, "RESUME", 7);
    }
    else {
        SDL_LogTrace(SDL_LOG_CATEGORY_RENDER, "Changing pauseButton text to 'PAUSE'...");
        memcpy(&graphicsDataContext->sidebarUI->pauseButton.text, "PAUSE", 6);
    }
    if (!RenderButton(graphicsDataContext, &graphicsDataContext->sidebarUI->pauseButton)) return false;

    if (!RenderButton(graphicsDataContext, &graphicsDataContext->sidebarUI->quitButton)) return false;

    return true;
}

bool DrawGameOverScreen(GraphicsDataContext* graphicsDataContext, const Fonts* fonts, GameDataContext* gameDataContext)
{
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calling %s...", __func__);

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
    SDL_LogVerbose(SDL_LOG_CATEGORY_VIDEO, "Calling %s...", __func__);

    const float widthBasedSize = (float)windowWidth / ((float)ARENA_WIDTH + (float)graphicsDataContext->sidebarUI->width);
    const float heightBasedSize = (float)windowHeight / (float)ARENA_HEIGHT;

    graphicsDataContext->gridSquareSize = (widthBasedSize < heightBasedSize) ? widthBasedSize : heightBasedSize;
    SDL_LogDebug(SDL_LOG_CATEGORY_VIDEO, "Resizing grid squares to (%f) based on / relative to %s...", graphicsDataContext->gridSquareSize, (widthBasedSize < heightBasedSize) ? "width" : "height");

    return true;
}

bool RenderText(GraphicsDataContext* graphicsDataContext, const FGridRect gridRect, const float margin, char* text, TextCache* cache, TTF_Font* font, const SDL_Color color)
{
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calling %s...", __func__);

    SDL_Texture* texture = GenerateTextTexture(graphicsDataContext, text, cache, font, color);
    if (!texture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Generated text texture was invalid!");
        return false;
    }
    const float innerWidth = (gridRect.w - margin * 2) * graphicsDataContext->gridSquareSize;
    const float innerHeight = (gridRect.h - margin * 2) * graphicsDataContext->gridSquareSize;
    const float innerX = (gridRect.x + margin) * graphicsDataContext->gridSquareSize;
    const float innerY = (gridRect.y + margin) * graphicsDataContext->gridSquareSize;
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Scaled Inner Rect: x=%f, y=%f, w=%f, h=%f.", innerX, innerY, innerWidth, innerHeight);

    // Aspect ratio
    const float widthRatio = innerWidth / (float)texture->w;
    const float heightRatio = innerHeight / (float)texture->h;
    const float ratio = (widthRatio <= heightRatio) ? widthRatio : heightRatio;
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calculated text aspect ratio as %f!", ratio);

    const float scaledWidth = (float)texture->w * ratio;
    const float scaledHeight = (float)texture->h * ratio;

    // Center inside inner-rect
    const float centeredX = innerX + (innerWidth - scaledWidth) / 2;
    const float centeredY = innerY + (innerHeight - scaledHeight) / 2;

    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Centering text in scaled rect at: x=%f, y=%f.", centeredX, centeredY);

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
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calling %s...", __func__);

    const SDL_Color buttonColor = button->isHovered ? button->hoverColor : button->color;

    if (!SDL_SetRenderDrawColor(graphicsDataContext->renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a)) return false;
    const SDL_FRect rect = FGridRectToFRect(graphicsDataContext, button->gridRect, 0.1f);
    if (!SDL_RenderFillRect(graphicsDataContext->renderer, &rect)) return false;

    if (!RenderText(graphicsDataContext, button->gridRect, 0.25f, button->text, &button->cache, button->font, button->textColor)) return false;

    return true;
}

void HandleButtonEvent(GraphicsDataContext* graphicsDataContext, SDL_Event* event, Button* button)
{
    SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Calling %s...", __func__);

    const SDL_FRect rect = FGridRectToFRect(graphicsDataContext, button->gridRect, 0);

    if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        button->isHovered = SDL_PointInRectFloat(&(SDL_FPoint) { event->motion.x, event->motion.y }, &rect);
        if (button->isHovered) SDL_LogTrace(SDL_LOG_CATEGORY_RENDER, "Button (text=%s) is hovered!", button->text);
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && button->isHovered)
    {
        button->isPressed = true;
        SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "Button (text=%s) is pressed!", button->text);
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        // Callback
        if (button->isHovered && button->isPressed && button->onClick)
        {
            SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "Button (text=%s) has activated and called callback method!", button->text);
            button->onClick(button->userData);
        }
        button->isPressed = false;
    }
}

SDL_FRect FGridRectToFRect(const GraphicsDataContext* graphicsDataContext, const FGridRect gridRect, const float margin)
{
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calling %s...", __func__);

    Assert((margin * 2) < gridRect.w, "Invalid margin!\n");
    Assert((margin * 2) < gridRect.h, "Invalid margin!\n");

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
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Calling %s...", __func__);
    SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Cache request for TextCache object with text='%s'...", text);

    if (cache->valid && !strcmp(text, cache->text))
    {
        SDL_LogVerbose(SDL_LOG_CATEGORY_RENDER, "Cache hit! Returning cached texture...");
        return cache->texture;
    }

    // Cache miss
    SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "Cache miss! Generating new texture and cache object...");

    SDL_DestroyTexture(cache->texture);
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, 0, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(graphicsDataContext->renderer, surface);
    SDL_DestroySurface(surface);

    if (memcpy(cache->text, text, MAX_STRING_LENGTH) == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to copy new text ('%s') to cache object!", text);
        return NULL;
    }
    cache->texture = texture;
    cache->valid = true;

    return texture;
}