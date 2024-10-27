all:
	gcc -lSDL2 -lm raycaster.c -o raycast

run: all
	./raycast
