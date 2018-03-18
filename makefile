main:clean gestionEntree.o main.o
	gcc -g -Wall main.o gestionEntree.o -o main

main.o: main.c
	gcc -g -Wall -c main.c

gestionEntree.o: gestionEntree.c gestionEntree.h
	gcc -g -Wall -c gestionEntree.c

editeur:
	geany *.c *.h makefile
clean:
	rm -f *.o main
