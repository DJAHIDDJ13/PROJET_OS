#include <sys/types.h>
#include "deck.h"
#include "gestionEntree.h"
#include "processManagment.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
void pushCard(int* cartes, int *top, deck_t *deck){
	int drawn = drawCard(deck);
	discardCard(deck, drawn);
	cartes[(*top)++] = drawn;
}
void initCards(int **cartes, int *tops, int nbrJoueurs){
	for(int i=0; i<nbrJoueurs; i++){
		tops[i] = 0;
		for(int j=0; j<22; j++){
			cartes[i][j] = 0;
		}
	}
}
void play(infoJeu info, deck_t *deck){
	int **cartesJoueurs = malloc(sizeof(int*) * info.nbrJoueurs);
	for(int i=0; i<info.nbrJoueurs; i++)
		cartesJoueurs[i] = malloc(sizeof(int) * 22); //Le plus pire cas est 22 l'as consecutifs
	int *tops = malloc(sizeof(int) * info.nbrJoueurs);
	initCards(cartesJoueurs, tops, info.nbrJoueurs);
	for(int i=0; i<info.nbrJoueurs; i++){
		//Creation des pipes
		int ChildPipe[2];
		int ParentPipe[2];
		pipe(ChildPipe);
		pipe(ParentPipe);
		if(ChildPipe<0 || ParentPipe<0){
			perror("Pipe error");
			exit(-1);
		}
		pid_t pid = fork();
		if(pid<0){
			perror("Fork error");
			exit(-1);
		}
		if(pid){
			//Initialisation des pipes dans le parent
			close(ChildPipe[1]);
			close(ParentPipe[0]);
			int in = ChildPipe[0], out = ParentPipe[1];
			//Envoi des information joueur
			write(out, &info.joueurs[i], sizeof(joueur));
			//initialisation des cartes joueur
			pushCard(cartesJoueurs[i], tops+i, deck);
			pushCard(cartesJoueurs[i], tops+i, deck);
			int sig;
			do{
				//Recevoir signal
				read(in, &sig, sizeof(int));
				printf("sig = %d\n", sig);
				if(sig){
					//piochement
					pushCard(cartesJoueurs[i], tops+i, deck);
					//envoi des cartes
					write(out, tops+i, sizeof(int));
					write(out, cartesJoueurs[i], sizeof(int) * 22);
				}
			} while(sig);
			while(wait(NULL)>=0);
		} else {
			//Initialisation des pipes dans le fils
			close(ChildPipe[0]);
			close(ParentPipe[1]);
			int out = ChildPipe[1], in = ParentPipe[0];
			//Recevoir les information de jeu
			joueur info;
			read(in, &info, sizeof(joueur));
			printf("Max for %d is: %d\n",i, info.valStop);
			//initialisation de la somme
			int somme = 0;
			while(somme <= info.valStop){
				//Envoi de signal
				int sig = 1;
				write(out, &sig, sizeof(int));
				//recevoir les cartes 
				int top;
				int *cartes = malloc(sizeof(int) * 22);
				read(in, &top, sizeof(int));
				read(in, cartes, sizeof(int)*22);
				//Affichage des cartes et calcul de somme
				printf("[");
				somme = 0;
				for(int j=0; j<top; j++){
					printf("(%d,%d)%s",cartes[j], getValueFromCardID(cartes[j])>9?10:getValueFromCardID(cartes[j]), j!=top-1?",":"");
					somme += getValueFromCardID(cartes[j])>9?10:getValueFromCardID(cartes[j]);
				}
				printf("]\n");
			}
			//Envoi de signal d'arret
			int sig = 0;
			write(out, &sig, sizeof(int));
			exit(0);
		}
	}
}
//~ void play(infoJeu info, deck_t *deck){
	//~ for(int i=0; i<info.nbrJoueurs; i++){
		//~ pid_t pid = fork();
		//~ if(pid < 0){
			//~ perror("fork error");
			//~ exit(-1);
		//~ }
		//~ if(pid){
			//~ card_t **cartesJoueurs = malloc(sizeof(card_t) * info.nbrJoueurs);
			//~ card_t *catesBanque = NULL;
			//~ int drawn = drawCard(deck);
			//~ discardCard(deck, drawn);
			//~ pushCard(cartesBanque, drawn);
			//~ drawn = drawCard(deck);
			//~ discardCard(deck, drawn);
			//~ pushCard(cartesBanque, drawn);
			//~ for(int i=0; i<info.nbrJoueurs; i++){
				//~ int fd[2];
				//~ pipe(fd);
				//~ close(fd[1]);
				//~ int drawn = drawCard(deck);
				//~ discardCard(deck, drawn);
				//~ cartesJoueurs[i] = pushCard(NULL, drawn);
				//~ drawn = drawCard(deck);
				//~ discardCard(deck, drawn);
				//~ cartesJoueurs[i] = pushCard(cartesJoueurs[i], drawn);
				//~ write(fd[0], info.joueurs[i], sizeof(joueur*));
				//~ write(fd[0], cartesJoueurs[i], sizeof(card_t*));
				//~ write(fd[0], cartesJoueurs[i], sizeof(card_t*));
				
			//~ }
			//~ while(wait(NULL)>=0);
		//~ } else {
			//~ joueur* j;
			//~ read(fd[1], &j, sizeof(joueur*));
			//~ card_t *cartes;
			//~ read(fd[1], cartes, sizeof(card_t*));
			//~ card_t *banque;
			//~ read(fd[1], banque, sizeof(card_t*));
			//~ int somme = getValueFromCardID(cartes->value) + 
			//~ while(
			//~ close(fd[0]);
			//~ exit()
		//~ }
	//~ }
//~ }
