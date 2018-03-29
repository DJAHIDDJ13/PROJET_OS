#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include "gestionEntree.h"


char *line(int fd)
{
	char *chaine=calloc(1000,sizeof(char));
	char c;
	int cmpt = 0;
	while(read(fd,&c,1) > 0 && c != '\n')
	{
			sprintf(chaine,"%s%c",chaine,c);
			cmpt++;	
	}
	return cmpt<2?NULL:chaine;
}


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


infoJeu lireEntree(int fichier){
	infoJeu result;
	char *buf=calloc(1000,sizeof(char));
	int cmpt = 0;
	while((buf = line(fichier)) != NULL){
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
