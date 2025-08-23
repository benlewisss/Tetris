#ifndef TETROMINO_H
#define TETROMINO_H

#include <SDL3/SDL.h>
#include <stdbool.h>

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
	TETROMINO_MAX_SIZE = 4, // The maximum dimension of a tetromino block (i.e. how big the square matrix representation of a tetromino is)
};

/**
 * A structure that represents a tetromino's shape and texture.
 */
typedef struct TetrominoShape
{
	TetrominoIdentifier identifier;
	SDL_Texture* texture;
	bool coordinates[4][TETROMINO_MAX_SIZE][TETROMINO_MAX_SIZE]; // 4 Orientations in 2D Space, and a 2D matrix representation
} TetrominoShape;

/**
 * A struct containing a defined Tetromino shape, it's xy coordinate location, and it's rotation
 */
typedef struct DroppingTetromino
{
	int x;
	int y;
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
TetrominoShape* GetTetrominoShapeByIdentifier(TetrominoIdentifier identifier);

/**
 * Return a pointer to a random readonly tetromino shape object.
 *
 * \returns A tetromino shape object.
 */
const TetrominoShape* GetRandomTetrominoShape(void);

/**
 * Rotate a given dropping tetromino either left or right.
 * 
 * \param tetromino A pointer to the dropping tetromino.
 * \param rotationAmount A number, either positive or negative, indicating the number of times to rotate a shape, right or left respectively.
 *
 * \returns True if success, false otherwise.
 */
void RotateDroppingTetromino(DroppingTetromino* tetromino, int rotationAmount);

#endif //TETROMINO_H
