#ifndef CLIENT_H
#define CLIENT_H

/////////////////////////////////////////////////////////////////////
//Partie pour client.c
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#define CRLF     "\r\n"
#define PORT     1977

#define BUF_SIZE 1024

void app(const char *address, const char *name);
int init_connection(const char *address, SOCKADDR_IN *sin);
void end_connection(int sock);
int read_server(SOCKET sock, SOCKADDR_IN *sin, char *buffer);
void write_server(SOCKET sock, SOCKADDR_IN *sin, const char *buffer);

/////////////////////////////////////////////////////////////////////
//Partie pour jeuCoteClient.c
#define TAILLE_MAX_PSEUDO 32

typedef struct
{
	int annonce_pli[3];
    int plis_fait[3];
    int carte_joueur[5];
    char pseudo1[TAILLE_MAX_PSEUDO];
	char pseudo2[TAILLE_MAX_PSEUDO];
	char pseudo3[TAILLE_MAX_PSEUDO];
    int dernier_tour;
    int score[3];
    int carte_jouee[3];
}Affichage;

Affichage* init_Affichage();
void liberer_Affichage(Affichage* Affichage);
void afficher(Affichage *affichage);
void afficherClassement(int score[3], char pseudos[3][TAILLE_MAX_PSEUDO]);

int demanderAnnonce(int maxAnnonce, int totalAnnoncee, char *touteLesAnnonce);
int estLeDernierAParler(char *touteLesAnnonce);
int totAnnoncee(char annonce[]);
int choisirCarteAJouer(int cartes[]);
int estDansLaMain(int cartes[], int choix);
int extrairePseudos(char *message, char pseudos[3][TAILLE_MAX_PSEUDO], char *monPseudo);
char *formatage_pseudo(char *pseudo);
int quiAGagnePli(char *buffer);
void recoitMessage(char *buffer, char *name, Affichage *affichage); 
#endif