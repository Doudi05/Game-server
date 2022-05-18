#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
//#include "libproject.h"
#include "message.h"

main()
{
    pid_t retour ;
    int tube[2], lettre = 0, chiffre = 0 ;
    char k ;
    if (pipe (tube) == -1) { perror ("creation pipe impossible\n") ; exit (1) ; }
    switch (retour = fork ()) {
        case -1 : perror ("Creation impossible") ; 
        exit(1) ;
        case 0  :
        printf ("processus fils\n") ;
        /* le tube est ici ferme en ecriture: le dernier descripteur ouvert   */
        /* en ecriture sur le tube sera dans le processus pere. Quand celui   */
        /* ci fermera ce descripteur, le read effectué par le fils renverra 0 */
        close (tube[1]) ;
        while (read (tube[0], &k, 1) >0)
        if (isdigit (k)) {
            chiffre++ ;
        }else {
            lettre++ ;
        }
        printf ("%d chiffres recus\n", chiffre) ;
        printf ("%d lettres recues\n", lettre) ;
        exit (0) ;
    default :
        printf ("pere: a cree processus %d\n", retour) ;
        close (tube[0]) ;
        while (read (0, &k, 1) >0){
            if (isalnum(k)){
                write (tube[1], &k, 1) ;
            }
        }
        /* le tube est ici ferme en ecriture : un read sur le */
        /* tube vide retournera 0 dans le processus fils      */
        close (tube[1]) ;
        wait (0) ;
        printf ("pere: a recu terminaison fils\n") ;
    }
}