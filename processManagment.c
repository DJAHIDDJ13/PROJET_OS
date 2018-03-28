#include <sys/types.h>
#include "deck.h"
#include "gestionEntree.h"
#include "processManagment.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
void pushCards(int* cartes, int top, deck_t *deck, int nbrOfCards){
	for(int i=top; i<top+nbrOfCards; i++){
		int drawn = drawCard(deck);
		discardCard(deck, drawn);
		cartes[i] = drawn;
	}
}

void play(infoJeu info, deck_t *deck){
	pid_t* pids = malloc(sizeof(pid_t) * info.nbrJoueurs);
	int **cartesJoueurs = malloc(sizeof(int*) * info.nbrJoueurs);
	int *tops = malloc(sizeof(int) * info.nbrJoueurs);
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
			cartesJoueurs
			pushCards(cartesJoueur[i],tops[i] deck, 2);
			write(out, cartesJoueurs[i], sizeof(card_t*));
			printf("Sent Cards: %d %d\n", cartesJoueurs[i]->value, cartesJoueurs[i]->next->value);
			while(wait(NULL)>=0);
		} else {
			//Initialisation des pipes dans le fils
			close(ChildPipe[0]);
			close(ParentPipe[1]);
			int out = ChildPipe[1], in = ParentPipe[0];
			card_t* cartes;
			read(in, &cartes ,sizeof(card_t*));
			while(cartes){
				printf("%d ", cartes->value);
				cartes = cartes->next;
			}
			printf("\n");
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
