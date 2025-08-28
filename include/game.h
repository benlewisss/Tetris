#ifndef GAME_H
#define GAME_H

#include "util.h"
#include "tetromino.h"

struct
{
    int score;
    int level;
} gameData;

/**
 * Initialises the gameData values.
 *
 * @return True on success, false otherwise.
 */
bool InitGameData(void);

/**
 * The method handling all the game logic that should happen each frame.
 * 
 * @param arena The matrix representation of the tetris arena.
 * @param droppingTetromino A pointer to the dropping tetromino object.
 */
void GameIteration(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino);


/**
 * Checks whether the dropping tetromino object would collide at some given orientation.
 *
 * @param arena The matrix representation of the tetris arena.
 * @param droppingTetromino A pointer to the dropping tetromino object.
 * @param translationX The theoretical translation of the dropping tetromino on the X plane.
 * @param translationY The theoretical translation of the dropping tetromino on the Y plane.
 * @param rotationAmount The theoretical rotation amount of the dropping tetromino.
 *
 * @return True if the tetromino would collide, false otherwise.
 */
bool CheckDroppingTetrominoCollision(const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH],
                                     const DroppingTetromino* droppingTetromino, 
                                     int translationX, int translationY,
                                     int rotationAmount);


/**
 * Scan the board for rows that should be cleared, clear them, increment the score by an appropriate amount, and
 * then return the number of lines that were cleared.
 * 
 * @param arena The matrix representation of the tetris arena.
 * 
 * @return The number of rows that were simultaneously cleared.
 */
int ClearLines(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH]);

/**
 * Drops every row in the arena above dropToRow by dropAmount.
 *
 * @param arena The matrix representation of the tetris arena.
 * @param dropToRow The row to drop to.
 * @param dropAmount The amount to drop the rows by.
 */
static void DropRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], int dropToRow, int dropAmount);

/**
 * Writes the location of the dropping tetromino onto the arena, and then resets it's attributes,
 * essentially "spawning" a new one.
 *
 * @param arena The matrix representation of the tetris arena.
 * @param droppingTetromino A pointer to the dropping tetromino object.
 */
void ResetDroppingTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino);

/**
 * Rotate a given dropping tetromino on the board using the Super Rotation System.
 *
 * This potentially involves translating the tetromino as well as rotating it, in order to allow the user more freedom
 * when rotating in tight spaces.
 *
 * @note See "Wall Kicks": https://tetris.wiki/Super_Rotation_System
 *
 * @param arena The matrix representation of the tetris arena.
 * @param droppingTetromino A pointer to the dropping tetromino object.
 * @param rotationDirection The direction to rotate the tetromino, 1 for right, -1 for left. 
 *
 * @returns True on success, false otherwise.
 */
bool WallKickDroppingTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino, int rotationDirection);

/** Hard Drop a tetromino.
 *
 * @note Hard dropping causes the tetromino to land instantly.
 * @note See "Hard Drop": https://tetris.wiki/Drop
 *
 * @param arena The matrix representation of the tetris arena.
 * @param droppingTetromino A pointer to the dropping tetromino object.
 */
void HardDropTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino);

/** Drop a tetromino a single grid square (provided it does not collide)
 *
 * @note See "Soft Drop": https://tetris.wiki/Drop
 *
 * @param arena The matrix representation of the tetris arena.
 * @param droppingTetromino A pointer to the dropping tetromino object.
 */
void SoftDropTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino);

/** Shift a tetromino either left or right (provided it does not collide)
 *
 * @param arena The matrix representation of the tetris arena.
 * @param droppingTetromino A pointer to the dropping tetromino object.
 * @param translation A negative or positive value indicating how far to shift the tetromino left, or right, respectively.
 */
void ShiftTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino, int translation);

#endif //GAME_H
