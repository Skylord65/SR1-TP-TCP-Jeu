/* =================================================================== */
// Progrmame Serveur qui calcule le résultat d'un coup joué à partir
// des coordonnées reçues de la part d'un client "joueur".
// Version ITERATIVE : 1 seul client/joueur à la fois
/* =================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include "tresor.h"

#define N 10

/* =================================================================== */
/* FONCTION PRINCIPALE : SERVEUR ITERATIF                              */
/* =================================================================== */
int main(int argc, char **argv) {

    printf("serveur lancé\n");

    /* Initialisation des variables */

    int sid, res=-1, x_tresor = 4, y_tresor = 5;
    
    /* creation de socket */

    sid = socket(AF_INET, SOCK_STREAM, 0);
    if (sid==-1)
    {
        perror("Socket cretion error");
        return EXIT_FAILURE;
    }

    /* init caracteristiques serveur distant (struct sockaddr_in) */
    
    struct sockaddr_in cli_adr;
    socklen_t cli_adr_len = sizeof(cli_adr);

    struct sockaddr_in sk_addr;
    memset(&sk_addr, 0, sizeof(sk_addr));
    sk_addr.sin_family = AF_INET;
    sk_addr.sin_port = htons(5555); /* host to network byte order */
    sk_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* any interface */

    /* association des identifiants */

    if (bind(sid, (struct sockaddr*) &sk_addr, sizeof(sk_addr)) == -1) {
        perror("Bind error");
        close(sid);
        return EXIT_FAILURE;
    }

    /* lancer l'écoute de demande de connexions */

    if (listen(sid,15)==-1){
        perror("Listen error");
        close(sid);
        return EXIT_FAILURE;
    }
    printf("mise sur écoute:\n");

    /* acceptation de la connexion */

    int client_sock = accept(sid, (struct sockaddr *) &cli_adr, &cli_adr_len);
    if (client_sock<0)
    {
        perror("accept failed");
        close(client_sock);
        exit(EXIT_FAILURE);
    }
    printf("connection acceptée\n");

    /* gestion de l'aléatoire grâce au préfixe "rand" lors du lancement du serveur */

    if (argc == 2 && strcmp(argv[1],"rand")==0) {
        srand(time(NULL));
        x_tresor = 1 + rand()%N;
        y_tresor = 1 + rand()%N;
        printf("position trésor : %d,%d\n", x_tresor,y_tresor);
    }

    while (res!=0){
        
        /* réception d'une requête TCP */

        char mess[6];
        if(recv(client_sock, mess, sizeof(mess), 0)==-1)
        {
            perror("recv error");
            close(client_sock);
            continue;
        }

        printf("message reçu: %s\n", mess);
        
        /* désérialisation du message */

        int lig; //= atoi(&mess[0]);
        int col; //= atoi(&mess[2]);
        sscanf(mess, "%d %d", &lig, &col);

        /* recherche du trésor en fonction des coordonnées */

        res = recherche_tresor(N, x_tresor, y_tresor, lig, col);

        /* sérialisation de la réponse */

        char serialisation[2];
        snprintf(serialisation, sizeof(serialisation), "%d", res);

        printf("résultat trésor : %d\n", res);

        /* envoie de la réponse */

        if(send(client_sock, &serialisation, sizeof(serialisation),0)==-1)
        {
            perror("send error");
            close(client_sock);
            exit(EXIT_FAILURE);
        }

        printf("message envoyé : %s\n", serialisation);

        /* vérification du résulat du trésor pour savoir si le processus peut se terminer */

        if(res==0){
            close(client_sock);
            close(sid);
            break;
        }
        
        
    }
    
    return 0;
} // end main
