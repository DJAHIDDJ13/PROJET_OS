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

infoJeu lireEntreeJoueur(const char *buf, infoJeu result, int numJoueur);
infoJeu lireEntree(FILE* fichier);
#endif
