#include <SDL2/SDL.h>
#include "snake.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define GridSize 20
#define keyVal e.key.keysym.sym

int main(void) {
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

    SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    //Drawing the grid
    int hPadding = 0;
    int vPadding = 0;
    int squareSize = calc_screen_size(window, &hPadding, &vPadding);
    
    //Initialising the game grid
    SDL_Rect** grid = alloc_grid();
    if (grid == NULL){
        return 1;
    }
    initialise_rect(grid, squareSize, hPadding, vPadding);

    Snake snake;
    int failure;
    failure = initialise_snake(&snake);
    if (failure){
        return 1;
    }

    Coords apple = {9,9};

    //Seeding the random number generator.
    srand(time(NULL));

    draw_grid(renderer, grid, &snake, &apple);
    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            if (e.type == SDL_KEYDOWN){
                if ((keyVal == SDLK_w || keyVal == SDLK_UP) && snake.direction != DOWN){
                    snake.direction = UP;
                }
                if ((keyVal == SDLK_a || keyVal == SDLK_LEFT) && snake.direction != RIGHT){
                    snake.direction = LEFT;
                }
                if ((keyVal == SDLK_s || keyVal == SDLK_DOWN) && snake.direction != UP){
                    snake.direction = DOWN;
                }
                if ((keyVal == SDLK_d || keyVal == SDLK_RIGHT) && snake.direction != LEFT){
                    snake.direction = RIGHT;
                }
            }
        }
        //Call move snake first, check return value to determine game over.
        int gameOver;
        gameOver = move_snake(&snake, &apple);
        if (gameOver){
            break;
        }
        draw_grid(renderer, grid, &snake, &apple);
        SDL_Delay(125);
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
    snake->direction = RIGHT;
    /*
    Going to start the Snake in the top left corner, with the head at (2,0)
    */
    snake->head.x = 0;
    snake->head.y = 2;
    // Tail is at (0,0)
    snake->tail.x = 0;
    snake->tail.y = 0;
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

void draw_grid(SDL_Renderer* renderer, SDL_Rect** grid, Snake* snake, Coords* apple){
    int isApple = 0;
    int isSnake = 0;
    SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
    SDL_RenderClear(renderer);
    for (int i = 0; i < GridSize; i++){
        for (int j = 0; j < GridSize; j++){
            isApple = 0;
            isSnake = 0;
            // Should be guaranteed that the apple and snake don't overlap.
            if (i == apple->x && j == apple->y){
                isApple = 1;
            }
            for (int k = 0; k < snake->length; k++){
                if (i == snake->positions[k].x && j == snake->positions[k].y){
                    isSnake = 1;
                    break;
                }
            }
            if (isApple){
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            else if (isSnake)
            {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            } else{
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
            }
            SDL_RenderFillRect(renderer, &grid[i][j]);
        }
    }
    SDL_RenderPresent(renderer);
}

int move_snake(Snake* snake, Coords* apple){
    //Checking the direction and that its in bounds.
    Coords newHead;
    if (snake->direction == UP && snake->head.y > 0){
        newHead.x = snake->head.x;
        newHead.y = snake->head.y - 1;
    }else if (snake->direction == LEFT && snake->head.x > 0){
        newHead.x = snake->head.x - 1;
        newHead.y = snake->head.y;
    }else if (snake->direction == RIGHT && snake->head.x < GridSize - 1){
        newHead.x = snake->head.x + 1;
        newHead.y = snake->head.y;
    }else if (snake->direction == DOWN && snake->head.y < GridSize - 1){
        newHead.x = snake->head.x;
        newHead.y = snake->head.y + 1;
    }else{
        // Movement is out of bounds so game over.
        return 1;
    }
    // Check for apple and check for collision with self.
    int collision = check_collision(snake, &newHead);
    if (collision){
        return 1;
    }
    if (newHead.x == apple->x && newHead.y == apple->y){
        int fail = 0;
        fail = insert_position(snake, &newHead);
        if (fail){
            // Unable to allocate enough memory.
            return 1;
        }
        gen_new_apple(apple, snake);
    }else{
        update_position(snake, &newHead);
    }
    return 0;
}

void gen_new_apple(Coords* apple, Snake* snake){
    int collision = 1;
    int col = -1;
    int row = -1;
    while (collision){
        int randNum = rand() % 400;
        col = randNum % 20;
        row = randNum / 20;
        Coords newApple = {col, row};
        collision = check_collision(snake, &newApple);
    }
    apple->x = col;
    apple->y = row;
}

int insert_position(Snake* snake, Coords* value){
    // If we have space, then we can just assing it to a new value and increment.
    if (snake->length < snake->positionsLength){
        snake->positions[snake->length] = *value;
        snake->length++;
        snake->head = *value;
    }else{
        //If the allocation fails, we return 1 which will be used to end the gameloop, and therefore we don't need to free the positions pointer here on failure.
        snake->positionsLength += 10;
        Coords* newPositions = realloc(snake->positions, snake->positionsLength);
        if (newPositions == NULL){
            return 1;
        }
        snake->positions = newPositions;
        snake->positions[snake->length] = *value;
        snake->length++;
        snake->head = *value;
    }
    return 0;
}

void update_position(Snake* snake, Coords* value){
    for (int i = 0; i < snake->length - 1; i++){
        snake->positions[i] = snake->positions[i+1];
    }
    snake->positions[snake->length - 1] = *value;
    snake->tail = snake->positions[0];
    snake->head = *value;
}

int check_collision(Snake* snake, Coords* value){
    for (int i = 0; i < snake->length; i++){
        if (snake->positions[i].x == value->x && snake->positions[i].y == value->y){
            return 1;
        }
    }
    return 0;
}
