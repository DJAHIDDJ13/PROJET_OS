#include <sys/types.h>
#include "deck.h"
#include "gestionEntree.h"
#include "processManagment.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
// pour piocher un carte
// arg1: le tableau des cartes
// arg2: l'indice de la derniere cartes dans le tableau
// arg3: le deck des cartes 
void pushCard(int* cartes, int *top, deck_t *deck){
	int drawn = drawCard(deck);
	discardCard(deck, drawn);
	cartes[(*top)++] = drawn;
}
// pour initialiser les valeur des tableaux de cartes
void initCards(int **cartes, int *tops, int nbrJoueurs){
	for(int i=0; i<nbrJoueurs; i++){
		tops[i] = 0;
		for(int j=0; j<22; j++){
			cartes[i][j] = 0;
		}
	}
}

// pour calculer la valeur des cartes
// ret: valeur de cartes si elle est 2-9 et 1 si c'est un l'as sinon 10 
int getCardValue(int cardid){
	return getValueFromCardID(cardid)>9?10:getValueFromCardID(cardid);
}

// pour calculer la somme d'un tableau
// arg1: le tableau
// arg2: l'indice jusqu a ou elle va calculer 
// ret: la somme de tableau t
int somme(int *t, int tot){
	int s = 0;
	for(int i=0; i<tot; i++){
		s += t[i];
	}
	return s;
}

// pour mettre le journal a jour
roundInfo update_journal(int* cartesJoueur, int topJoueur, int sommeJoueur,
						 int *cartesBanque, int topBanque, int sommeBanque,
						 int mise, int jet, int win){
	roundInfo res;
	res.mise = mise;
	res.nbJetons = jet;				
	res.topJoueur = topJoueur;				
	res.cartesJoueur = malloc(sizeof(int) * 22);
	for(int j=0; j<topJoueur; j++){
		res.cartesJoueur[j] = cartesJoueur[j];
	}
	res.topBanque = topBanque;				
	res.cartesBanque = malloc(sizeof(int) * 22);
	for(int j=0; j<topBanque; j++){
		res.cartesBanque[j] = cartesBanque[j];
	}
	res.totalBanque = sommeBanque;				
	res.totalJoueur = sommeJoueur;				
	res.gain = win? mise:0;		
	return res;
}

// pour liberer la memoire dans playBanque
void freeMemoryBank(playerInfo* journal, int* mises, int* jetons, int* playerStop, infoJeu info){
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

// pour liberer la memoire dans play
void freeMemory(infoJeu info, int** cartesJoueurs, int **ChildPipe, int **ParentPipe, int *inP, int* outP, int *tops){
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
// pour initialiser une partie
// envoyer le signal pour commencer et melanger le deck
void initialize_round(infoJeu info, deck_t* deck, int* inP, int* outP,
                      int *mises, int* jetons, int *playerStop){
	// envoi de signal pour commencer la partie
	for(int i=0; i<info.nbrJoueurs; i++){
		int sigP = 1;
		if(playerStop[i] == 0){
			write(outP[i], &sigP, sizeof(int));
			read(inP[i], &mises[i], sizeof(int));
			read(inP[i], &jetons[i], sizeof(int));
		}
	}
	// commencer la partie
	printf("\n");
	shuffleDeck(deck);
}

// pour calculer la valeur de win (si un joueur a gagne ou pas)
// ret: le win
int calc_win(int* cartesJoueur, int topJoueur, int *sommeJoueur, int sommeBanque){
	for(int j=0; j<topJoueur; j++){
		(*sommeJoueur) += getCardValue(cartesJoueur[j]);
	}
	if((*sommeJoueur) > 21){
		return 0;
	} else {
		if(sommeBanque > 21){
			return 1;
		} else {
			if((*sommeJoueur) == sommeBanque){
				return -1;
			} else if((*sommeJoueur) > sommeBanque){
				return 1;
			} else {
				return 0;
			}
		}
	}
	return 0;
}

// pour piocher la tour de la banque
// et pour calculer la somme des cartes de la banque
void bank_turn(deck_t *deck, int* cartesBanque, int* topBanque, int *sommeBanque){
	while((*sommeBanque) <= 16){
		pushCard(cartesBanque, topBanque, deck);
		(*sommeBanque) = 0;
		for(int i=0; i<(*topBanque); i++){
			(*sommeBanque) += getCardValue(cartesBanque[i]);
		}
	}
	printf("banque:");
	for(int i=0; i<(*topBanque); i++)
		printf("%c", cardIdToCard(cartesBanque[i]));
	printf(";somme=%d\n", (*sommeBanque));
}

// la fonction principale de la banque
// arg1: les information de jeu d'entree
// arg2: le deck
// arg3: les descripteur pour chaque joueur de sortie (les pipes)
// arg4: les descripteur pour chaque joueur d'entree (les pipes)
// arg5: les cartes des joueurs
// arg6: les indice de les cartes de la fin des cartes des joueurs
void playBanque(infoJeu info, deck_t* deck, int *outP, int *inP, int **cartesJoueurs, int* tops){
	// initialisation des variables et tableaux
	// pour stocker les mises et jetons des joueurs
	int* mises = malloc(sizeof(int)*info.nbrJoueurs);
	int* jetons = malloc(sizeof(int)*info.nbrJoueurs);
	// le journal des infomations de chaque joueur
	playerInfo *journal = malloc(sizeof(playerInfo) * info.nbrJoueurs);
	for(int i=0; i<info.nbrJoueurs; i++){
		journal[i].round = malloc(sizeof(roundInfo) * info.nbrMains);
		journal[i].nbrRounds = 0;
	}
	// un tableau des booleens, 1 veut dire le jouers est en train de joue, 0 veut dire qu'il a arreté
	int* playerStop = malloc(sizeof(int)*info.nbrJoueurs);
	for(int i=0; i<info.nbrJoueurs; i++){
		playerStop[i] = 0;
	}
	// compteur des mains jouee
	int mainsJoue = 0;
	// la boucle principale de la banque
	// on joue tant qu'il y des joueurs en train de jouer et tant que on a pas attendu le nbrMains
	while(somme(playerStop, info.nbrJoueurs) < info.nbrJoueurs && mainsJoue < info.nbrMains){
		initialize_round(info,deck,inP,outP,mises,jetons,playerStop);
		//initialisation des cartes joueurs et banque
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
		// le tableau des signales des joueurs
		int* sigs = calloc(info.nbrJoueurs, sizeof(int));
		// on commence a piocher pour chaque joueur
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
		// pour mettre le playerStop a jour
		for(int i=0; i<info.nbrJoueurs; i++)
			if(sigs[i] <= 0)
				playerStop[i] = 1;
		// pour sortir du jeu quand tout les joueurs ont le signal de fin
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
		bank_turn(deck, cartesBanque, &topBanque, &sommeBanque);
		for(int i=0; i<info.nbrJoueurs; i++){
			if(sigs[i] == 2){
				//calcul de win
				int sommeJoueur = 0;
				int win = calc_win(cartesJoueurs[i], tops[i], &sommeJoueur, sommeBanque);
				//envoi de win
				write(outP[i], &win, sizeof(int));
				//mise a jour journal
				journal[i].round[journal[i].nbrRounds] = update_journal(cartesJoueurs[i], tops[i],   sommeJoueur,
																		cartesBanque,     topBanque, sommeBanque,
																		mises[i],         jetons[i], win);
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
	freeMemoryBank(journal, mises, jetons, playerStop, info);
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
// fonction principal des joueurs
// pour gerer les communication et l'execution principale des joueurs
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
				printf("joueur%d:", i+1);
				for(int j=0; j<top; j++){
					printf("%c",cardIdToCard(cartes[j]));
					somme += getCardValue(cartes[j]);
				}
				printf(";somme=%d,jetons=%d,mise=%d\n", somme,info.nbrJetons, mise);
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
	// Les cartes des joueurs defini par un tableau des entiers et un entier top
	// qui est l'indice de la de la derniere carte dans le tableau
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
	freeMemory(info, cartesJoueurs, ChildPipe, ParentPipe, inP, outP, tops);
}
