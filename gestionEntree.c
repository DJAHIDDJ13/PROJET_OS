#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gestionEntree.h"
#include <ctype.h>
infoJeu lireEntreeJoueur(const char *buf, infoJeu result, int numJoueur){
	if(strchr(buf, '+') || strchr(buf, '-')){
		if(sscanf(buf, "%d;%d%c;%d;%d", &(result.joueurs[numJoueur].nbrJetons), &(result.joueurs[numJoueur].strategie.mise),&(result.joueurs[numJoueur].strategie.type),
									  &(result.joueurs[numJoueur].valStop),
									  &(result.joueurs[numJoueur].objJetons))<4)
		{
			perror("Corrupted file! joueur\n");
			exit(-1);
		}
	} else {
		if(sscanf(buf, "%d;%d;%d;%d", &(result.joueurs[numJoueur].nbrJetons), &(result.joueurs[numJoueur].strategie.mise),
									  &(result.joueurs[numJoueur].valStop),
									  &(result.joueurs[numJoueur].objJetons))<4)
		{
			perror("Corrupted file! joueur\n");
			exit(-1);
		}
		result.joueurs[numJoueur].strategie.type = '*';
	}
	return result;
}
infoJeu lireEntree(FILE* fichier){
	infoJeu result;
	char buf[1000];
	int cmpt = 0;
	while((fscanf(fichier, "%s", buf)) == 1){
		if(!strchr(buf, '#')){
			if(cmpt == 0){
				if(sscanf(buf, "%d;%d;%d", &(result.nbrJoueurs), &(result.nbrMains), &(result.nbrDecks))<3){
					perror("Corrupted file!\n");
					exit(-1);
				}
				result.joueurs = malloc(sizeof(joueur) * result.nbrJoueurs);
			} else {
				if(result.nbrJoueurs-cmpt>=0){
					result = lireEntreeJoueur(buf, result, cmpt-1);
				} else {
					perror("Corrupted file!cmp!=0\n");
					exit(-1);
				}
			}
			cmpt ++;
		}
	}
	return result;
}
