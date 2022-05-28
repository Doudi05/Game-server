#include "message.h"

/*
send_string envoie un string et sa longueur via le descripteur de fichier (fd);
    Elle prend comme arguments le descripteur de fichier (fd) et le  string (str);
*/
int send_string(int fd, const char *str){      
    size_t len = strlen(str);
    write(fd, &len, sizeof(size_t));
    write(fd, str, len);
    return 0;
}

/*
recv_string lit à partir du descripteur de fichier fd un string et sa longueur; 
    Elle prend comme arguments le descripteur de fichier; 
*/
char *recv_string(int fd){
   size_t len;
   read(fd, &len, sizeof(size_t));
   char *val = malloc((len+1)*sizeof(char));
   read(fd, val, len);
   val[len] = '\0';
   return val;
}


/*
send_argv envoie la longueur d'un tableau et chaque string qu'il contient via le descripteur de fichier fd;
*/
int send_argv(int fd, char* argv[]){
    int nb=0;
    while(1){ //TODO
        if(argv[nb]==NULL){
            break;
        }
        nb++;
    }
    nb=nb-1;
    write(fd,&nb,sizeof(int));
    for(int i=1;i<=nb;i++){
        send_string(fd,argv[i]);
    }

    return 0;
}

/*
recv_argv reçoit la taille d'un tableau et chaque string qu'il contient via le descripteur de fichier fd;
*/
char **recv_argv(int fd){
    int nb;
    //char* r;
    read(fd,&nb,sizeof(int));
    recv_a=(char**)calloc(nb+1,sizeof(char*));
    for(int i=0;i<nb;i++){
        recv_a[i]=recv_string(fd);
    }
    return recv_a;
}