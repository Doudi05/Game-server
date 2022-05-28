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
int send_argv(int fd, char *argv[]){
  ssize_t len = 0;
  while(argv[len]!=NULL){
    len++;
  }

  write(fd,&len,sizeof(ssize_t));
  for (size_t i = 0; i<len;i++){
    send_string(fd,argv[i]);
  }
  return 0;
}

/*
recv_argv reçoit la taille d'un tableau et chaque string qu'il contient via le descripteur de fichier fd;
*/
char **recv_argv(int fd){
  size_t len;
  read(fd,&len,sizeof(ssize_t));

  char **tab=calloc(len+1,sizeof(char*));

  for(size_t i = 0; i<len;i++){
    tab[i]=recv_string(fd);
  }

  tab[len]=NULL;
  return tab;

}