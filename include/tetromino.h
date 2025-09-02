#ifndef TETROMINO_H
#define TETROMINO_H

#include <SDL3/SDL.h>
#include <stdbool.h>

/**
 * @brief Generic tetromino configuration enum values.
 */
enum TetrominoConfig
{
    /** @brief How many different tetromino identifiers there are (i.e. how many shapes). */
    TETROMINO_COUNT = 7,

    /** @brief The maximum dimension of a tetromino block (i.e. how big the square matrix representation of a tetromino is). */
    TETROMINO_MAX_SIZE = 4, 
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
    /** @brief The identifier for this shape */
    TetrominoIdentifier identifier;

    /** @brief A pointer to the texture that belongs to this shape. */
    SDL_Texture* texture;

    /** @brief A matrix containing four orientations (of the same shape) representing this tetromino in 2D space. */
    bool coordinates[4][TETROMINO_MAX_SIZE][TETROMINO_MAX_SIZE];

} TetrominoShape;

/**
 * @brief A struct containing the current state of the currently dropping tetromino.
 *
 * @details Contains the tetromino shape, coordinate location, orientation and the tick at which
 * it was marked for termination.
 */
typedef struct DroppingTetromino
{
    /** @brief The x-coordinate location of the tetromino in the arena. **/
    int x;

    /** @brief The y-coordinate location of the tetromino in the arena. **/
    int y;

    /** @brief The current orientation of the dropping tetromino. **/
    enum Orientation orientation;

    /** @brief A pointer to the tetromino shape object that contains details unique to each tetromino. **/
    const TetrominoShape* shape;

    /** @brief The tick at which the dropping tetromino was marked for termination. **/
    Uint64 terminationTick; // 

    // TODO Possibly implement tracker for number of moves, so we can limit the number of rotations to 15 before
    // it hard locks, preventing infinite spin (see wiki)
} DroppingTetromino;

/**
 * @brief The 'bag' containing the set of possible tetrominoes.
 */
typedef struct TetrominoBag
{
    int dropCount;
    TetrominoIdentifier bag[TETROMINO_COUNT];
} TetrominoBag;

/**
 * @brief Initialise and shuffle a tetromino bag.
 *
 * @note The "random" selection is done using the tetris guidelines Random Generator, wherein a "bag" of the possible
 * tetrominoes is generated and dished out one by one until the bag is empty, at which point it is reshuffled.
 */
void InitTetrominoBag(TetrominoBag* bag);

/**
 * @brief Draw the next tetromino shape from the bag.
 *
 * @param bag A pointer to the TetrominoBag state.
 * @return A readonly TetrominoShape.
 */
const TetrominoShape* NextTetrominoFromBag(TetrominoBag* bag);

/**
 * @brief Return a pointer to a tetromino shape object using its identifier.
 *
 * @note This should be treated as a READONLY object, the only reason it is not is so we can initialise
 * the shape textures
 *
 * @param identifier
 * @return A tetromino shape object.
 */
TetrominoShape* GetTetrominoShapeByIdentifier(TetrominoIdentifier identifier);

/**
 * @brief Rotate a given dropping tetromino either left or right.
 * 
 * @param droppingTetromino A pointer to the dropping tetromino.
 * @param rotationAmount A number, either positive or negative, indicating the number of times to rotate a shape, right or left respectively.
 *
 * @returns True on success, false otherwise.
 */
void RotateDroppingTetromino(DroppingTetromino* droppingTetromino, int rotationAmount);

/** 
 * @brief Shuffles a given array using a Fisher-yates shuffle.
 *
 * @param array A pointer to an integer array.
 * @param n The size of the array.
 */
static void Shuffle(int* array, size_t n);

#endif //TETROMINO_H
