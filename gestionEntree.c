#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "gestionEntree.h"

//fonction permettant de lire une ligne
//arg:le descripteur du fichier
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
void checkFile(infoJeu info){
	if(info.nbrJoueurs < 0 || info.nbrMains < 0 || info.nbrDecks < 1){
		perror("corrupted data\n");
		exit(-1);
	}
	for(int i=0; i<info.nbrJoueurs; i++){
		if(info.joueurs[i].nbrJetons < 0 ||
		   info.joueurs[i].objJetons < 0 || 
		   info.joueurs[i].valStop < 0 || 
		   info.joueurs[i].valStop > 21 ||
		   info.joueurs[i].strategie.mise <= 0)
		{
			perror("corrupted data\n");
			exit(-1);
		}
	}
}

//fonction stockant les information d'un joueur 
//arg1:buffeur contenant la ligne du joueur spécifié à l'argument 3
//arg2:la structure qui récupère les info de ce joueur
//arg3: le numéro du joueur concerné
infoJeu lireEntreeJoueur(const char *buf, infoJeu result, int numJoueur){
	//si le type de la mise est + ou -
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

//cette fonction lit les informations d'entrées du jeu(#nbJoueurs ; nbDecks ; nbMains) et,
//celle des joueurs (#nbJetons ; typeMise  ; valStop ; objJetons)
//les stockes ensuite comme se doit dans la structure infojeu défini dans gestionEntree.h
//arg:descripteur du fihcier
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
	checkFile(result);
	return result;
}

//prend en entrée l'identifiant d'une carte,
//return la carte associée
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
//cette fonction retourne les cartes d'une main d'un joueur
//arg1:le tableau des identifiants des cartes
//arg2:le nombre de carte de la main
char *cardString(int *cartes,int top){
	char *chaine=malloc(sizeof(char)*22);
	int i=0;
	for(i=0;i<top;i++)
		chaine[i] = cardIdToCard(cartes[i]);
	chaine[i] = '\0';
	return chaine;
}

//Comme l'indique son nom, cette fonction écrit la structure playerInfo
//dans un fichier de sortie pour chaque joueur
//arg1:information de la main(tours,
//arg2:l'identifiant du joueur concerné
void ecritureFichierSortie(playerInfo inforound,int i){
		int fd;
		char* path=malloc(sizeof(char)*30);
		char *c=malloc(sizeof(char)*35);
		sprintf(path,"./PlayerOutputFile%d.txt",i+1);
		printf("writing to %s\n",path);
		fd = open(path,O_TRUNC|O_CREAT|O_WRONLY,0600);
		if(fd<0){
			perror("error");
			exit(-1);
		}
		write(fd, "#cartes;totalJoueur;banque;totalBanque;mise;gain;nbJetons\n", sizeof(char)*58);
		for(int i=0; i<inforound.nbrRounds; i++){
			//transformation des identifiants des cartes en cartes(celles du joueurs et celles de la banque)
			char *ch1 = cardString(inforound.round[i].cartesJoueur,inforound.round[i].topJoueur);
			char *ch2 = cardString(inforound.round[i].cartesBanque,inforound.round[i].topBanque);
			sprintf(c,"%s;%d;%s;%d;%d;%d;%d\n", ch1,
			                                    inforound.round[i].totalJoueur,
		                                    	ch2,
		                                    	inforound.round[i].totalBanque,
		                                    	inforound.round[i].mise,
		                                    	inforound.round[i].gain,
												inforound.round[i].nbJetons);
			write(fd,c,strlen(c));
			free(ch1);
			free(ch2);
		}
		close(fd);
		free(path);
		free(c);
}



