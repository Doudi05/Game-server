#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "message.c"

pid_t read_pid();

void handSIGUSR1(int sig){
	usr1_receive = 1;
}

void handSIGUSR2(int sig){
	usr2_receive = 1;
}

int main(int argc, char const *argv[])
{
	if(argc == 1){
		fprintf(stderr, "Erreur : nombre d'arguments incorrect\n");
		fprintf(stderr, "Usage : ./client <game> [args]...\n");
		exit(EXIT_FAILURE);
	}

	char *game = malloc((2+strlen(argv[1])+5)*sizeof(char));
	strcat(game, argv[1]);
	strcat(game, "_cli");


	if(access(game, X_OK) != 0){
		fprintf(stderr, "Erreur : fichier <game>_cli introuvable.\n");
		exit(EXIT_FAILURE);
	}

	signal(SIGUSR1, handSIGUSR1);
	signal(SIGUSR2, handSIGUSR2);


	pid_t serv_pid = read_pid();
	pid_t cli_pid = getpid();
	char *spid = malloc(6);
	sprintf(spid, "%d", cli_pid);

	char **args;
	size_t len = 0;
	for(int i = 1; i < argc; ++i){
		len += strlen(argv[i]);
	}
	args = malloc((len+strlen(spid))*sizeof(char));
	args[0] = malloc(strlen(spid)*sizeof(char));
	strcat(args[0], spid);
	for(int i = 1; i < argc; ++i){
		args[i] = malloc(strlen(argv[i])*sizeof(char));
		strcat(args[i], argv[i]);
	}

	args[argc] = NULL;

	
	kill(serv_pid, SIGUSR1);
	int fd = open("/tmp/game_server.fifo", O_WRONLY);
	send_argv(fd, args);
	close(fd);

	/*sigset_t sig;
	sigemptyset(&sig);
	sigprocmask(SIG_BLOCK, &sig, NULL);
	sigsuspend(&sig);*/

	while(1){
		if(usr1_receive == 1){
			printf("usr1 reçu, continuité du programme");

			/*char *fifo_0 = "/tmp/game_server./cli";
			strcat(fifo_0, spid);
			strcat(fifo_0, "_0.fifo");

			char *fifo_1 = "/tmp/game_server./cli";
			strcat(fifo_1, spid);
			strcat(fifo_1, "_1.fifo");
			stdout = fopen(fifo_0, "w");

			if(stdout == NULL){
				perror("fopen");
				fprintf(stderr, "Erreur : ficher serveur introuvable ou acces refusé\n");
				exit(EXIT_FAILURE);
			}
			stdin = fopen(fifo_1, "r");

			if(stdin == NULL){
				perror("fopen");
				fprintf(stderr, "Erreur : ficher serveur introuvable ou acces refusé\n");
				exit(EXIT_FAILURE);
			}

			char *seek_game = malloc((2+strlen(argv[1])+8)*sizeof(char));
			strcat(seek_game, argv[1]);
			strcat(seek_game, "_cli");

			char *game_args[argc-1];
			game_args[0] = seek_game;

			for(int i = 1; i < argc-1 ; ++i){
				game_args[i] = strcpy(game_args[i], argv[i]);
			}
			argv[argc-1] = NULL;

			execvp(seek_game, game_args);*/
			usr1_receive = 0;
		}

		if(usr2_receive == 1){
			printf("arret du programme");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}

/**
* cette fonction va lire le pid du programme server present dans le fichier game_server.pid s'il existe
*/
pid_t read_pid(){
	pid_t pid;

	if(access("/tmp/game_server.pid", R_OK) != 0){
		fprintf(stderr, "Erreur : Serveur non démarré...\n");
		exit(EXIT_FAILURE);
	}

	FILE *f = fopen("/tmp/game_server.pid", "r");
	char buf[10];

	fread(buf, sizeof(int), 10, f);

	pid = atoi(buf);

	fclose(f);

	return pid;
}