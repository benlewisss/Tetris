#ifndef TETROMINO_H
#define TETROMINO_H

#include <SDL3/SDL.h>
#include <stdbool.h>

/**
 * @brief Generic tetromino configuration enum values.
 */
enum TetrominoConfig
{
    TETROMINO_COUNT = 7, // How many different tetromino identifiers there are (how many shapes)
    TETROMINO_MAX_SIZE = 4, // The maximum dimension of a tetromino block (i.e. how big the square matrix representation of a tetromino is)
};

/**
 * @brief The four possible directions in 2D space that a tetromino could be oriented.
 */
enum Orientation
{
    NORTH,
    EAST,
    SOUTH,
    WEST,
};

/**
 * @brief The set of unique identifiers for the possible tetromino shapes.
 */
typedef enum TetrominoIdentifier
{
    I = 1,
    O = 2,
    T = 3,
    Z = 4,
    S = 5,
    L = 6,
    J = 7,
} TetrominoIdentifier;

/**
 * @brief A struct that represents a tetromino's shape and texture.
 */
typedef struct TetrominoShape
{
    TetrominoIdentifier identifier;
    SDL_Texture* texture;
    bool coordinates[4][TETROMINO_MAX_SIZE][TETROMINO_MAX_SIZE]; // 4 Orientations in 2D Space, and a 2D matrix representation
} TetrominoShape;

/**
 * @brief A struct containing a Tetromino shape, coordinate location,
 * orientation and the time at which it was marked for termination
 */
typedef struct DroppingTetromino
{
    const TetrominoShape* shape;
    int x;
    int y;
    enum Orientation orientation;
    Uint64 terminationTime; // The time at which the dropping tetromino was marked for termination
    // TODO Possibly implement tracker for number of moves, so we can limit the number of rotations to 15 before it hard locks, preventing infinite spin (see wiki)
} DroppingTetromino;

/**
 * Return a pointer to a tetromino shape object using its identifier.
 *
 * @note This should be treated as a READONLY object, the only reason it is not is so we can initialise
 * the shape textures
 *
 * @param identifier
 * @return A tetromino shape object.
 */
TetrominoShape* GetTetrominoShapeByIdentifier(TetrominoIdentifier identifier);

/**
 * Return a pointer to a random readonly tetromino shape object.
 *
 * @note The "random" selection is done using the tetris guidelines Random Generator, wherein a "bag" of the possible
 * tetrominoes is generated and dished out one by one until the bag is empty, at which point it is reshuffled.
 *
 * @returns A tetromino shape object.
 */
const TetrominoShape* GetRandomTetrominoShape(void);

/**
 * Rotate a given dropping tetromino either left or right.
 * 
 * @param droppingTetromino A pointer to the dropping tetromino.
 * @param rotationAmount A number, either positive or negative, indicating the number of times to rotate a shape, right or left respectively.
 *
 * @returns True if success, false otherwise.
 */
void RotateDroppingTetromino(DroppingTetromino* droppingTetromino, int rotationAmount);

/** 
 * Shuffles a given array using a Fisher-yates shuffle.
 *
 * @param array A pointer to an integer array.
 * @param n The size of the array.
 */
static void Shuffle(int* array, size_t n);

#endif //TETROMINO_H
