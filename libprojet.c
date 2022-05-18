#include "libprojet.h"
#include "message.h"

/*
Création du tube nommée "/tmp/pendu.fifo";
*/
void creat_fifo(){
    if(mkfifo("/tmp/pendu.fifo",0644)){
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
}

/*
Tester si le programme pendu est en cours d'execution;
Si pendu existe, lire le pid dans le fichier "/tmp/pendu.pid";
Si ce n'est pas le cas creer le fichier "/tmp/pendu.pid";
*/
int test_pid(int z){

    FILE* f;
    int x;
    f=fopen("/tmp/pendu.pid", "r");
    if(f==NULL){
        if(z==1){
            f=fopen("/tmp/pendu.pid", "w+");
            x= getpid();
            fprintf(f,"%d\n", x );
        }else{
            printf("No pendu program\n");
        }
    }else{
        fscanf(f, "%d ", &x);
    }
    
    fclose(f);
    return x;
}

/* handler du signal SIGUSR1 */
void handSIGUSR1(int sig) {
    usr1_receive = 1;
}

//Récupèrer le temps actuel;
long actuel(){
    time_t seconds;
    seconds = time(NULL);
    return seconds;
}

int main(int argc, char *argv[]){
    unlink("/tmp/period.pid");
    unlink("/tmp/period.fifo");
    test_pid(1);
    creat_fifo();
    FILE* f;
    int fd;

    com=(commande*)malloc(100*sizeof(commande));

    char** recv;
    long m;
    char* endptr;

    mkdir("/tmp/period",0755);
    entree= open("/dev/null", O_WRONLY);

    while(1){
        signal(SIGINT,sig_handler);
        signal(SIGTERM,sig_handler);
        signal(SIGQUIT,sig_handler);
        signal(SIGUSR1,sig_handler);
        signal(SIGUSR2,sig_handler);
        signal(SIGCHLD,sig_handler);
        
        if(usr1_receive){
            fd= open("/tmp/period.fifo", O_RDONLY);
            recv=recv_argv(fd);
            close(fd);

            if(strcmp(recv[0],"remove")==0 && atoi(recv[1])<=count){
                remove_c(com,atoi(recv[1]));
            }else if(strcmp(recv[0],"remove")!=0){
                m=strtol(recv[0],&endptr,10);
                //On calucle le temps en seconde dans le cas d'Epoch
                if(strlen(recv[0])==10 && endptr[0]=='\0') {
                    sprintf(recv[0],"+%ld",m-actuel());
                    puts(recv[0]);
                }
                add(com, recv);
            }
            usr1_receive = 0;
        }
        if(usr2_receive){
            fd=open("/tmp/period.fifo", O_WRONLY);
            char t[16]={0};
            if(count==0){
                send_string(fd,"No commands.\n");
            }else{
                for(int i=0;i<count;i++){
                    sprintf(t,"command No %d: ",com[i].num);
                    send_string(fd,t);
                    if(com[i].start[0]!='+') send_string(fd,"actuel");
                    else {
                        char l[12];
                        snprintf(l,12,"+%d",com[i].start2);
                        send_string(fd,l);
                    }
                    send_string(fd,com[i].period);
                    for(int k=0;k<2;k++){
                        if(com[i].args[k]==NULL) break;
                        send_string(fd,com[i].args[k]);
                    }
                    send_string(fd,"\n");
                }            
            }
            close(fd);
            usr2_receive = 0;
        }
        //Dormir jusqu’à ce qu’une commande doive être exécutée.
        if(count==0){
            pause();
        }
        if (count>0){
            commande_a_executer(com);
        }
    }
    return 0;
}