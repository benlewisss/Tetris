#include "game.h"

#include "util.h"
#include "tetromino.h"

bool InitGameData()
{
    gameData.score = 0;
    gameData.level = 1;

    return true;
}

void GameIteration(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH],
                   DroppingTetromino* droppingTetromino)
{
    // The time (in milliseconds) to drop a tetromino one cell (i.e. speed) for each of the tetris levels
    static Uint64 gravityValues[MAX_LEVEL] = {1000, 793, 618, 473, 355, 262, 190, 135, 94, 64, 43, 28, 18, 11, 7, 5, 4, 3, 2, 1};

    // Lock Down time - how long the player should have to move a tetromino around on the board once it has made contact with the ground
    static int lockDownTime = 500;

    // Number of lines cleared on a particular level
    static int levelLinesCleared = 0;

    // Check dropping tetromino is marked for termination and has passed Lock Down (See https://tetris.wiki/Tetris_Guideline#LockDown)
    if (droppingTetromino->terminationTime)
    {
        // If the tetromino is in Lock Down, but moves to a position where it can drop, then we don't want to reset it
        if (!CheckDroppingTetrominoCollision(arena, droppingTetromino, 0, 1, 0))
        {
            droppingTetromino->terminationTime = 0;
        }
        else if (SDL_GetTicks() > (droppingTetromino->terminationTime + lockDownTime))
        {
            ResetDroppingTetromino(arena, droppingTetromino);
        }
    }

    levelLinesCleared += ClearLines(arena);

    // Every n ticks, drop tetromino and run tetromino operations
    static Uint64 oldTick = 0;

    if (SDL_GetTicks() - oldTick >= gravityValues[gameData.level - 1])
    {
        if (levelLinesCleared >= 4)
        {
            levelLinesCleared = 0;
            if (gameData.level < MAX_LEVEL) gameData.level++;
            SDL_Log("NEW LEVEL %d", gameData.level);
        }

        SoftDropTetromino(arena, droppingTetromino);

        oldTick = SDL_GetTicks();
    }
}

bool CheckDroppingTetrominoCollision(const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH],
                                     const DroppingTetromino* droppingTetromino, int translationX, int translationY,
                                     const int rotationAmount)
{
    const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape->coordinates[(((
        droppingTetromino->orientation + rotationAmount) % 4) + 4) % 4];

    translationX += droppingTetromino->x;
    translationY += droppingTetromino->y;

    for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
    {
        for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
        {
            if (droppingTetrominoRotatedCoordinates[i][j] == false) continue;

            const int offsetX = translationX + j;
            const int offsetY = translationY + i;

            // Check if the tetromino has collided with the arena
            if (offsetX >= ARENA_WIDTH || offsetX < 0 || offsetY >= ARENA_HEIGHT || offsetY < 0) return true;

            // Check if the tetromino has collided with another tetromino on the board
            if (arena[offsetY][offsetX] != 0) return true;
        }
    }

    return false;
}

void ResetDroppingTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino)
{
    const int droppingTetrominoX = droppingTetromino->x;
    const int droppingTetrominoY = droppingTetromino->y;
    const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape->coordinates[droppingTetromino
        ->orientation];

    // Update the arena with the location of the tetromino where it has collided
    for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
    {
        for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
        {
            if (droppingTetrominoRotatedCoordinates[i][j] == false) continue;
            arena[droppingTetrominoY + i][droppingTetrominoX + j] = droppingTetromino->shape->identifier;
        }
    }

    droppingTetromino->x = (ARENA_WIDTH - 1) / 2;
    droppingTetromino->y = 0;
    droppingTetromino->orientation = NORTH;
    droppingTetromino->shape = GetRandomTetrominoShape();
    droppingTetromino->terminationTime = 0;
}

/* Drops everything above this row by drop amount
 *
*/
static void DropRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], const int dropToRow, const int dropAmount)
{
    // Start scanning for filled rows from bottom of arena
    for (int row = dropToRow; row > 0; row--)
    {
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

int ClearLines(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH])
{
    // Number of sequential rows that have been filled
    int numFilledRows = 0;

    // Sliding window technique
    int bottomPointer = ARENA_HEIGHT - 1;
    int topPointer = ARENA_HEIGHT - 2;

    while (topPointer < bottomPointer && topPointer >= 0)
    {
        int bottomPointerSquareCount = 0;
        int topPointerSquareCount = 0;

        for (int col = 0; col < ARENA_WIDTH; col++)
        {
            if (arena[bottomPointer][col] != 0) bottomPointerSquareCount++;
            if (arena[topPointer][col] != 0) topPointerSquareCount++;
        }

        if (topPointerSquareCount < ARENA_WIDTH && bottomPointerSquareCount < ARENA_WIDTH)
        {
            topPointer--;
            bottomPointer--;
            continue;
        }

        if (topPointerSquareCount >= ARENA_WIDTH)
        {
            topPointer--;
        }
        else
        {
            numFilledRows = bottomPointer - topPointer;
            break;
        }

        if (bottomPointerSquareCount < ARENA_WIDTH)
        {
            bottomPointer--;
        }
    }

    // Clear the cleared rows and drop the rows above
    for (int row = bottomPointer; row <= topPointer; row--)
    {
        memset(arena[row], 0, ARENA_WIDTH);
    }
    DropRows(arena, bottomPointer, bottomPointer - topPointer);

    // Scoring for different levels
    switch (numFilledRows)
    {
    case 1:
        gameData.score += 100 * (gameData.level);
        break;
    case 2:
        gameData.score += 300 * (gameData.level);
        break;
    case 3:
        gameData.score += 500 * (gameData.level);
        break;
    case 4:
        gameData.score += 800 * (gameData.level);
        break;
    default:
        break;
    }

    return numFilledRows;
}

bool WallKickRotateDroppingTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino,
                                     const int rotationDirection)
{
    // 2D array of coordinate pairs (3D) representing the Wall Kick Data (see https://tetris.wiki/Super_Rotation_System).
    // The first dimension is the orientation direction, the second dimension are one of the 5 tests, and the third dimension are the test coordinate pairs.
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

    // One of eight possible orientation state changes (In numerical order: NORTH->EAST, EAST->NORTH, EAST->SOUTH, SOUTH->EAST, SOUTH->WEST, WEST->SOUTH, WEST->NORTH, NORTH->WEST)
    int rotationStateChange = 0;

    switch (droppingTetromino->orientation)
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

    // Special case (The [I] tetromino(s) have differing Wall Kick data)
    if (droppingTetromino->shape->identifier == I)
    {
        for (int i = 0; i < 5; i++)
        {
            if (!CheckDroppingTetrominoCollision(arena,
                                                 droppingTetromino,
                                                 SPECIAL_WALL_KICK_DATA[rotationStateChange][i][0],
                                                 SPECIAL_WALL_KICK_DATA[rotationStateChange][i][1],
                                                 rotationDirection))
            {
                droppingTetromino->x += SPECIAL_WALL_KICK_DATA[rotationStateChange][i][0];
                droppingTetromino->y += SPECIAL_WALL_KICK_DATA[rotationStateChange][i][1];
                RotateDroppingTetromino(droppingTetromino, rotationDirection);
                return true;
            }
        }
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            if (!CheckDroppingTetrominoCollision(arena,
                                                 droppingTetromino,
                                                 WALL_KICK_DATA[rotationStateChange][i][0],
                                                 WALL_KICK_DATA[rotationStateChange][i][1],
                                                 rotationDirection))
            {
                droppingTetromino->x += WALL_KICK_DATA[rotationStateChange][i][0];
                droppingTetromino->y += WALL_KICK_DATA[rotationStateChange][i][1];
                RotateDroppingTetromino(droppingTetromino, rotationDirection);
                return true;
            }
        }
    }

    return false;
}

void HardDropTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino)
{
    while (!CheckDroppingTetrominoCollision(arena, droppingTetromino, 0, 1, 0))
    {
        gameData.score += 2;
        droppingTetromino->y++;
    }

    ResetDroppingTetromino(arena, droppingTetromino);
}

void SoftDropTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino)
{
    if (CheckDroppingTetrominoCollision(arena, droppingTetromino, 0, 1, 0))
    {
        if (droppingTetromino->terminationTime == 0) droppingTetromino->terminationTime = SDL_GetTicks();
    }
    else
    {
        gameData.score += 1;
        droppingTetromino->y++;
    }
}

void ShiftTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino, int translation)
{
    if (!CheckDroppingTetrominoCollision(arena, droppingTetromino, translation, 0, 0)) droppingTetromino->x += translation;
}