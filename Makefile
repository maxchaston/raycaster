all:
	gcc -lSDL2 -lm doom.c -o raycast

run: all
	./raycast
