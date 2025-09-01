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

} GraphicsDataContext;

/**
 * @brief A struct containing fonts.
 */
typedef struct Fonts
{
    TTF_Font* mainFont;
    TTF_Font* secondaryFont;
} Fonts;

/**
 * @brief A struct containing textures.
 */
typedef struct Textures
{
    SDL_Texture* titleTexture;
} Textures;

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

typedef struct Button
{
    SDL_Rect bounds;
} Button;

/**
 * @brief Initialises the graphicsData values.
 *
 * @param graphicsDataContext A struct containing the graphics data to initialise.
 *
 * @return True on success, false otherwise.
 */
bool InitGraphicsData(GraphicsDataContext* graphicsDataContext);

/**
 * @brief Loads resources into memory, including tetromino square textures, fonts, and const text.
 * 
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param fonts A pointer to the fonts struct to load the fonts to.
 * @param textures A pointer to the textures struct to load the textures to.
 *
 * @return True on success, false otherwise.
 */
bool LoadResources(GraphicsDataContext* graphicsDataContext, Fonts* fonts, Textures* textures);

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
 * @param fonts A pointer to a struct of fonts to use.
 * @param textures A pointer to the struct of textures to use.
 * @param gameDataContext A struct containing the game data context.
 *
 * @return True on success, false otherwise.
 */
bool DrawSidebar(GraphicsDataContext* graphicsDataContext, Fonts* fonts, Textures* textures, GameDataContext* gameDataContext);

/** TODO Fill out
 * 
 * @param graphicsDataContext 
 * @param fonts 
 * @param textures 
 * @param gameDataContext 
 * @return 
 */
bool DrawGameOverScreen(GraphicsDataContext* graphicsDataContext, Fonts* fonts, Textures* textures, GameDataContext* gameDataContext);

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
 * @param x The x-coordinate on the grid system of the rect.
 * @param y The y-coordinate on the grid system of the rect.
 * @param width The width of the rect, in grid squares.
 * @param height The height of the rect, in grid squares.
 * @param margin The margin of the generated text.
 * @param text The text to generate.
 * @param cache A persistent cache object.
 * @param font The font generate the text in.
 * @param color The color to generate the text in.
 *
 * @return True if success, false otherwise.
 */
bool RenderText(GraphicsDataContext* graphicsDataContext, float x, float y, float width, float height, float margin, char* text, TextCache* cache, TTF_Font* font, SDL_Color color);

/**
 * @brief Generate an SDL_FRect object based on a grid system that abstracts alignment and resizing.
 *
 * @note This method can take a fraction of a grid square as a location on the grid.
 *
 * @param graphicsDataContext A struct containing the graphics data context.
 * @param x The x-coordinate on the grid system of the rect.
 * @param y The y-coordinate on the grid system of the rect.
 * @param width The width of the rect, in grid squares.
 * @param height The height of the rect, in grid squares.
 *
 * @return A pointer to an SDL_FRect object
 */
SDL_FRect GenerateRect(GraphicsDataContext* graphicsDataContext, float x, float y, float width, float height);

/**
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
