#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/util.h"
#include "../include/tetromino.h"
#include "../include/graphics.h"

static const struct
{
    const char* key;
    const char* value;
} extended_metadata[] =
{
    {SDL_PROP_APP_METADATA_CREATOR_STRING, "@benlewisss"},
    {SDL_PROP_APP_METADATA_COPYRIGHT_STRING, "MIT"},
    {SDL_PROP_APP_METADATA_TYPE_STRING, "Tetris"}
};

typedef struct
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool is_running;
} AppState;

static TetrominoIdentifier g_arena[ARENA_HEIGHT][ARENA_WIDTH] = {{0}};
static DroppingTetromino g_dropping_tetromino;

bool game_iteration(SDL_Renderer* renderer, DroppingTetromino* dropping_tetromino, TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH]);

bool check_dropping_tetromino_collision(const DroppingTetromino* dropping_tetromino, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], const int8_t x, const int8_t y);

void reset_dropping_tetromino(DroppingTetromino* tetromino);

uint16_t clear_filled_rows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH]);

/* Drops everything above this row by one
 *
*/
void drop_rows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], int drop_to_row);

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_Log("Tetris Initialisation");

    // Setup application metadata
    if (!SDL_SetAppMetadata("TETRIS", "1.0", "Tetris"))
    {
        return SDL_APP_FAILURE;
    }

    for (size_t i = 0; i < SDL_arraysize(extended_metadata); i++)
    {
        if (!SDL_SetAppMetadataProperty(extended_metadata[i].key, extended_metadata[i].value))
        {
            return SDL_APP_FAILURE;
        }
    }

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        // SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't initialize SDL!", SDL_GetError(), NULL);
        return SDL_APP_FAILURE;
    }

    AppState* state = SDL_calloc(1, sizeof(AppState));
    if (!state) return SDL_APP_FAILURE;

    SDL_CreateWindowAndRenderer("TETRIS", ARENA_WIDTH * BLOCK_SIZE, ARENA_HEIGHT * BLOCK_SIZE, false, &state->window, &state->renderer);
    if (!state->window || !state->renderer)
    {
        SDL_Log("Window or Renderer creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(state->window);
        return SDL_APP_FAILURE;
    }

    state->is_running = true;
    *appstate = state;

    // TETRIS Init
    // Initialise the first dropping tetromino
    reset_dropping_tetromino(&g_dropping_tetromino);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    AppState* state = appstate;

    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        state->is_running = false;
        break;

    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        SDL_Log("Window close requested");
        state->is_running = false;
        break;

    case SDL_EVENT_KEY_DOWN:

        switch (event->key.key)
        {
        case SDLK_RIGHT:
            if (!check_dropping_tetromino_collision(&g_dropping_tetromino, g_arena, g_dropping_tetromino.x + 1, g_dropping_tetromino.y)) g_dropping_tetromino.x++;
            break;
        case SDLK_LEFT:
            if (!check_dropping_tetromino_collision(&g_dropping_tetromino, g_arena, g_dropping_tetromino.x - 1, g_dropping_tetromino.y)) g_dropping_tetromino.x--;
            break;
        case SDLK_UP: // TODO There's a BIG bug where you can rotate at the edges and clip into the sides
            if (!check_dropping_tetromino_collision(&g_dropping_tetromino, g_arena, g_dropping_tetromino.x + 1, g_dropping_tetromino.y)) RotateDroppingTetromino(&g_dropping_tetromino, 1);
            break;
        case SDLK_DOWN:
            if (!check_dropping_tetromino_collision(&g_dropping_tetromino, g_arena, g_dropping_tetromino.x - 1, g_dropping_tetromino.y)) RotateDroppingTetromino(&g_dropping_tetromino, -1);
            break;
        default:
            break;
        }

        if (event->key.key == SDLK_ESCAPE)
        {
            SDL_Log("ESC pressed, exiting");
            state->is_running = false;
        }

        else
        {
            SDL_Log("Key: %s", SDL_GetKeyName(event->key.key));
        }
        break;

    case SDL_EVENT_WINDOW_RESIZED:
        SDL_Log("Window Resized to %d x %d", event->window.data1, event->window.data2);
        break;

    default:
        break;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    const AppState* state = (AppState*)appstate;

    // Clear screen
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->renderer);

    game_iteration(state->renderer, &g_dropping_tetromino, g_arena);

    // int width, height;
    // SDL_GetWindowSize(state->window, &width, &height);

    SDL_RenderPresent(state->renderer);

    return state->is_running ? SDL_APP_CONTINUE : SDL_APP_SUCCESS; // return SDL_APP_SUCCESS to quit
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    SDL_Log("App Quit");

    if (appstate != NULL)
    {
        AppState* state = (AppState*)appstate;
        if (state->renderer) SDL_DestroyRenderer(state->renderer);
        if (state->window) SDL_DestroyWindow(state->window);
        SDL_free(state);
    }
}

bool game_iteration(SDL_Renderer* renderer, DroppingTetromino* dropping_tetromino, TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH])
{
    DrawArena(renderer, arena);

    DrawDroppingTetromino(renderer, dropping_tetromino);

    // Every n ticks, drop tetromino and run tetromino operations
    const uint16_t speed = 200;
    static Uint64 old_tick = 0;

    if (SDL_GetTicks() - old_tick >= speed)
    {
        clear_filled_rows(arena);

        // Check if dropping tetromino has connected with ground or another block, and mark for termination
        if (check_dropping_tetromino_collision(dropping_tetromino, arena, dropping_tetromino->x, dropping_tetromino->y + 1) == true)
        {
            // Update the arena with the location of the tetromino where it has collided
            for (int i = 0; i <= (TETROMINO_SIZE - 1) * 2; i += 2)
            {
                const int offset_x = dropping_tetromino->x + dropping_tetromino->shape.offsets[dropping_tetromino->rotation][i];
                const int offset_y = dropping_tetromino->y + dropping_tetromino->shape.offsets[dropping_tetromino->rotation][i + 1];

                arena[offset_y][offset_x] = dropping_tetromino->shape.identifier; // TODO This can assign an ENUM, 0-7, of the index in some array somewhere corresponding to an RGB value
            }

            // If the dropping tetromino in the previous iteration is marked for termination, that means we must replace it
            // with a new dropping tetromino, essentially "spawning" a new one
            reset_dropping_tetromino(dropping_tetromino);
            return true;
        }

        dropping_tetromino->y++;

        old_tick = SDL_GetTicks();
    }

    // Check if a row is filled and destroy it, increment score

    return true;
}

bool check_dropping_tetromino_collision(const DroppingTetromino* dropping_tetromino, const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], const int8_t x, const int8_t y)
{
    SDL_Log("DT @(%d,%d): Block(%d,%d,%d,%d,%d,%d,%d,%d)",
        dropping_tetromino->x,
        dropping_tetromino->y,
        dropping_tetromino->x + dropping_tetromino->shape.offsets[dropping_tetromino->rotation][0],
        dropping_tetromino->y + dropping_tetromino->shape.offsets[dropping_tetromino->rotation][1],
        dropping_tetromino->x + dropping_tetromino->shape.offsets[dropping_tetromino->rotation][2],
        dropping_tetromino->y + dropping_tetromino->shape.offsets[dropping_tetromino->rotation][3],
        dropping_tetromino->x + dropping_tetromino->shape.offsets[dropping_tetromino->rotation][4],
        dropping_tetromino->y + dropping_tetromino->shape.offsets[dropping_tetromino->rotation][5],
        dropping_tetromino->x + dropping_tetromino->shape.offsets[dropping_tetromino->rotation][6],
        dropping_tetromino->y + dropping_tetromino->shape.offsets[dropping_tetromino->rotation][7]);

    // Iterate over every other offset in the tetromino shape (to differentiate x, y coords)
    for (int i = 0; i <= (TETROMINO_SIZE - 1) * 2; i += 2)
    {
        const int8_t offset_x = x + dropping_tetromino->shape.offsets[dropping_tetromino->rotation][i];
        const int8_t offset_y = y + dropping_tetromino->shape.offsets[dropping_tetromino->rotation][i + 1];

        // Check if the tetromino has collided with the arena
        if (offset_x >= ARENA_WIDTH || offset_x < 0 || offset_y >= ARENA_HEIGHT || offset_y < 0) return true;

        // Check if the tetromino has collided with another tetromino on the board
        if (arena[offset_y][offset_x] != 0)
        {
            SDL_Log("Block @(%d,%d) collision!", offset_x, offset_y);
            return true;
        }
    }

    return false;
}

void reset_dropping_tetromino(DroppingTetromino* tetromino)
{
    tetromino->x = (ARENA_WIDTH - 1) / 2;
    tetromino->y = 0;
    tetromino->rotation = NORTH;
    tetromino->shape = *GetRandomTetrominoShape();
    tetromino->terminate = false;
}

uint16_t clear_filled_rows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH])
{

    // Start scanning for filled rows from bottom of arena
	for (int row = ARENA_HEIGHT-1; row >= 0; row--)
	{
        // The number of grid squares that contain part of a tetromino in them
        int grid_fill_count = 0;
        for (int col = 0; col < ARENA_WIDTH; col++)
        {
            if (arena[row][col] != 0) grid_fill_count++;
        }

        // If we encounter an empty row while scanning upwards, then there can't be a filled row above that point
        if (grid_fill_count == 0)
        {
            break;
        }

        if (grid_fill_count >= ARENA_WIDTH)
        {
            // Clear row
            memset(arena[row], 0, ARENA_WIDTH);
            drop_rows(arena, row);
        }
		// TODO Maybe change the colour of the row 1 frame before we delete it, as some sort of animation?
	}

    return 0;
}

void drop_rows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], int drop_to_row)
{
    // Start scanning for filled rows from bottom of arena
    for (int row = drop_to_row; row > 0; row--)
    {
        for (int col = 0; col < ARENA_WIDTH; col++)
        {
            arena[row][col] = arena[row-1][col];
        }
    }
}