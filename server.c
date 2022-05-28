#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

#include "message.c"

int main(int argc, char const *argv[])
{
	struct stat file_stat;
	if(stat("/tmp/game_server.pid", &file_stat) == 0){
		//fichier déjà créé, on a une seule ouverture du serveur
		fprintf(stderr, "Erreur : le serveur est déjà démarré\n");
		exit(1);
	}
	else{
		FILE *f;
		pid_t pid = getpid();
		f = fopen("/tmp/game_server.pid", "w");
		if(f == NULL){
			perror("fopen");
			fprintf(stderr, "Erreur : ficher serveur introuvable ou acces refusé\n");
			exit(EXIT_FAILURE);
		}

		//creation du tube nommé s'il n'existe pas
		if(stat("/tmp/game_server.fifo", &file_stat) != 0){
			mkfifo("/tmp/game_server.fifo", 0664);
			printf("[OK] : Tube nommé.\n");
		}

		//creation du repertoire game_server s'il n'existe pas.
		if(stat("/tmp/game_server.", &file_stat) != 0){
			mkdir("/tmp/game_server.", 0664);
			printf("[OK] : Repertoire server.\n");
		}
		
		//on ecrit le pid du processus dans le fichier créé
		char *spid = malloc(6);
		sprintf(spid, "%d", pid);
		printf("%s", spid);
		fwrite(spid, sizeof(char), 10, f);
		printf("[OK] : Server.\n");
		fclose(f);

		/* handler du signal SIGUSR1 */
		void handSIGUSR1(int sig) {
			usr1_receive = 1;
		}

		signal(SIGUSR1, handSIGUSR1);
		while(1){
			if(usr1_receive == 1){
				printf("signal reçu\n");
				int fd = open("/tmp/game_server.fifo", O_RDONLY);
				char **read = recv_argv(fd);
				
				close(fd);
				//on stocke le pid du client, le nom du jeu ainsi que les potentiels arguments
				pid_t cli_pid = atoi(read[0]);

				char *game = read[1];
				char *seek_game = malloc((2+strlen(game)+8)*sizeof(char));
				strcat(seek_game, "./");
				strcat(seek_game, game);
				strcat(seek_game, "_serv");

				char **args = read+1;

				//test du fichier server du jeu choisi
				if(access(seek_game, X_OK) == 0){
					if(fork() == 0){

						char *fifo_0 = "/tmp/game_server./cli";
						strcat(fifo_0, read[0]);
						strcat(fifo_0, "_0.fifo");
						mkfifo(fifo_0, 0644);

						char *fifo_1 = "/tmp/game_server./cli";
						strcat(fifo_1, read[0]);
						strcat(fifo_1, "_1.fifo");
						mkfifo(fifo_1, 0644);

						//redirection des flux

						stdin = fopen(fifo_0, "r");

						if(stdin == NULL){
							perror("fopen");
							fprintf(stderr, "Erreur : ficher serveur introuvable ou acces refusé\n");
							exit(EXIT_FAILURE);
						}
						stdout = fopen(fifo_1, "w");

						if(stdout == NULL){
							perror("fopen");
							fprintf(stderr, "Erreur : ficher serveur introuvable ou acces refusé\n");
							exit(EXIT_FAILURE);
						}						

						//recouvrement du processus
						execvp(seek_game, args);
					}

					wait(NULL);
				}
				else{
					//envoi de SIGUSR2 au client
					fprintf(stderr, "Erreur : serveur de jeu introuvable");
					kill(cli_pid, SIGUSR2);
				}

				usr1_receive = 0;
			}
			else{
				printf("%d\n", getpid());
			}
		}

		//fermeture du fichier pid
		if(fclose(f) != 0){
			perror("fclose");
			fprintf(stderr, "Erreur : erreur lors de la fermeture du fichier\n");
			exit(EXIT_FAILURE);
		}
	}


	return 0;
}