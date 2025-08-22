#ifndef GAME_H
#define GAME_H

#include "util.h"
#include "tetromino.h"

bool GameIteration(SDL_Renderer* renderer, DroppingTetromino* droppingTetromino,
                   TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH]);

bool CheckDroppingTetrominoCollision(DroppingTetromino* droppingTetromino,
                                     TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], int8_t x, int8_t y);

void ResetDroppingTetromino(DroppingTetromino* tetromino);

uint16_t ClearFilledRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH]);

/* Drops everything above this row by one
 *
*/
void DropRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], int dropToRow);

#endif //GAME_H
