#include "game.h"

#include "util.h"
#include "tetromino.h"

bool InitGameData(GameDataContext* gameDataContext)
{
    // Init array
    memset(gameDataContext->arena, 0, sizeof(gameDataContext->arena[0][0]) * ARENA_HEIGHT * ARENA_WIDTH);

    // Initialise the first dropping tetromino AFTER textures are loaded
    DroppingTetromino* droppingTetromino = SDL_calloc(1, sizeof(DroppingTetromino));
    if (!droppingTetromino) return false;

    droppingTetromino->shape = GetRandomTetrominoShape();
    if (droppingTetromino->shape->identifier == I)
    {
        droppingTetromino->y = -1;
    }
    else
    {
        droppingTetromino->y = 0;
    }

    droppingTetromino->x = ((ARENA_WIDTH - TETROMINO_MAX_SIZE / 2) - 1) / 2;
    droppingTetromino->orientation = NORTH;
    droppingTetromino->terminationTick = 0;

    gameDataContext->score = 0;
    gameDataContext->level = 1;
    gameDataContext->droppingTetromino = droppingTetromino;
    return true;
}

void GameIteration(GameDataContext* gameDataContext)
{
    // The time (in milliseconds) to drop a tetromino one cell (i.e. speed) for each of the tetris levels
    static Uint64 gravityValues[MAX_LEVEL] = {1000, 793, 618, 473, 355, 262, 190, 135, 94, 64, 43, 28, 18, 11, 7, 5, 4, 3, 2, 1};

    // Lock down time (in milliseconds) - how long the player should have to move a tetromino around on the board once
    // it has made contact with the ground
    static int lockDownTime = 500;

    // Number of lines cleared on a particular level
    static int levelLinesCleared = 0;
    levelLinesCleared += ClearLines(gameDataContext);
    

    // Check dropping tetromino is marked for termination and has passed Lock Down (See https://tetris.wiki/Tetris_Guideline#LockDown)
    if (gameDataContext->droppingTetromino->terminationTick)
    {
        // If the tetromino is in Lock Down, but moves to a position where it can drop, then we cancel the Lock Down
        if (!WillDroppingTetrominoCollide(gameDataContext, 0, 1, 0))
        {
            gameDataContext->droppingTetromino->terminationTick = 0;
        }
        else if (SDL_GetTicks() > (gameDataContext->droppingTetromino->terminationTick + lockDownTime))
        {
            ResetDroppingTetromino(gameDataContext);
        }
    }

    // Based on gravity, every n-ticks, drop tetromino and run tetromino operations
    static Uint64 oldTick = 0;
    if (SDL_GetTicks() - oldTick >= gravityValues[gameDataContext->level - 1])
    {
        if (levelLinesCleared >= 10)
        {
            levelLinesCleared = 0;
            if (gameDataContext->level - 1 < MAX_LEVEL) gameDataContext->level++;
        }

        SoftDropTetromino(gameDataContext);

        oldTick = SDL_GetTicks();
    }
}

bool WillDroppingTetrominoCollide(const GameDataContext* gameDataContext,
                                     int translationX, 
                                     int translationY,
                                     const int rotationAmount)
{
    const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = gameDataContext->droppingTetromino->shape->coordinates[((gameDataContext->droppingTetromino->orientation + rotationAmount) % 4 + 4) % 4];

    translationX += gameDataContext->droppingTetromino->x;
    translationY += gameDataContext->droppingTetromino->y;

    for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
    {
        for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
        {
            if (!droppingTetrominoRotatedCoordinates[i][j]) continue;

            const int offsetX = translationX + j;
            const int offsetY = translationY + i;

            // Check if the tetromino has collided with the arena
            if (offsetX >= ARENA_WIDTH || offsetX < 0 || offsetY >= ARENA_HEIGHT || offsetY < 0) return true;

            // Check if the tetromino has collided with another tetromino on the board
            if (gameDataContext->arena[offsetY][offsetX] != 0) return true;
        }
    }

    return false;
}

void ResetDroppingTetromino(GameDataContext* gameDataContext)
{
    const int droppingTetrominoX = gameDataContext->droppingTetromino->x;
    const int droppingTetrominoY = gameDataContext->droppingTetromino->y;
    const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = gameDataContext->droppingTetromino->shape->coordinates[gameDataContext->droppingTetromino->orientation];

    // Update the arena with the location of the tetromino where it has collided
    for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
    {
        for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
        {
            if (!droppingTetrominoRotatedCoordinates[i][j]) continue;
            gameDataContext->arena[droppingTetrominoY + i][droppingTetrominoX + j] = gameDataContext->droppingTetromino->shape->identifier;
        }
    }

    gameDataContext->droppingTetromino->shape = GetTetrominoShapeByIdentifier(I);//GetRandomTetrominoShape();
    // TODO Find a better way to do this: The I-Piece is the only piece who's initial starting location is offset
    // downwards by one, so we do this in order to spawn the piece flush with the ceiling.
    if (gameDataContext->droppingTetromino->shape->identifier == I)
    {
        gameDataContext->droppingTetromino->y = -1;
    }
    else
    {
        gameDataContext->droppingTetromino->y = 0;
    }
    gameDataContext->droppingTetromino->x = ((ARENA_WIDTH - TETROMINO_MAX_SIZE / 2) - 1) / 2;
    gameDataContext->droppingTetromino->orientation = NORTH;
   
    gameDataContext->droppingTetromino->terminationTick = 0;
}

static void DropRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], const int dropToRow, const int dropAmount)
{
    // Start scanning for filled rows upwards from dropToRow
    for (int row = dropToRow; row >= 0; row--)
    {
        // Any rows at the top of the arena must be set to zero rather than filled with blocks above
        // them (as there are none).
        if (row - dropAmount < 0)
        {
            memset(arena[row], 0, ARENA_WIDTH * sizeof(arena[0][0]));
            continue;
        }

        int squareCount = 0;
        for (int col = 0; col < ARENA_WIDTH; col++)
        {
            if (arena[row][col] != 0) squareCount++;
            arena[row][col] = arena[row - dropAmount][col];
        }

        // If we reach an empty row, then nothing above it to drop down
        if (squareCount == 0) break;
    }
}

int ClearLines(GameDataContext* gameDataContext)
{
    // Number of sequential rows that have been filled
    int numFilledRows = 0;

    // Sliding window technique
    int bottomPointer = ARENA_HEIGHT - 1; // Points to the bottom
    int topPointer = ARENA_HEIGHT - 2; // Points to the penultimate bottom row

    while (topPointer >= 0 && topPointer < bottomPointer)
    {
        int topPointerSquareCount = 0;
        int bottomPointerSquareCount = 0;

        for (int col = 0; col < ARENA_WIDTH; col++)
        {
            if (gameDataContext->arena[topPointer][col] != 0) topPointerSquareCount++;
            if (gameDataContext->arena[bottomPointer][col] != 0) bottomPointerSquareCount++;
        }

        const bool topRowFilled = (topPointerSquareCount >= ARENA_WIDTH);
        const bool bottomRowFilled = (bottomPointerSquareCount >= ARENA_WIDTH);

        if (!topRowFilled && !bottomRowFilled)
        {
            // Neither row is full so we move both upwards
            topPointer--;
            bottomPointer--;
            continue;
        }

        if (topRowFilled)
        {
            // Top row is full so we move the top upwards
            topPointer--;
        }
        else
        {
            // Top row is not full but bottom row is full so we found our range
            numFilledRows = bottomPointer - topPointer;
            break;
        }

        if (!bottomRowFilled)
        {
            // Bottom row is not full so we move bottom upwards
            bottomPointer--;
        }
    }

    if (bottomPointer == 0) return 0;

    // Clear the cleared rows and drop the rows above
    SDL_Log("Dropping Rows - Drop to: %d, Drop by: %d", bottomPointer, numFilledRows);
    DropRows(gameDataContext->arena, bottomPointer, numFilledRows);

    // Scoring for different levels
    switch (numFilledRows)
    {
    case 1:
        gameDataContext->score += 100 * (gameDataContext->level);
        break;
    case 2:
        gameDataContext->score += 300 * (gameDataContext->level);
        break;
    case 3:
        gameDataContext->score += 500 * (gameDataContext->level);
        break;
    case 4:
        gameDataContext->score += 800 * (gameDataContext->level);
        break;
    default:
        break;
    }

    return numFilledRows;
}

bool WallKickDroppingTetromino(GameDataContext* gameDataContext, const int rotationDirection)
{
    // 2D array of coordinate pairs (3D) representing the Wall Kick Data (see https://tetris.wiki/Super_Rotation_System).
    // The first dimension is the orientation direction, the second dimension are one of the 5 tests, and the third
    // dimension are the test coordinate pairs.
    static const int8_t WALL_KICK_DATA[8][5][2] = {
        {{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
        {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},
        {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
    };

    static const int8_t SPECIAL_WALL_KICK_DATA[8][5][2] = {
        {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}},
        {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},
        {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}},
        {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}},
        {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},
        {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}},
        {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}},
        {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}},
    };

    // Valid parameter checks
    if (!(rotationDirection == -1 || rotationDirection == 1)) return false;

    // One of eight possible orientation state changes
    // (In numerical order: NORTH->EAST, EAST->NORTH, EAST->SOUTH, SOUTH->EAST, SOUTH->WEST, WEST->SOUTH, WEST->NORTH, NORTH->WEST)
    int rotationStateChange = 0;

    switch (gameDataContext->droppingTetromino->orientation)
    {
    case NORTH:
        if (rotationDirection == -1) rotationStateChange = 7;
        if (rotationDirection == 1) rotationStateChange = 0;
        break;
    case EAST:
        if (rotationDirection == -1) rotationStateChange = 1;
        if (rotationDirection == 1) rotationStateChange = 2;
        break;
    case SOUTH:
        if (rotationDirection == -1) rotationStateChange = 3;
        if (rotationDirection == 1) rotationStateChange = 4;
        break;
    case WEST:
        if (rotationDirection == -1) rotationStateChange = 5;
        if (rotationDirection == 1) rotationStateChange = 6;
        break;
    }

    // Pick the correct wall kick data set (I-piece has a special case)
    const int8_t (*wallKickData)[5][2] = (gameDataContext->droppingTetromino->shape->identifier == I)
        ? SPECIAL_WALL_KICK_DATA
        : WALL_KICK_DATA;

    for (int i = 0; i < 5; i++)
    {
        const int8_t dx = wallKickData[rotationStateChange][i][0];
        const int8_t dy = wallKickData[rotationStateChange][i][1];

        if (!WillDroppingTetrominoCollide(gameDataContext, dx, dy, rotationDirection))
        {
            gameDataContext->droppingTetromino->x += dx;
            gameDataContext->droppingTetromino->y += dy;
            RotateDroppingTetromino(gameDataContext->droppingTetromino, rotationDirection);
            return true;
        }
    }

    return false;
}

void HardDropTetromino(GameDataContext* gameDataContext)
{
    while (!WillDroppingTetrominoCollide(gameDataContext, 0, 1, 0))
    {
        gameDataContext->score += 2;
        gameDataContext->droppingTetromino->y++;
    }

    ResetDroppingTetromino(gameDataContext);
}

void SoftDropTetromino(GameDataContext* gameDataContext)
{
    if (WillDroppingTetrominoCollide(gameDataContext, 0, 1, 0))
    {
        if (gameDataContext->droppingTetromino->terminationTick == 0) gameDataContext->droppingTetromino->terminationTick = SDL_GetTicks();
    }
    else
    {
        gameDataContext->score += 1;
        gameDataContext->droppingTetromino->y++;
    }
}

void ShiftTetromino(GameDataContext* gameDataContext, const int translation)
{
    if (!WillDroppingTetrominoCollide(gameDataContext, translation, 0, 0)) gameDataContext->droppingTetromino->x += translation;
}