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
	pid_t* pids = malloc(sizeof(pid_t) * info.nbrJoueurs);
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
			//initialisation des cartes joueur
			pushCard(cartesJoueurs[i], tops+i, deck);
			//envoi des cartes
			write(out, tops+i, sizeof(int));
			write(out, cartesJoueurs[i], sizeof(int) * 22);
			int sig = 0;
			read()
			while(wait(NULL)>=0);
		} else {
			//Initialisation des pipes dans le fils
			close(ChildPipe[0]);
			close(ParentPipe[1]);
			int out = ChildPipe[1], in = ParentPipe[0];
			//recevoir les cartes 
			int top;
			int *cartes;
			read(in, &top, sizeof(int));
			read(in, cartes, sizeof(int)*22);
			printf("[");
			for(int j=0; j<top; j++){
				printf("%d%s", cartes[j], j!=top-1?",":"");
			}
			printf("]\n");
			pids[i] = pid;
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
