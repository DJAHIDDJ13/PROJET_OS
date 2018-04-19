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
	deck_t* deck = initDeck(1, info.nbrDecks*4);
	shuffleDeck(deck);
	play(info, deck);
	removeDeck(deck);
	close(file);
	//File f;
	playerInfo pf;
	pf.nbrRounds =1;
	pf.round = malloc(sizeof(roundInfo));
	pf.round[0].topJoueur=3;
	pf.round[0].topBanque=2;
	pf.round[0].totalBanque = 12;
	pf.round[0].totalJoueur = 23;
	pf.round[0].mise = 50;
	pf.round[0].gain = 50;
	pf.round[0].nbJetons = 150;
	
	pf.round[0].cartesJoueur = malloc(sizeof(int)*pf.round[0].topJoueur);
	pf.round[0].cartesBanque = malloc(sizeof(int)*pf.round[0].topBanque);
	for(int i=0;i<pf.round[0].topJoueur;i++)
	{
		pf.round[0].cartesJoueur[i]=i+9;
	}
	for(int i=0;i<pf.round[0].topBanque;i++)
	{
		pf.round[0].cartesBanque[i]=i+9;
	}
		
	ecritureFichierSortie(pf,1);
	free(pf.round[0].cartesJoueur);
	free(pf.round[0].cartesBanque);
	free(pf.round);
	return(0);
}
