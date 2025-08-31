#ifndef GAME_H
#define GAME_H

#include "tetromino.h"

/**
 * @brief Generic game configuration enum values.
 */
enum GameConfig
{
    /** @brief */
    ARENA_WIDTH = 10,

    /** @brief */
    ARENA_HEIGHT = 20,
};



/** 
 *  @brief A struct that holds the current game state: score, level, arena and currently dropping tetromino.
 *
 *  @details It is designed for runtime state tracking.
 */
typedef struct GameDataContext
{
    /** @brief Player's current score. */
    int score;

    /** @brief Player's current level. */
    int level;

    /** @brief Whether the game is currently paused. */
    bool isPaused;

    /** @brief Whether the game is currently over. */
    bool isGameOver;

    /** @brief The state the of the tetris arena, where each cell is either empty or has a ::TetrominoIdentifier value. */
    TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH];

    /** @brief A pointer to the state of the currently dropping tetromino. */
    DroppingTetromino* droppingTetromino;

} GameDataContext;

/**
 * @brief Initialises the gameDataContext values.
 *
 * @note Must be called after textures are loaded with LoadResources()!
 *
 * @param gameDataContext A struct containing the game data to initialise.
 *
 * @return True on success, false otherwise.
 */
bool InitGameData(GameDataContext* gameDataContext);

/**
 * @brief The method handling all the game logic that should happen each frame.
 * 
 * @param gameDataContext A struct containing the game data context.
 */
void GameIteration(GameDataContext* gameDataContext);


/**
 * @brief Checks whether the dropping tetromino object would collide at some given orientation.
 *
 * @param gameDataContext A struct containing the game data context.
 * @param translationX The theoretical translation of the dropping tetromino on the X plane.
 * @param translationY The theoretical translation of the dropping tetromino on the Y plane.
 * @param rotationAmount The theoretical rotation amount of the dropping tetromino.
 *
 * @return True if the tetromino would collide, false otherwise.
 */
bool CheckDroppingTetrominoCollision(GameDataContext* gameDataContext,
                                     int translationX, int translationY,
                                     int rotationAmount);


/**
 * @brief Scan the board for rows that should be cleared, clear them, increment the score by an appropriate amount, and
 * then return the number of lines that were cleared.
 * 
 * @param gameDataContext A struct containing the game data context.
 * 
 * @return The number of rows that were simultaneously cleared.
 */
int ClearLines(GameDataContext* gameDataContext);

/**
 * @brief Drops every row in the arena above dropToRow by dropAmount.
 *
 * @param arena The matrix representation of the tetris arena.
 * @param dropToRow The row to drop to.
 * @param dropAmount The amount to drop the rows by.
 */
static void DropRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], int dropToRow, int dropAmount);

/**
 * @brief Writes the location of the dropping tetromino onto the arena, and then resets it's attributes,
 * essentially "spawning" a new one.
 *
 * @param gameDataContext A struct containing the game data context.
 */
void ResetDroppingTetromino(GameDataContext* gameDataContext);

/**
 * @brief Rotate a given dropping tetromino on the board using the Super Rotation System.
 *
 * @details This potentially involves translating the tetromino as well as rotating it, in order to allow the user more
 * freedom when rotating in tight spaces.
 *
 * @note See "Wall Kicks": https://tetris.wiki/Super_Rotation_System
 *
 * @param gameDataContext A struct containing the game data context.
 * @param rotationDirection The direction to rotate the tetromino, 1 for right, -1 for left. 
 *
 * @returns True on success, false otherwise.
 */
bool WallKickDroppingTetromino(GameDataContext* gameDataContext, int rotationDirection);

/**
 * @brief Hard Drop a tetromino.
 *
 * @note Hard dropping causes the tetromino to land instantly.
 * @note See "Hard Drop": https://tetris.wiki/Drop
 *
 * @param gameDataContext A struct containing the game data context.
 */
void HardDropTetromino(GameDataContext* gameDataContext);

/**
 * @brief Drop a tetromino a single grid square (provided it does not collide)
 *
 * @note See "Soft Drop": https://tetris.wiki/Drop
 *
 * @param gameDataContext A struct containing the game data context.
 */
void SoftDropTetromino(GameDataContext* gameDataContext);

/**
 * @brief Shift a tetromino either left or right (provided it does not collide)
 *
 * @param gameDataContext A struct containing the game data context.
 * @param translation A negative or positive value indicating how far to shift the tetromino left, or right, respectively.
 */
void ShiftTetromino(GameDataContext* gameDataContext, int translation);

#endif //GAME_H
