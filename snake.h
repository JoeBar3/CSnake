#pragma once

#include <SDL2/SDL.h>

/*
Enum representing all of the possible directions the snake can travel.
*/
typedef enum{
    UP,
    LEFT,
    RIGHT,
    DOWN
} Direction;

/*
Struct representing a coordinate pair in a Cartesian Plane.
*/
typedef struct{
    int x; // The horizontal position (column).
    int y; // The vertical position (row).
} Coords;

/*
Struct representing the Snake, encapsulating all of its required data.
*/
typedef struct{
    Coords* positions; // Pointer to array of coordinates that the Snake occupies.
    int length; // Fixed integer value representing the length of the Snake. This is not guaranteed to be the same as the length of the positions array.
    int positionsLength; // Actual length of the positions array.
    Direction direction; // Enum value representing up, left, right and down.
    Coords head; // The head of the Snake.
    Coords tail; // The tail of the Snake.
} Snake;

/*
A function to allocate a two dimensional game grid.
Returns a pointer to a two dimensional array on success, NULL on failure.
*/
SDL_Rect** alloc_grid(void);

/*
A function to free an existing two dimensional game grid.
*/
void free_grid(SDL_Rect** grid);

/*
A function to initialise all of the SDL_Rect structs that were allocated in alloc_grid.
*/
void initialise_rect(SDL_Rect** grid, int squareSize, int hPadding, int vPadding);

/*
A function that calculates the available screen size.
Returns the square size and modifies the hPadding and vPadding, which are passed by reference.
*/
int calc_screen_size(SDL_Window* window, int* hPadding, int* vPadding);

/*
A function to initialise a newly created Snake struct.
Returns 0 on success, 1 on failure to allocate the position array.
*/
int initialise_snake(Snake* snake);

/*
A function to free an existing Snake struct.
*/
void free_snake(Snake* snake);

/*
A function to draw the updated version of the game grid.
*/
void draw_grid(SDL_Renderer* renderer, SDL_Rect** grid, Snake* snake, Coords* apple);

/*
A function to update the game state for every turn.
Returns 0 on success and 1 on failure.
Failure usually indicates an illegal move and therefore game over, but in rare cases it can also indicate a failure to allocate enough memory for the positions grid.
*/
int move_snake(SDL_Rect** grid, Snake* snake, Coords* apple);

/*
A function to randomly generate a new available position on the board for the apple to spawn.
*/
void gen_new_apple(Coords* apple, Snake* snake);

/*
A function to add a new value to the positions array.
Returns 0 on success and 1 on failure.
*/
int insert_position(Snake* snake, Coords* value);
