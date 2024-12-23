all:
	clang -Wall -Wextra -Wpedantic snake.c -o snake -F/Library/Frameworks -framework SDL2
