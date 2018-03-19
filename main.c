#include <string.h>
#include <stdio.h>
#include "gestionEntree.h"
#include "deck.h"

int main () {
	initDeckLib();
	decktype_t type = P32;
	deck_t* d = initDeck(type, 1);
	shuffleDeck(d);
	printDrawPile(d);
	drawCard(d);
	printDrawPile(d);
	while(d->handCards){
		printf("%d ",d->handCards->value);
		d->handCards = d->handCards->next;
	}
	printf("\n");
	removeDeck(d);
	//~ FILE* f = fopen("./fich","r");
	//~ infoJeu j = lireEntree(f);
	//~ printf("%d %d %d\n", j.nbrJoueurs, j.nbrMains, j.nbrDecks);
	//~ for(int i=0; i<j.nbrJoueurs; i++)
		//~ printf("%d %d %c %d %d\n",j.joueurs[i].nbrJetons, j.joueurs[i].strategie.mise,j.joueurs[i].strategie.type,  j.joueurs[i].valStop, j.joueurs[i].objJetons);
	//~ fclose(f);
	return(0);
}
