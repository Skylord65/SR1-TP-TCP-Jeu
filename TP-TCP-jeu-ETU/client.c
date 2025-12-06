/* =================================================================== */
// Progrmame Client à destination d'un joueur qui doit deviner la case
// du trésor. Après chaque coup le résultat retourné par le serveur est
// affiché. Le coup consite en une abcsisse et une ordonnée (x, y).
/* =================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define N 10
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define MAGENTA "\033[35m"

/* ====================================================================== */
/*                  Affichage du jeu en mode texte brut                   */
/* ====================================================================== */
void afficher_jeu(int jeu[N][N], int res, int points, int coups) {

    printf("\n************ TROUVEZ LE TRESOR ! ************\n");
    printf("    ");
    for (int i=0; i<10; i++)
        printf("  %d ", i+1);
    printf("\n    -----------------------------------------\n");
    for (int i=0; i<10; i++){
        printf("%2d  ", i+1);
        for (int j=0; j<10; j++) {
            printf("|");
            switch (jeu[i][j]) {
                case -1:
                    printf(" 0 ");
                    break;
                case 0:
                    printf(GREEN " T " RESET);
                    break;
                case 1:
                    printf(YELLOW " %d " RESET, jeu[i][j]);
                    break;
                case 2:
                    printf(RED " %d " RESET, jeu[i][j]);
                    break;
                case 3:
                    printf(MAGENTA " %d " RESET, jeu[i][j]);
                    break;
                default:
                    printf(" 0 ");
            }
        }
        printf("|\n");
    }
    printf("    -----------------------------------------\n");
    printf("Pts dernier coup %d | Pts total %d | Nb coups %d\n", res, points, coups);
}


/* ====================================================================== */
/*                    Fonction principale                                 */
/* ====================================================================== */
int main(int argc, char **argv) {

    int jeu[N][N];
    int lig, col;
    int res = -1, points = 0, coups = 0;

    /* Init args */
    // @IP et @Port
    
    if (argc!=3){
        perror("paramètres");
        return EXIT_FAILURE;
    }
    /* Init jeu */
    
    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++)
            jeu[i][j] = -1;

    /* Creation socket TCP */
    
    int sid;
    sid = socket(AF_INET, SOCK_STREAM, 0);
    if (sid==-1)
    {
        perror("Socket cretion error");
        return EXIT_FAILURE;
    }

    /* Init caracteristiques serveur distant (struct sockaddr_in) */
    
    struct sockaddr_in dst_serv_adr;
    memset(&dst_serv_adr, 0, sizeof(dst_serv_adr));
    dst_serv_adr.sin_family = AF_INET;
    dst_serv_adr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &(dst_serv_adr.sin_addr));

    /* Etablissement connexion TCP avec process serveur distant */

    if (connect(sid, (struct sockaddr*) &dst_serv_adr,sizeof(dst_serv_adr)) == -1) 
    {
        perror("Connection error");
        close(sid);
        return EXIT_FAILURE;
    }

    /* Tentatives du joueur : stoppe quand tresor trouvé */
    
    do {
        afficher_jeu(jeu, res, points, coups);
        printf("\nEntrer le numéro de ligne : ");
        scanf("%d", &lig);
        printf("Entrer le numéro de colonne : ");
        scanf("%d", &col);

        /* Construction requête (serialisation en chaines de caractères) */

        

        char mess[6];
        snprintf(mess, sizeof(mess), "%d %d", lig, col);
        
        printf("j'envoie : ");
        
        /* Envoi de la requête au serveur (send) */

        if(send(sid, mess, sizeof(mess), 0)==-1) {
            perror("send error client");
            close(sid);
            return EXIT_FAILURE;
        }

        printf("%s\n", mess);

        /* Réception du resultat du coup (recv) */
        
        char val[2];
        
        if (recv(sid, val, sizeof(val), 0) == -1) {
            perror("recv error client");
            close(sid);
            return EXIT_FAILURE;
        }

        printf("message reçu : %s\n", val);

        /* Deserialisation du résultat en un entier */

        sscanf(val, "%d", &res);

        /* Mise à jour */
        
        if (lig>=1 && lig<=N && col>=1 && col<=N)
            jeu[lig-1][col-1] = res;
        points += res;
        coups++;
        

    } while (res);

    /* Fermeture connexion TCP */

    close(sid);
    

    /* Terminaison du jeu : le joueur a trouvé le tresor */
    
    afficher_jeu(jeu, res, points, coups);
    printf("\nBRAVO : trésor trouvé en %d essai(s) avec %d point(s)"
            " au total !\n\n", coups, points);
    return 0;
}
