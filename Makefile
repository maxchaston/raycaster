all:
	gcc -lSDL2 -lm raycaster.c -o raycast

debug:
	gcc -g -lSDL2 -lm raycaster.c -o bug

run: all
	./raycast
