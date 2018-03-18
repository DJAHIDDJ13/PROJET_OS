#include <string.h>
#include <stdio.h>
#include "gestionEntree.h"
int main () {
	FILE* f = fopen("./fich","r");
	infoJeu j = lireEntree(f);
	printf("%d %d %d\n", j.nbrJoueurs, j.nbrMains, j.nbrDecks);
	for(int i=0; i<j.nbrJoueurs; i++)
		printf("%d %d %c %d %d\n",j.joueurs[i].nbrJetons, j.joueurs[i].strategie.mise,j.joueurs[i].strategie.type,  j.joueurs[i].valStop, j.joueurs[i].objJetons);
	fclose(f);
	return(0);
}
