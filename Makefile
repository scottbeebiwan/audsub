audsub: audsub.c
	gcc -o audsub audsub.c -lSDL2 -lSDL2_mixer -Wall -Ofast -march=native