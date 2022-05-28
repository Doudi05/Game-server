#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

volatile int usr1_receive = 0;
volatile int usr2_receive = 0;

char* recv_s={0};
char** recv_a={0};

int send_string(int fd, const char *str);
char *recv_string(int fd);
int send_argv(int fd, char *argv[]);
char **recv_argv(int fd);