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

void playBanque(infoJeu info, deck_t* deck, int *outP, int *inP, int **cartesJoueurs, int* tops){
	int* mises = malloc(sizeof(int)*info.nbrJoueurs);
	int* jetons = malloc(sizeof(int)*info.nbrJoueurs);
	playerInfo *journal = malloc(sizeof(playerInfo) * info.nbrJoueurs);
	for(int i=0; i<info.nbrJoueurs; i++){
		journal[i].round = malloc(sizeof(roundInfo) * info.nbrMains);
		journal[i].nbrRounds = 0;
	}
	int* playerStop = malloc(sizeof(int)*info.nbrJoueurs);
	for(int i=0; i<info.nbrJoueurs; i++){
		playerStop[i] = 0;
	}
	int mainsJoue = 0;
	while(somme(playerStop, info.nbrJoueurs) < info.nbrJoueurs && mainsJoue < info.nbrMains){
		//sending start signal
		for(int i=0; i<info.nbrJoueurs; i++){
			int sigP = 1;
			if(playerStop[i] == 0){
				write(outP[i], &sigP, sizeof(int));
				read(inP[i], &mises[i], sizeof(int));
				read(inP[i], &jetons[i], sizeof(int));
				//printf("%d %d\n", mises[i], jetons[i]);
			}
		}
		//starting a round
		printf("\n");
		shuffleDeck(deck);
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
		int* sigs = calloc(info.nbrJoueurs, sizeof(int));
		for(int i=0; i<info.nbrJoueurs ; i++) {
			if(playerStop[i] == 0) {
				// start drawing for each active player
				do{
					//Recevoir signal
					read(inP[i], sigs+i, sizeof(int));
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
		for(int i=0; i<info.nbrJoueurs; i++)
			if(sigs[i] <= 0)
				playerStop[i] = 1;
		int endGame = 1;
		for(int i=0; i<info.nbrJoueurs; i++){
			if(sigs[i] == 2)
				endGame = 0;
		}
		if(endGame){
			printf("Ended game with %d players out\n", somme(playerStop, info.nbrJoueurs));
			free(sigs);
			break;
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
				//mise a jour journal
				journal[i].round[journal[i].nbrRounds].mise = mises[i];
				journal[i].round[journal[i].nbrRounds].nbJetons = jetons[i]-mises[i];				
				journal[i].round[journal[i].nbrRounds].topJoueur = tops[i];				
				journal[i].round[journal[i].nbrRounds].cartesJoueur = malloc(sizeof(int) * 22);
				for(int j=0; j<tops[i]; j++){
					journal[i].round[journal[i].nbrRounds].cartesJoueur[j] = cartesJoueurs[i][j];
				}
				journal[i].round[journal[i].nbrRounds].topBanque = topBanque;				
				journal[i].round[journal[i].nbrRounds].cartesBanque = malloc(sizeof(int) * 22);
				for(int j=0; j<topBanque; j++){
					journal[i].round[journal[i].nbrRounds].cartesBanque[j] = cartesBanque[j];
				}
				journal[i].round[journal[i].nbrRounds].totalBanque = sommeBanque;				
				journal[i].round[journal[i].nbrRounds].totalJoueur = sommeJoueur;				
				journal[i].round[journal[i].nbrRounds].gain = win? mises[i]:0;				
				journal[i].nbrRounds++;
			} else {
				playerStop[i] = 1;
			}
		}
		free(sigs);
		mainsJoue++;
	}
	for(int i=0; i<info.nbrJoueurs; i++){
		ecritureFichierSortie(journal[i], i);
	}
	//~ //sending end signal
	for(int i=0; i<info.nbrJoueurs; i++){
		int sigP = -1;
		if(playerStop[i] == 0)
			write(outP[i], &sigP, sizeof(int));
	}
	// freeing memory
	for(int i=0; i<info.nbrJoueurs; i++){
		for(int j=0; j<journal[i].nbrRounds; j++){
			free(journal[i].round[j].cartesBanque);
			free(journal[i].round[j].cartesJoueur);
		}
		free(journal[i].round);
	}
	free(journal);
	free(mises);
	free(jetons);
	free(playerStop);
}
void update_mise_jetons(int win ,int *mise, int *jetons, strat str){
	if(win == 1){
		*jetons += *mise;
		*mise = str.mise;
	} else if(win == 0) {
		*jetons -= *mise;
		switch(str.type){
			case '*':
				*mise = str.mise;
				break;
			case '+':
				*mise *= 2;
				break;
			case '-':
				*mise /= 2;
				*mise = *mise?*mise:1;
		}
	}	
}
void playJoueur(joueur info, int in, int out, int i){
	// debut de jeu
	int mise = info.strategie.mise;
	while(info.nbrJetons - mise >= 0 && info.nbrJetons < info.objJetons) {
		//wait for start signal
		int sigP = 0;
		read(in, &sigP, sizeof(int));
		if(sigP == 1){
			write(out, &mise, sizeof(int));
			write(out, &info.nbrJetons, sizeof(int));
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
				//affichage et calcul de somme
				somme = 0;
				printf("joueur n:%d[", i);
				for(int j=0; j<top; j++){
					printf("(%d,%d)%s",cartes[j], getCardValue(cartes[j]), j!=top-1?",":"");
					somme += getCardValue(cartes[j]);
				}
				printf("]%d,jet%d,mis%d\n", somme,info.nbrJetons, mise);
			}
			//Envoi de signal d'arret de partie
			int sig = 2;
			write(out, &sig, sizeof(int));
			//Recevoir le win
			int win;
			read(in, &win, sizeof(int));
			update_mise_jetons(win, &mise, &info.nbrJetons, info.strategie);
		} else if(sigP == -1){
			break;
		}
	}
	int sigP = 0;
	read(in, &sigP, sizeof(int));
	write(out, &mise, sizeof(int));
	write(out, &info.nbrJetons, sizeof(int));
	//Envoi de signal d'arret
	int sig = -1;
	write(out, &sig, sizeof(int));
}
void play(infoJeu info, deck_t *deck){
	// Les cartes des joueurs definie par un tableau des entiers et un entier top qui est l'indice de la de la premier carte
	int **cartesJoueurs = malloc(sizeof(int*) * info.nbrJoueurs);
	for(int i=0; i<info.nbrJoueurs; i++)
		cartesJoueurs[i] = malloc(sizeof(int) * 22); //Le plus pire cas c'est 22 l'as consecutifs
	int *tops = malloc(sizeof(int) * info.nbrJoueurs);
	// initialisations des cartes
	initCards(cartesJoueurs, tops, info.nbrJoueurs);
	// allocation des pipes
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
			//L'execution principal de joueur
			playJoueur(info, in, out, i);
			exit(0);
		}
	}
	//l'execution pricipal de la banque
	playBanque(info, deck, outP, inP, cartesJoueurs, tops);
	printf("OUT\n");
	//deallocation de memoire
	for(int i=0; i<info.nbrJoueurs; i++){
		free(cartesJoueurs[i]);
		free(ChildPipe[i]);
		free(ParentPipe[i]);
	}
	free(ChildPipe);
	free(ParentPipe);
	free(cartesJoueurs);
	free(tops);
	free(inP);
	free(outP);
}
