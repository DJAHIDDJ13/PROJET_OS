#ifndef GES_FICH
#define GES_FICH
#include <stdio.h>
#include <sys/types.h>


typedef struct {
	int mise; 
	char type; // - + ou *
}strat;

typedef struct {
	int nbrJetons;
	int valStop;
	strat strategie;
	int objJetons;
} joueur;

typedef struct {
	int nbrJoueurs;
	int nbrMains;
	int nbrDecks;
	joueur* joueurs;
}infoJeu;

typedef struct{
	int *cartesJoueur;
	int topJoueur;
	int totalJoueur;
	int totalBanque;
	int *cartesBanque;
	int topBanque;
	int mise;
	int gain;
	int nbJetons;
}roundInfo;

typedef struct{
	roundInfo* round;
	int nbrRounds;
}playerInfo;

typedef struct{
	playerInfo* player;
}gameInfo;

infoJeu lireEntreeJoueur(const char *buf, infoJeu result, int numJoueur);
infoJeu lireEntree(int fichier);
char *cardString(int *cartes,int top);
void ecritureFichierSortie(playerInfo inforound,int i);
#endif
