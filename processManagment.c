
void play(infoJeu info, deck_t *deck){
	for(int i=0; i<info.nbrJoueurs; i++){
		pid_t pid = fork();
		if(pid < 0){
			perror("fork error");
			exit(-1);
		}
		if(pid){
			card_t **cartesJoueurs = malloc(sizeof(card_t) * info.nbrJoueurs);
			
		} else {
			exit()
		}
	}
}
