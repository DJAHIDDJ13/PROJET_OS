#include <sys/types.h>
card_t *pushCard(card_t *carte, const int cardId){
	card_t *tmp = malloc(sizeof(card_t));
	tmp->value = cardId;
	tmp->next = NULL;
	if(carte == NULL){
		return tmp;
	}
	tmp->next = carte;
	return tmp;
}
card_t *pushCards(card_t* cartes, deck_t *deck, int nbrOfCards){
	for(int i=0; i<nbrOfCards; i++){
		int drawn = drawCard(deck);
		discardCard(deck, drawn);
		cartes = pushCard(cartes, drawn);
	}
	return cartes;
}

void play(infoJeu info, deck_t *deck){
	pid_t* pids = malloc(sizeof(pid_t) * info.nbrJoueurs);
	for(int i=0; i<info.nbrJoueurs; i++){
		int ParentPipe[2];
		int ChildPipe[2];
		if(pipe(ParentPipe) || pipe(ChildPipe)) {
			perror("pipe error");
			exit(-1);
		}
		pid_t pid = fork();
		if(pid<0){
			perror("Fork error");
			exit(-1);
		}
		if(!pid){
			int in, out;
			in = ChildPipe[0];
			out = ParentPipe[1];
			close(ChildPipe[1]);
			close(ParentPipe[0]);
			char* s = NULL;
			read(in, s, 12);
			printf("Hi parent, you said: %s\n", s);
			exit(0);
		} else {
			int in, out;
			in = ChildPipe[1];
			out = ParentPipe[0];
			close(ChildPipe[0]);
			close(ParentPipe[1]);
			write(out, "Hello child", 12);
			pids[i] = pid;
		}
	}
	return pids;
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
