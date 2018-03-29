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
	
	printf(" %d %d %d \n",info.nbrJoueurs,info.nbrDecks,info.nbrMains);
	
	for(int i=0;i<info.nbrJoueurs; i++)
	{
		printf("%d;%d%c;%d;%d\n",info.joueurs[i].nbrJetons,info.joueurs[i].strategie.mise,info.joueurs[i].strategie.type,info.joueurs[i].valStop,info.joueurs[i].objJetons);
	}
		
	deck_t *deck = initDeck(1, info.nbrDecks);
	/*shuffleDeck(deck);
	play(info, deck);
	
	removeDeck(deck);*/
	close(file);
	return(0);
}
