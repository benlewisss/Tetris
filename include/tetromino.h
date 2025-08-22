#ifndef TETROMINO_H
#define TETROMINO_H

#include <SDL3/SDL.h>

/* ENUMERATORS */
enum Orientation
{
	NORTH,
	EAST,
	SOUTH,
	WEST,
};

typedef enum
{
	I = 1,
	O = 2,
	T = 3,
	Z = 4,
	S = 5,
	L = 6,
	J = 7,
} TetrominoIdentifier;

enum
{
	TETROMINO_COUNT = 7, // How many different tetromino identifiers there are (how many shapes)
	TETROMINO_SIZE = 4, // How many blocks constitute a tetromino
};

/**
 * A structure that represents a tetromino's shape and color.
 */
typedef struct TetrominoShape
{
	TetrominoIdentifier identifier;
	SDL_Color color;
	uint8_t offsets[4][TETROMINO_SIZE * 2];
	// 4 Orientations in 2D Space, 4 coordinate pairs for each constituent square
} TetrominoShape;

/**
 * A struct containing a defined Tetromino shape, it's xy coordinate location, and it's rotation
 */
typedef struct DroppingTetromino
{
	int8_t x;
	int8_t y;
	enum Orientation rotation;
	TetrominoShape shape;
	bool terminate;
} DroppingTetromino;

/**
 * Return a pointer to a tetromino shape object using its identifier.
 *
 * @param identifier
 * @return A tetromino shape object.
 */
const TetrominoShape* GetTetrominoShapeByIdentifier(TetrominoIdentifier identifier);

/**
 * Return a pointer to a random tetromino shape object.
 *
 * \returns A tetromino shape object.
 */
const TetrominoShape* GetRandomTetrominoShape(void);

/**
 * Rotate a given dropping tetromino either left or right.
 * 
 * \param tetromino A pointer to the dropping tetromino
 * \param rotationAmount A number, either positive or negative, indicating the number of times to rotate a shape, right or left respectively
 *
 * \returns True if success, false otherwise.
 */
void RotateDroppingTetromino(DroppingTetromino* tetromino, int8_t rotationAmount);

#endif //TETROMINO_H
