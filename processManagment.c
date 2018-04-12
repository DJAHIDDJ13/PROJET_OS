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
int getCardValue(int cardid){
	return getValueFromCardID(cardid)>9?10:getValueFromCardID(cardid);
}

int somme(int *t, int tot){
	int s = 0;
	for(int i=0; i<tot; i++){
		s += t[i];
	}
	return s;
}
void play(infoJeu info, deck_t *deck){
	int **cartesJoueurs = malloc(sizeof(int*) * info.nbrJoueurs);
	for(int i=0; i<info.nbrJoueurs; i++)
		cartesJoueurs[i] = malloc(sizeof(int) * 22); //Le plus pire cas c'est 22 l'as consecutifs
	int *tops = malloc(sizeof(int) * info.nbrJoueurs);
	initCards(cartesJoueurs, tops, info.nbrJoueurs);
	int **ChildPipe = malloc(sizeof(int*)*info.nbrJoueurs);
	int **ParentPipe = malloc(sizeof(int*)*info.nbrJoueurs);
	int *inP = malloc(sizeof(int)*info.nbrJoueurs);
	int *outP = malloc(sizeof(int)*info.nbrJoueurs);
	for(int i=0; i<info.nbrJoueurs; i++){
		ChildPipe[i] = malloc(sizeof(int)*2);
		ParentPipe[i] = malloc(sizeof(int)*2);
		//Creation des pipes
		pipe(ChildPipe[i]);
		pipe(ParentPipe[i]);
		if(ChildPipe[i]<0 || ParentPipe[i]<0){
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
			close(ChildPipe[i][1]);
			close(ParentPipe[i][0]);
			inP[i] = ChildPipe[i][0], outP[i] = ParentPipe[i][1];
			//Envoi des information joueur
			write(outP[i], &info.joueurs[i], sizeof(joueur));
		} else {
			//Initialisation des pipes dans le fils
			close(ChildPipe[i][0]);
			close(ParentPipe[i][1]);
			int out = ChildPipe[i][1], in = ParentPipe[i][0];
			//Recevoir les information de jeu
			joueur info;
			read(in, &info, sizeof(joueur));
			// debut de jeu
			int mise = info.strategie.mise;
			while(info.nbrJetons - mise >= 0 && info.nbrJetons < info.objJetons) {
				//wait for start signal
				int sigP = 0;
				read(in, &sigP, sizeof(int));
				if(sigP){
					//initialisation de la somme
					int somme = 0;
					int top;
					int cartes[22];
					while(somme < info.valStop){
						//Envoi de signal
						int sig = 1;
						write(out, &sig, sizeof(int));
						//recevoir les cartes 
						read(in, &top, sizeof(int));
						read(in, cartes, sizeof(int)*22);
						//calcul de somme
						somme = 0;
						for(int j=0; j<top; j++){
							somme += getCardValue(cartes[j]);
						}
						//~ sleep(1);
					}
					//affichage
					printf("joueur n:%d[", i);
					for(int j=0; j<top; j++){
						printf("(%d,%d)%s",cartes[j], getCardValue(cartes[j]), j!=top-1?",":"");
					}
					printf("]%d,jet%d,mis%d\n", somme,info.nbrJetons, mise);

					//Envoi de signal d'arret de partie
					int sig = 2;
					write(out, &sig, sizeof(int));
					//Recevoir le win
					int win;
					read(in, &win, sizeof(int));
					//~ printf("win de %d = %d\n",i, win);
					if(win == 1){
						info.nbrJetons += mise;
						mise = info.strategie.mise;
					} else if(win == 0) {
						info.nbrJetons -= mise;
						switch(info.strategie.type){
							case '*':
								mise = info.strategie.mise;
								break;
							case '+':
								mise *= 2;
								break;
							case '-':
								mise /= 2;
								mise = mise?mise:1;
						}
					}
					//~ printf("jet de %d = %d\nmise = %d\n",i , info.nbrJetons, mise);
				}
			}
			//Envoi de signal d'arret
			int sig = -1;
			write(out, &sig, sizeof(int));
			exit(0);
		}
	}
	int* playerStop = malloc(sizeof(int)*info.nbrJoueurs);
	for(int i=0; i<info.nbrJoueurs; i++){
		playerStop[i] = 0;
	}
	while(somme(playerStop, info.nbrJoueurs) < info.nbrJoueurs){
		//sending start signal
		for(int i=0; i<info.nbrJoueurs; i++){
			int sigP = 1;
			if(playerStop[i] == 0)
				write(outP[i], &sigP, sizeof(int));
		}
		//starting a round
		printf("\nstarting round\n");
		//initializing cards
		for(int i=0; i<info.nbrJoueurs; i++){
			tops[i] = 0;
		}
		int topBanque = 0;
		int cartesBanque[22];
		for(int i=0; i<info.nbrJoueurs; i++){
			pushCard(cartesJoueurs[i], tops+i, deck);
		}
		pushCard(cartesBanque, &topBanque, deck);
		for(int i=0; i<info.nbrJoueurs; i++){
			pushCard(cartesJoueurs[i], tops+i, deck);
		}
		int* sigs = malloc(sizeof(int)*info.nbrJoueurs);
		for(int i=0; i<info.nbrJoueurs ; i++) {
			if(playerStop[i] == 0) {
				// start drawing for each active player
				do{
					//Recevoir signal
					read(inP[i], sigs+i, sizeof(int));
					//~ printf("sig de %d = %d\n",i, sig);
					if(sigs[i] == 1){
						//piochement
						pushCard(cartesJoueurs[i], tops+i, deck);
						//envoi des cartes
						write(outP[i], tops+i, sizeof(int));
						write(outP[i], cartesJoueurs[i], sizeof(int) * 22);
					}
				} while(sigs[i] == 1);
			}
		}
		//bank's turn
		int sommeBanque = 0;
		while(sommeBanque <= 16){
			pushCard(cartesBanque, &topBanque, deck);
			sommeBanque = 0;
			for(int i=0; i<topBanque; i++){
				sommeBanque += getCardValue(cartesBanque[i]);
			}
		}
		printf("banque = %d\n", sommeBanque);
		for(int i=0; i<info.nbrJoueurs; i++){
			if(sigs[i] == 2){
				//calcul de win
				int sommeJoueur = 0;
				for(int j=0; j<tops[i]; j++){
					sommeJoueur += getCardValue(cartesJoueurs[i][j]);
				}
				int win;
				if(sommeJoueur > 21){
					win = 0;
				} else {
					if(sommeBanque > 21){
						win = 1;
					} else {
						if(sommeJoueur == sommeBanque){
							win = -1;
						} else if(sommeJoueur > sommeBanque){
							win = 1;
						} else {
							win = 0;
						}
					}
				}
				//envoi de win
				write(outP[i], &win, sizeof(int));
			} else {
				playerStop[i] = 1;
				//calcul gain et ecriture dans fichier TODO
			}
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
