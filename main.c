#include <string.h>
#include <stdio.h>
#include "gestionEntree.h"
#include "deck.h"
#include <sys/types.h>
int main () {
	initDeckLib();
	FILE* file = fopen("./fich","r");
	infoJeu info = lireEntree(file);
	deck_t *deck = initDeck(decktype_t.P52, info.nbrDecks);
	shuffleDeck(deck);
	play(info, deck);
	fclose(file);
	removeDeck(deck);
	return(0);
}
