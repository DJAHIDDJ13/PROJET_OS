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

int main () {
	initDeckLib();
	int  file = open("./fich",0);
	infoJeu info = lireEntree(file);
	deck_t* deck = initDeck(1, info.nbrDecks);
	shuffleDeck(deck);
	play(info, deck);
	removeDeck(deck);
	close(file);
	return(0);
}
