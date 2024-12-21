#include <SDL2/SDL.h>
#include "snake.h"
#include <stdio.h>
#define GridSize 20

int main(int argc, char *argv[]) {
    //Setting up SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    int screenWidth = 0, screenHeight = 0;
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    screenWidth = displayMode.w;
    screenHeight = displayMode.h;

    SDL_Window *window = SDL_CreateWindow(
        "Snake", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        screenWidth, screenHeight, 
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
    SDL_RenderClear(renderer);

    //Drawing the grid
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    int hPadding = 0;
    int vPadding = 0;
    int squareSize = calc_screen_size(window, &hPadding, &vPadding);
    
    //Initialising the game grid
    SDL_Rect** grid = alloc_grid();
    if (grid == NULL){
        return 1;
    }
    initialise_rect(grid, squareSize, hPadding, vPadding);

    for (int i = 0; i < GridSize; i++){
        for (int j = 0; j < GridSize; j++){
            SDL_RenderFillRect(renderer, &grid[i][j]);
        }
    }
    SDL_RenderPresent(renderer);

    Snake snake;
    int failure;
    failure = initialise_snake(&snake);
    if (failure){
        return 1;
    }

    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
    }

    //Free snake call should be whenever game over logic occurs, which I think will be here.
    free_snake(&snake);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free_grid(grid);
    return 0;
}

SDL_Rect** alloc_grid(void){
    SDL_Rect** grid = malloc(GridSize * sizeof(SDL_Rect*));
    if (grid == NULL){
        return NULL;
    }
    for (int i = 0; i < GridSize; i++){
        grid[i] = malloc(GridSize * sizeof(SDL_Rect));
        if (grid[i] == NULL){
            for (int j = 0; j < i; j++){
                free(grid[j]);
            }
            free(grid);
            return NULL;
        }
    }
    return grid;
}

void free_grid(SDL_Rect** grid){
    for(int i = 0; i < GridSize; i++){
        free(grid[i]);
    }
    free(grid);
}

void initialise_rect(SDL_Rect** grid, int squareSize, int hPadding, int vPadding){
    for (int i = 0; i < GridSize; i++){
        for(int j = 0; j < GridSize; j++){
            grid[i][j].x = hPadding + (i*squareSize);
            grid[i][j].y = vPadding + (j*squareSize);
            grid[i][j].w = squareSize;
            grid[i][j].h = squareSize;
        }
    }
}

int calc_screen_size(SDL_Window* window, int* hPadding, int* vPadding){
    int screenWidth = 0;
    int screenHeight = 0;
    int up, down, left, right = 0;
    SDL_GetWindowSize(window, &screenWidth, &screenHeight);
    SDL_GetWindowBordersSize(window, &up, &left, &down, &right);
    screenWidth -= (left+right);
    screenHeight -= (up+down);

    int paddingWidth = screenWidth * 0.05;
    int paddingHeight = screenHeight * 0.05;

    int gridWidth = screenWidth - (2 * paddingWidth);
    int gridHeight = screenHeight - (2 * paddingHeight);

    int squareSize = (gridWidth < gridHeight) ? (gridWidth/GridSize) : (gridHeight/GridSize);

    //Now we have the square size, basically always based on the width, I need to calculate a new offset for the width so that it is centred.
    int totalGridSize = squareSize * GridSize;
    int newPaddingWidth = (screenWidth - totalGridSize) / 2;

    *hPadding = newPaddingWidth;
    *vPadding = paddingHeight;

    return squareSize;
}

int initialise_snake(Snake* snake){
    //Positions array starts at 10 length, as most games will likely end well before the player reaches 10 length of the snake.
    snake->positionsLength = 10;
    snake->positions = malloc(snake->positionsLength * sizeof(Coords));
    if (snake->positions == NULL){
        printf("Unable to allocate the Snake positions array.\n");
        return 1;
    }
    //Starting length of 3 for the Snake.
    snake->length = 3;
    snake->direction = LEFT;
    /*
    Going to start the Snake in the top left corner, with the head at (2,0)
    */
    snake->head.x = 2;
    snake->head.y = 0;
    // Tail is at (0,0)
    snake->tail.x = 0;
    snake->tail.y = 0;
    /*
    I'm going to organise the positions by row and then column so that I can just iterate through the array once and change the colour when I encounter a value in the positions array.
    */
    // Setting the initial positions.
    snake->positions[0].x = 0;
    snake->positions[0].y = 0;
    snake->positions[1].x = 1;
    snake->positions[1].y = 0;
    snake->positions[2].x = 2;
    snake->positions[2].y = 0;
    // IMPORTANT, I'm using snake.length to track initialised (and valid) values of the positions. snake.positionsLength is how many indexes we can actually use.
    return 0;
}

void free_snake(Snake* snake){
    if (snake->positions != NULL){
        free(snake->positions);
        snake->positions = NULL;
    }
}
