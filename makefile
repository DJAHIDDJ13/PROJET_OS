main: clean processManagment.o gestionEntree.o main.o deck.o
	gcc -g -Wall main.o gestionEntree.o processManagment.o deck.o -o main

main.o: main.c
	gcc -g -Wall -c main.c

processManagment.o: processManagment.c processManagment.h
	gcc -g -Wall -c processManagment.c
	
gestionEntree.o: gestionEntree.c gestionEntree.h
	gcc -g -Wall -c gestionEntree.c

deck.o: deck.c deck.h
	gcc -g -Wall -c deck.c
	
editeur:
	geany *.c *.h makefile &
	
clean:
	rm -f *.o main
