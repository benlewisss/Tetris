#ifndef TETRINOMINO_H
#define TETRINOMINO_H

#include <SDL3/SDL.h>

// How many blocks constitute a tetromino
#define TETROMINO_SIZE 4

// How many different tetromino shapes there are
#define TETROMINO_COUNT 7

/**
 * A structure that represents a tetromino color and shape.
 */
typedef struct TetrominoShape
{
    SDL_Color color;
    int8_t offsets[TETROMINO_SIZE * 2];
} TetrominoShape;

/**
 * A struct containing a defined Tetromino shape, it's xy coordinate location, and it's rotation
 */
typedef struct DroppingTetromino
{
    uint8_t x;
    uint8_t y;
    uint16_t rotation; // It doesn't seem possible to rotate a shape 16000 times before it hits the floor TODO If you put a limit to rotations/sec
    TetrominoShape shape;
    bool terminate;
} DroppingTetromino;

// Tetromino shape declarations
const TetrominoShape PIECE_I;
const TetrominoShape PIECE_O;
const TetrominoShape PIECE_T;
const TetrominoShape PIECE_Z;
const TetrominoShape PIECE_N;
const TetrominoShape PIECE_L;
const TetrominoShape PIECE_J;

/**
 * Return a random tetromino shape object.
 *
 * \returns A tetromino shape object.
 */
TetrominoShape get_random_tetromino_shape();

#endif //TETRINOMINO_H