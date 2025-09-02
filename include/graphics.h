#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL3_ttf/SDL_ttf.h>

#include "util.h"
#include "game.h"

/**
 * @brief Generic graphics configuration enum values.
 */
enum GraphicsConfig
{
    /** @brief The width (in grid squares of size gridSquareSize) of the sidebar. */
    SIDEBAR_GRID_WIDTH = 3,

    /** @brief The width (in grid squares of size gridSquareSize) of the window. */
    WINDOW_GRID_WIDTH = 13,

    /** @brief The height (in grid squares of size gridSquareSize) of the window. */
    WINDOW_GRID_HEIGHT = 20,
};

/**
 * @brief A struct containing fonts.
 */
typedef struct Fonts
{
    TTF_Font* mainFont;
    TTF_Font* secondaryFont;
} Fonts;

/**
 * @brief A rectangle, on the grid, with the origin at the upper left (using floating point values).
 */
typedef struct FGridRect
{
    float x;
    float y;
    float w;
    float h;
} FGridRect;

/**
 * @brief A struct containing cache data for some text
 *
 * @details Useful when we are rendering constantly updating text, to avoid regenerating textures
 */
typedef struct TextCache
{
    /** @brief The text to cache. */
    char text[MAX_STRING_LENGTH];

    /** @brief A texture generated from the text in the cache. */
    SDL_Texture* texture;

    /** @brief Whether this cache object is valid, i.e. has a valid texture associated with some text */
    bool valid;

} TextCache;

/**
 * @brief The function to callback when a button is clicked.
 */
typedef void (*ButtonCallback)(void* userData);

/**
 * @brief A struct containing 
 */
typedef struct Button
{
    FGridRect gridRect;
    SDL_Color color;
    SDL_Color hoverColor;
    SDL_Color textColor;
    TTF_Font* font;
    char text[MAX_STRING_LENGTH];
    TextCache cache;

    bool isHovered;
    bool isPressed;

    ButtonCallback onClick;
    void* userData;

} Button;

/**
 * @brief A struct containing data pertaining to objects in the sidebar UI
 */
typedef struct SidebarUI
{
    /** @brief The width of the SidebarUI in grid alignment squares */
    int width;
    Button restartButton;
    Button pauseButton;
    Button quitButton;
} SidebarUI;

/**
 *  @brief A struct that holds the current graphics state: renderer, window and gridSquareSize.
 *
 *  @details It is designed for runtime state tracking.
 */
typedef struct GraphicsDataContext
{
    /** @brief A pointer to the SDL renderer used for GPU calls. */
    SDL_Renderer* renderer;

    /** @brief A pointer to the SDL window object. */
    SDL_Window* window;

    /**
     * @brief The current size (in pixels) of a single unit grid square.
     * @details We devise pixel-coordinates for all draw calls using this value, as it changes relative to the window size.
     */
    float gridSquareSize;

    /** @brief A pointer to a sidebar UI struct. */
    SidebarUI* sidebarUI;

} GraphicsDataContext;



/**
 * @brief Initialises the graphicsData values.
 *
 * @param graphicsDataContext A struct containing the graphics data to initialise.
 * @param gameDataContext A struct containing the game data context.
 * @param fonts A pointer to the fonts struct to load the fonts to.
 *
 * @return True on success, false otherwise.
 */
bool GFX_Init(GraphicsDataContext* graphicsDataContext, GameDataContext* gameDataContext, Fonts* fonts);

/**
 * @brief Loads resources into memory, including tetromino square textures
 * 
 * @param graphicsDataContext A struct containing the graphics data context.
 *
 * @return True on success, false otherwise.
 */
bool GFX_LoadTetrominoTextures(GraphicsDataContext* graphicsDataContext);

/**
 * @brief A wrapper function to render all graphics objects onto the window.
 * 
 * @param graphicsDataContext A struct containing the graphics data to initialise.
 * @param fonts A pointer to the fonts struct to load the fonts to.
 * @param gameDataContext A struct containing the game data context.
 *
 * @return True on success, false otherwise.
 */
bool GFX_RenderGame(GraphicsDataContext* graphicsDataContext, GameDataContext* gameDataContext, Fonts* fonts);

/**
 * @brief Draw a single block on the grid.
 *
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param texture A pointer to the texture of the block.
 * @param alpha The alpha of the texture of the block.
 * @param x The x coordinate in the grid.
 * @param y The y coordinate in the grid.
 *
 * @return True on success, false otherwise.
 */
bool DrawBlock(GraphicsDataContext* graphicsDataContext, SDL_Texture* texture, Uint8 alpha, int x, int y);


/**
 * @brief Draw the arena grid.
 *
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param gameDataContext A struct containing the game data context.
 *
 * @returns True on success, false otherwise.
 */
bool DrawArena(GraphicsDataContext* graphicsDataContext, GameDataContext* gameDataContext);

/**
 * @brief Draw an entire tetromino on the grid.
 *
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param gameDataContext A struct containing the game data context.
 *
 * @return True on success, false otherwise.
 */
bool DrawDroppingTetromino(GraphicsDataContext* graphicsDataContext, GameDataContext* gameDataContext);

/**
 * @brief Draw the ghost of an entire tetromino on the grid.
 *
 * @note A ghost is where the dropping tetromino would be placed if the user hard-dropped it at that point.
 * @note See "Ghost Piece": https://tetris.wiki/Tetris_Guideline
 *
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param gameDataContext A struct containing the game data context.
 *
 * @return True on success, false otherwise.
 */
bool DrawDroppingTetrominoGhost(GraphicsDataContext* graphicsDataContext, GameDataContext* gameDataContext);

/**
 * @brief Draw the sidebar next to the arena.
 *
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param gameDataContext A struct containing the game data context.
 * @param fonts A pointer to a struct of fonts to use.
 *
 * @return True on success, false otherwise.
 */
bool DrawSidebar(GraphicsDataContext* graphicsDataContext, GameDataContext* gameDataContext, Fonts* fonts);

/**
 * @brief Render a game over screen.
 * 
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param gameDataContext A struct containing the game data context.
 * @param fonts A pointer to a struct of fonts to use.
 *
 * @return True on success, false otherwise.
 */
bool DrawGameOverScreen(GraphicsDataContext* graphicsDataContext, GameDataContext* gameDataContext, Fonts* fonts);

/**
 * @brief Resizes the grid square (used as a standard alignment unit) based on what would fit in the given window size.
 *
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param windowWidth The new width of the game window.
 * @param windowHeight The new height of the game window.
 *
 * @return True on success, false otherwise.
 */
bool ResizeGridSquares(GraphicsDataContext* graphicsDataContext, Sint32 windowWidth, Sint32 windowHeight);

/**
 * @brief Render text centered within given bounds (specified in grid squares).
 * 
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param gridRect A rectangle representing the bounds of the text on the alignment grid
 * @param margin The margin of the generated text.
 * @param text The text to generate.
 * @param cache A persistent cache object.
 * @param font The font generate the text in.
 * @param color The color to generate the text in.
 *
 * @return True if success, false otherwise.
 */
bool RenderText(GraphicsDataContext* graphicsDataContext, FGridRect gridRect, float margin, char* text, TextCache* cache, TTF_Font* font, SDL_Color color);

/**
 * @brief Render a button object onto the screen.
 *
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param button A pointer to the button to render.
 * @return True if success, false otherwise.
 */
bool RenderButton(GraphicsDataContext* graphicsDataContext, Button* button);

/**
 * @brief Handle events pertaining to a given button object.
 *
 * @details This method will update the button states given the correct event has taken place, such as hovering or
 * pressing.
 *
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param event The event to be handled.
 * @param button A pointer to the button to handle events for.
 */
void HandleButtonEvent(GraphicsDataContext* graphicsDataContext, SDL_Event* event, Button* button);

/**
 * @brief Generate an SDL_FRect object based on a grid system that abstracts alignment and resizing.
 *
 * @note This method can take a fraction of a grid square as a location on the grid.
 *
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param gridRect A rectangle representing the bounds of the text on the alignment grid.
 * @param margin A margin to include within the newly generated FRect within the grid aligned rect.
 *
 * @return An SDL_FRect object
 */
SDL_FRect FGridRectToFRect(GraphicsDataContext* graphicsDataContext, FGridRect gridRect, float margin);

/**
 * @public
 * @brief A wrapper method to generate a texture for text. Utilises a caching system to avoid regenerating existing textures.
 *
 * @note This method will NOT generate a new texture if anything other than the text has changed, i.e. A new color
 * @note will not trigger a texture regeneration.
 *
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param text The text to generate.
 * @param cache A persistent cache object.
 * @param font The font generate the text in.
 * @param color The color to generate the text in.
 *
 * @return A pointer to an SDL_Texture object of the specified text, font and color.
 */
SDL_Texture* GenerateTextTexture(GraphicsDataContext* graphicsDataContext, char* text, TextCache* cache, TTF_Font* font, SDL_Color color);

#endif //GRAPHICS_H
