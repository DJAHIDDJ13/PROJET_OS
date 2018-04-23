#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "gestionEntree.h"
#include "deck.h"
#include "processManagment.h"

int main (int argc, char** argv) {
	if(argc < 2){
		perror("argument error\n");
		exit(-1);
	}
	initDeckLib();
	int  file = open(argv[1],0);
	if(file < 0){
		perror("corrupted file\n");
		exit(-1);
	}
	infoJeu info = lireEntree(file);
	deck_t* deck = initDeck(1, info.nbrDecks);
	shuffleDeck(deck);
	play(info, deck);
	free(info.joueurs);
	removeDeck(deck);
	close(file);
	return(0);
}
