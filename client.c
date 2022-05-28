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
	strcat(game, "./");
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
	printf("aa\n");
	args[0] = malloc(strlen(spid)*sizeof(char));
	strcat(args[0], spid);
	printf("aaa\n");

	for(int i = 1; i < argc; ++i){
		args[i] = malloc(strlen(argv[i])*sizeof(char));
		strcat(args[i], argv[i]);
	}

	printf("aaaa\n");

	kill(serv_pid, SIGUSR1);
	int fd = open("/tmp/game_server.fifo", O_WRONLY);
	send_argv(fd, args);
	close(fd);

	printf("aaaa\n");

	sigset_t usr1;
	sigemptyset(&usr1);
	sigaddset(&usr1, SIGUSR1);
	printf("aaaaa\n");
	while(1){
		sigprocmask(SIG_BLOCK, &usr1, NULL);
		if(usr1_receive){
			printf("usr1 reçu, continuité du programme");
		}

		if(usr2_receive){
			printf("usr2 reçu, arret du programme");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}

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

	return pid;
}