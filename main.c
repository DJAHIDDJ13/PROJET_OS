#include <string.h>
#include <stdio.h>
#include "gestionEntree.h"
#include "deck.h"
#include "processManagment.h"
int main () {
	initDeckLib();
	FILE* file = fopen("./fich","r");
	infoJeu info = lireEntree(file);
	deck_t *deck = initDeck(1, info.nbrDecks);
	shuffleDeck(deck);
	play(info, deck);
	fclose(file);
	removeDeck(deck);
	return(0);
}
