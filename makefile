run:
	gcc  *.c -o main -Iincludes `sdl2-config --cflags --libs`
	./main
	rm main

build:
	gcc  main.c -o main
