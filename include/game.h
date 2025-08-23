#ifndef GAME_H
#define GAME_H

#include "util.h"
#include "tetromino.h"
/** The method handling all the game logic that should happen each frame
 * 
 * @param droppingTetromino A pointer to the dropping tetromino object.
 * @param arena The matrix representation of the tetris arena.
 * 
 * @return True if success, false otherwise.
 */
bool GameIteration(DroppingTetromino* droppingTetromino,
                   TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH]);

/** Checks whether the dropping tetromino object would collide at some given coordinates.
 * 
 * @param droppingTetromino A pointer to the dropping tetromino object.
 * @param arena The matrix representation of the tetris arena.
 * @param x The theoretical x-coordinate of the dropping tetromino.
 * @param y The theoretical y-coordinate of the dropping tetromino.
 * 
 * @return True if the tetromino would collide, false otherwise.
 */
bool CheckDroppingTetrominoCollision(const DroppingTetromino* droppingTetromino,
                                     const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], int x, int y);

/** Resets the attributed of the dropping tetromino, essentially "spawning" a new one on the board and deleting the old one.
 * 
 * @param droppingTetromino A pointer to the dropping tetromino object.
 */
void ResetDroppingTetromino(DroppingTetromino* droppingTetromino);

/** Scan the board for rows that should be cleared, clear them, and return the corresponding score.
 * 
 * @param arena The matrix representation of the tetris arena.
 * 
 * @return The score that should be awarded for clearing n rows.
 */
int ClearFilledRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH]);

/* Drops everything above this row by one.
 *
*/
static void DropRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], int dropToRow);

#endif //GAME_H
