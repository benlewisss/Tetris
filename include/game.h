#ifndef GAME_H
#define GAME_H

#include "util.h"
#include "tetromino.h"
/** The method handling all the game logic that should happen each frame
 * 
 * @param arena The matrix representation of the tetris arena.
 * @param droppingTetromino A pointer to the dropping tetromino object.
 * 
 * @return True if success, false otherwise.
 */
bool GameIteration(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH],
                   DroppingTetromino* droppingTetromino);


/** Checks whether the dropping tetromino object would collide at some given rotation
 *
 * @param arena The matrix representation of the tetris arena.
 * @param droppingTetromino A pointer to the dropping tetromino object.
 * @param translationX The theoretical translation of the dropping tetromino on the X plane.
 * @param translationY The theoretical translation of the dropping tetromino on the Y plane.
 * @param rotationAmount The theoretical rotation amount of the dropping tetromino.
 *
 * @return True if the tetromino would collide, false otherwise.
 */
bool CheckDroppingTetrominoCollision(const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], const DroppingTetromino* droppingTetromino, int translationX, int translationY, const int rotationAmount);

/** Writes the location of the dropping tetromino onto the arena, and then resets it's attributes, essentially "spawning" a new one.
 *
 * @param arena The matrix representation of the tetris arena.
 * @param droppingTetromino A pointer to the dropping tetromino object.
 */
void ResetDroppingTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino);

/** Scan the board for rows that should be cleared, clear them, and return the corresponding score.
 * 
 * @param arena The matrix representation of the tetris arena.
 * 
 * @return The score that should be awarded for clearing n rows.
 */
int ClearFilledRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH]);

/* TODO NEW Doc Drops everything above this row by one.
 *
*/
static void DropRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], int dropToRow, int dropAmount);

/** TODO IMPORTANT: This needs to be renamed to something befitting of its function to differentiate it from the rotating function in tetromino.c, this one abstracts a lot more game logic.
 * Maybe rename the other one?
 * 
 * @param arena The matrix representation of the tetris arena.
 * @param droppingTetromino A pointer to the dropping tetromino object.
 * @param rotationDirection The direction to rotate the tetromino, 1 for right, -1 for left. 
 *
 * @returns True if success, false otherwise.
 */
bool WallKickRotateDroppingTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino, const int rotationDirection);

void HardDropTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino);

void SoftDropTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino);

#endif //GAME_H
