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

uint8_t arena[ARENA_WIDTH][ARENA_HEIGHT] = { 0 };
static DroppingTetromino dt;


bool game_iteration(SDL_Renderer* renderer, DroppingTetromino* dropping_tetromino);

bool is_dropping_tetromino_move_possible(const DroppingTetromino* tetromino, uint8_t x, uint8_t y);

bool has_dropping_tetromino_collided(const DroppingTetromino* tetromino);

void reset_dropping_tetromino(DroppingTetromino* tetromino);

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
    reset_dropping_tetromino(&dt);

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
            if (is_dropping_tetromino_move_possible(&dt, dt.x + 1, dt.y)) dt.x++;
            break;
        case SDLK_LEFT:
            if (is_dropping_tetromino_move_possible(&dt, dt.x - 1, dt.y)) dt.x--;
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

    game_iteration(state->renderer, &dt);

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



bool game_iteration(SDL_Renderer* renderer, DroppingTetromino* dropping_tetromino)
{
    const SDL_Color grey = { 32, 32, 32, 255 };

    draw_arena(renderer, grey, arena);

    // If the dropping tetromino in the previous iteration is marked for termination, that means we must replace it
    // with a new dropping tetromino, essentially "spawning" a new one
    if (dropping_tetromino->terminate == true) reset_dropping_tetromino(&dt);

    // Draw Tetromino
    draw_tetromino(renderer, dropping_tetromino->shape, dropping_tetromino->x, dropping_tetromino->y);

    static Uint64 oldTick = 0;

    // Every 1000 ticks, drop tetromino
    if (SDL_GetTicks() - oldTick >= 500)
    {
        dropping_tetromino->y++;
        oldTick = SDL_GetTicks();
    }

    if (has_dropping_tetromino_collided(&dt) == true)
    {
        SDL_Log("COLLISION");

        // Update the arena with the location of the tetromino where it has collided
        for (int i = 0; i <= (TETROMINO_SIZE - 1) * 2; i += 2)
        {
            uint8_t offset_x = dropping_tetromino->x + dropping_tetromino->shape.offsets[i];
            uint8_t offset_y = dropping_tetromino->y + dropping_tetromino->shape.offsets[i + 1];

            arena[offset_x][offset_y] != 0;
        }
        dt.terminate = true;
    }

    // Check if dropping termino has connected with ground or another block, and mark for termination

    // Check if a row is filled and destroy it, increment score

    return true;
}

bool is_dropping_tetromino_move_possible(const DroppingTetromino* dropping_tetromino, const uint8_t x, const uint8_t y)
{
    // Iterate over every other offset in the tetromino shape (to differentiate x, y coords)
    for (int i = 0; i <= (TETROMINO_SIZE - 1) * 2; i += 2)
    {
        uint8_t offset_x = x + dropping_tetromino->shape.offsets[i];

        // Check if the tetromino has collided with the arena
        if (offset_x >= ARENA_WIDTH || offset_x < 0) return false;
    }

    return true;
}

bool has_dropping_tetromino_collided(const DroppingTetromino* dropping_tetromino)
{
    // Iterate over every other offset in the tetromino shape (to differentiate x, y coords)
    for (int i = 0; i <= (TETROMINO_SIZE - 1) * 2; i += 2)
    {
        uint8_t offset_x = dropping_tetromino->x + dropping_tetromino->shape.offsets[i];
        uint8_t offset_y = dropping_tetromino->y + dropping_tetromino->shape.offsets[i + 1];

        // Check if the tetromino has collided with the arena
        if (offset_y >= ARENA_HEIGHT || offset_y < 0) return true;

        // Check if the tetromino has collided with another tetromino on the board
        if (arena[offset_x][offset_y] != 0) return true;
    }

    return false;
}

void reset_dropping_tetromino(DroppingTetromino* tetromino)
{
    dt.x = 5;
    dt.y = 1;
    dt.rotation = 0;
    dt.shape = get_random_tetromino_shape();
    dt.terminate = false;
}