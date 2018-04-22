#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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
	if(cmpt<2)
		free(chaine);
	return cmpt<2?NULL:chaine;
}


infoJeu lireEntreeJoueur(const char *buf, infoJeu result, int numJoueur){
	if(strchr(buf, '+') || strchr(buf, '-')){
		if(sscanf(buf, "%d;%d%c;%d;%d", &(result.joueurs[numJoueur].nbrJetons),
		                              &(result.joueurs[numJoueur].strategie.mise),
		                              &(result.joueurs[numJoueur].strategie.type),
									  &(result.joueurs[numJoueur].valStop),
									  &(result.joueurs[numJoueur].objJetons))<4)
		{
			perror("Corrupted file! joueur\n");
			exit(-1);
		}
	} else {
		if(sscanf(buf, "%d;%d;%d;%d", &(result.joueurs[numJoueur].nbrJetons), 
		                              &(result.joueurs[numJoueur].strategie.mise),
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
	char *buf;
	int cmpt = 0;
	while((buf = line(fichier)) != NULL){
		if(!strchr(buf, '#')){
			if(cmpt == 0){
				if(sscanf(buf, "%d;%d;%d", &(result.nbrJoueurs), &(result.nbrDecks), &(result.nbrMains))<3){
					perror("Corrupted file!\n");
					exit(-1);
				}
				result.joueurs = calloc(result.nbrJoueurs, sizeof(joueur));
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
		free(buf);
	}
	return result;
}


char cardIdToCard(int value){
	switch (value % 13) {
		case 0: return 'A';
		case 9: return 'X'; 
		case 10: return 'J'; 
		case 11: return 'Q'; 
		case 12: return 'K';
		default: return '1'+value%13;
	}
}
char *cardString(int *cartes,int top){
	char *chaine=malloc(sizeof(char)*22);
	int i=0;
	for(i=0;i<top;i++)
		chaine[i] = cardIdToCard(cartes[i]);
	chaine[i] = '\0';
	return chaine;
}
void ecritureFichierSortie(playerInfo inforound,int i){
		int fd;
		char* path=malloc(sizeof(char)*30);
		char *c=malloc(sizeof(char)*35);
		sprintf(path,"./PlayerOutPutFile%d",i);
		printf("%s\n",path);
		fd = open(path,O_CREAT|O_WRONLY,0600);
		if(fd<0){
			perror("error");
			exit(-1);
		}
		for(int i=0; i<inforound.nbrRounds; i++){
			sprintf(c,"%s;%d;%s;%d;%d;%d;%d",cardString(inforound.round[i].cartesJoueur,inforound.round[i].topJoueur),inforound.round[i].totalJoueur,
			cardString(inforound.round[i].cartesBanque,inforound.round[i].topBanque),inforound.round[i].totalBanque,inforound.round[i].mise,inforound.round[i].gain,
			inforound.round[i].nbJetons);
			write(fd,c,strlen(c));write(fd,"\n",1);
		}
		close(fd);
		free(path);
		free(c);
	
}



