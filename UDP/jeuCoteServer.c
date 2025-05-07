#include <stdio.h>
#include <stdlib.h>
#include <time.h>    
#include <unistd.h>  

#include "server.h"
#include "cartes.h"
#include "jeuCoteServer.h"

//Variable globale
Partie* partie;

//Détermine la nature du message reçu
int recoitUnMessage(char* buffer, Client clients[MAX_CLIENTS], SOCKET sock)
{
	int i = 0;

	printf("nouveau message : ");
	while (buffer[i] != '\0')
	{
		printf("%d, ", (int) buffer[i]);
		i++;
	}
	printf("\n");

	switch (buffer[0])
	{
	case 1:
		int i = 0;
		char* pseudoDuNouveauJoueur = malloc((TAILLE_MAX_PSEUDO +1) * sizeof(char));

		while (buffer[i+1] != '\0' && i < TAILLE_MAX_PSEUDO)
			pseudoDuNouveauJoueur[i++] = buffer[i+1];

		pseudoDuNouveauJoueur[i] = '\0';

		recoitUnNouveauJoueur(pseudoDuNouveauJoueur, clients, sock); 
		free(pseudoDuNouveauJoueur);
	case 2:
		recoitAnnonce((int) buffer[1] -1, (int) buffer[2] - 1, clients, sock); 
	case 4:
		recoitCarteJoueur((int) buffer[1] -1, (int) buffer[2] -2, clients, sock); 
	}

	return 0;
}

//Concatène str2 dans str1 et retourne l'indice du dernier caractère copié dans str1
int fusionnerDeuxStrings(int indicePremierCharEcrase, char* str1, char* str2)
{
	int i = indicePremierCharEcrase;

	while (str2[i - indicePremierCharEcrase] != '\0')
		str1[i++] = str2[i - indicePremierCharEcrase];

	return i;
}

/*Gère le cas où on un nouveau joueur est arrivé*/
void recoitUnNouveauJoueur(char pseudoDuNouveauJoueur[TAILLE_MAX_PSEUDO], Client clients[MAX_CLIENTS], SOCKET sock)
{
	if (partie->laPartieEstLancee == 0)
	{
		Joueur* nouveauJoueur = (Joueur*) malloc(sizeof(Joueur));
		
		int i = 0;
		while (pseudoDuNouveauJoueur[i] != '\0' && i < TAILLE_MAX_PSEUDO) nouveauJoueur->pseudo[i++] = pseudoDuNouveauJoueur[i];
		
		nouveauJoueur->cartesEnMain = NULL;
		nouveauJoueur->carteJouee = -1;
		nouveauJoueur->nombrePlisAnnoncees = -1;
		nouveauJoueur->nombrePlisGagnes = 0;
		nouveauJoueur->score = 0;

		int nbDeJoueur;

		if (partie->joueurs[0] == NULL)
		{
			partie->joueurs[0] = nouveauJoueur;
			nbDeJoueur = 1;
		}
		else if (partie->joueurs[1] == NULL)
		{
			partie->joueurs[1] = nouveauJoueur;
			nbDeJoueur = 2;
		}
		else if (partie->joueurs[2] == NULL)
		{
			partie->joueurs[2] = nouveauJoueur;
			nbDeJoueur = 3;
		}

		char* messageAvecLesPseudos = malloc((1 + (TAILLE_MAX_PSEUDO+1) * 3) * sizeof(char));
		messageAvecLesPseudos[0] = 3; 
		int indiceCharAEcraser = 1;

		for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur)
		{
			if (partie->joueurs[indiceJoueur] != NULL) indiceCharAEcraser = fusionnerDeuxStrings(indiceCharAEcraser, messageAvecLesPseudos, partie->joueurs[indiceJoueur]->pseudo);
			messageAvecLesPseudos[indiceCharAEcraser++] = 1;		
		}

		messageAvecLesPseudos[indiceCharAEcraser-1] = '\0';

		for (int indiceJoueur = 0; indiceJoueur < nbDeJoueur; ++indiceJoueur) write_client(sock, &clients[indiceJoueur].sin, messageAvecLesPseudos);
		
		free(messageAvecLesPseudos);

		if (partie->joueurs[2] != NULL) lancerUnTourDeJeu(clients, sock);
	}
}

//Gère le cas où on reçoit des annonces
void recoitAnnonce(int indiceJoueur, int nombreDePlis, Client clients[MAX_CLIENTS], SOCKET sock)
{
	if (indiceJoueur == partie->indiceJoueurQuiALaParole)
	{
		partie->joueurs[partie->indiceJoueurQuiALaParole]->nombrePlisAnnoncees = nombreDePlis;

		if (partie->joueurs[(partie->indiceJoueurQuiALaParole + 1) % 3]->nombrePlisAnnoncees != -1)
			if (partie->joueurs[0]->nombrePlisAnnoncees + partie->joueurs[1]->nombrePlisAnnoncees + partie->joueurs[2]->nombrePlisAnnoncees == partie->nombreDePlis)
				printf("ERREUR : NE PEUT PAS ANNONCER CE NOMBRE DE PLIS\n");
		
		if (partie->joueurs[partie->indiceJoueurQuiALaParole]->nombrePlisAnnoncees > partie->nombreDePlis || partie->joueurs[partie->indiceJoueurQuiALaParole]->nombrePlisAnnoncees < 0)
			printf("ERREUR : NE PEUT PAS ANNONCER CE NOMBRE DE PLIS\n");

		partie->indiceJoueurQuiALaParole = (partie->indiceJoueurQuiALaParole + 1) % 3;

		if (partie->joueurs[partie->indiceJoueurQuiALaParole]->nombrePlisAnnoncees == -1)
			envoyerMessageTourDAnnonces(0, clients, sock);

		else
		{
			envoyerMessageTourDAnnonces(1, clients, sock);

			printf("TOUR DE CARTES\n");

			if (partie->nombreDePlis != 0)
				envoyerCartesJouees(0, clients, sock);

			else
			{
				for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur)
					partie->joueurs[indiceJoueur]->carteJouee = partie->joueurs[indiceJoueur]->cartesEnMain->numeroCarte;

				int indiceJoueurQuiAGagne = quiAGagne(partie->joueurs[0]->carteJouee, partie->joueurs[1]->carteJouee, partie->joueurs[2]->carteJouee);

				partie->joueurs[indiceJoueurQuiAGagne]->nombrePlisGagnes++;

				envoyerCartesJouees(1, clients, sock);

				partie->tourDeJeu++;
				if (partie->tourDeJeu < 3)
				{
					partie->indiceJoueurQuiDistribue = (partie->indiceJoueurQuiDistribue + 1) %3;
					partie->indiceJoueurQuiALaParole = partie->indiceJoueurQuiDistribue;
					partie->nombreDePlis = 6;

					lancerUnTourDeJeu(clients, sock);
				}
				else
				{
					printf("----- C'EST FINIT -----\n");
					for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur)
						printf("	score joueur %d : %d\n", indiceJoueur, partie->joueurs[indiceJoueur]->score);

					envoyerLesScores(clients, sock);
				}
			}
		}
	}
}

//Retourne l'indice du joueur qui a gagné le pli
int quiAGagne(int carte0, int carte1, int carte2)
{
	if (carte0 > carte1)
	{
		if (carte0 > carte2)
			return 0;
		return 2;
	}
	if (carte1 > carte2)
		return 1;
	return 2;
}

//Gère la cas où l'on reçoit la carte d'un joueur
void recoitCarteJoueur(int indiceJoueur, int valeurCarteJouee, Client clients[MAX_CLIENTS], SOCKET sock)
{
	if (indiceJoueur == partie->indiceJoueurQuiALaParole)
	{
		partie->joueurs[partie->indiceJoueurQuiALaParole]->carteJouee = valeurCarteJouee;

		printf("le joueur %d joue la carte %d\n", indiceJoueur, valeurCarteJouee);

		int laCarteEstLegale = 0;

		carte* ptrCarte = partie->joueurs[indiceJoueur]->cartesEnMain;
		while (ptrCarte != NULL)
		{
			if (ptrCarte->numeroCarte == valeurCarteJouee %22) laCarteEstLegale = 1;
			ptrCarte = ptrCarte->suivant;
		}

		if (laCarteEstLegale == 0) printf("ERREUR : NE PEUT PAS JOUER CETTE CARTE\n");

		partie->joueurs[indiceJoueur]->cartesEnMain = supprimerCarte(partie->joueurs[indiceJoueur]->cartesEnMain, valeurCarteJouee %22);

		partie->indiceJoueurQuiALaParole = (partie->indiceJoueurQuiALaParole + 1) % 3;

		if (partie->joueurs[partie->indiceJoueurQuiALaParole]->carteJouee == -1)
			envoyerCartesJouees(0, clients, sock);

		else
		{
			envoyerCartesJouees(1, clients, sock);

			partie->nombreDeCartesRestantes--;

			int indiceJoueurQuiAGagne = quiAGagne(partie->joueurs[0]->carteJouee, partie->joueurs[1]->carteJouee, partie->joueurs[2]->carteJouee);

			printf("LE JOUEUR QUI A GAGNE EST %d\n", indiceJoueurQuiAGagne);

			partie->joueurs[indiceJoueurQuiAGagne]->nombrePlisGagnes++;

			for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur)
				partie->joueurs[indiceJoueur]->carteJouee = -1;

			if (partie->nombreDeCartesRestantes > 0)
			{
				partie->indiceJoueurQuiALaParole = indiceJoueurQuiAGagne;
	
				envoyerLesCartes(clients, sock);
				envoyerCartesJouees(0, clients, sock);
			}
			else
			{
				partie->indiceJoueurQuiALaParole = partie->indiceJoueurQuiDistribue;

				printf("TOUR FINIT\n");

				lancerUnTourDeJeu(clients, sock);
			}
		}
	}
}

//Lance un tour de jeu
void lancerUnTourDeJeu(Client clients[MAX_CLIENTS], SOCKET sock)
{
	if (partie->laPartieEstLancee == 0)
	{
		printf("on lance la partie avec %s, %s et %s\n", partie->joueurs[0]->pseudo, partie->joueurs[1]->pseudo, partie->joueurs[2]->pseudo);

		partie->laPartieEstLancee = 1;
		
		partie->tourDeJeu = 0;
		partie->nombreDePlis = 5;
		partie->nombreDeCartesRestantes = 5;

		partie->indiceJoueurQuiDistribue = rand() % 3;
		partie->indiceJoueurQuiALaParole = partie->indiceJoueurQuiDistribue;
	}
	else
	{
		for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur)
		{
			partie->joueurs[indiceJoueur]->score += abs(partie->joueurs[indiceJoueur]->nombrePlisAnnoncees - partie->joueurs[indiceJoueur]->nombrePlisGagnes);
			partie->joueurs[indiceJoueur]->nombrePlisAnnoncees = -1;
			partie->joueurs[indiceJoueur]->nombrePlisGagnes = 0;
			partie->joueurs[indiceJoueur]->carteJouee = -1;
		}

		envoyerLesScores(clients, sock);

		partie->nombreDePlis--;
		partie->nombreDeCartesRestantes = partie->nombreDePlis;
	}

	distriberLesCartes();
	envoyerLesCartes(clients, sock);

	envoyerMessageTourDAnnonces(0, clients, sock);
}

//Envoie les cartes qui sont dans la main de chaque joueur
void envoyerLesCartes(Client clients[MAX_CLIENTS], SOCKET sock)
{	
	sleep(1);

	for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur)
	{
		char* messageCartesEnMain = malloc(7 * sizeof(char));
		for (int i = 0; i < 6; ++i) messageCartesEnMain[i] = 1; 
		messageCartesEnMain[6] = '\0';

		if (partie->nombreDePlis != 0)
		{
			int i = 1;
			carte* ptrCarte = partie->joueurs[indiceJoueur]->cartesEnMain;
			while (ptrCarte != NULL)
			{
				messageCartesEnMain[i++] = ptrCarte->numeroCarte +2;
				ptrCarte = ptrCarte->suivant;
			}
		}
		else
		{
			messageCartesEnMain[3 + (indiceJoueur +2) %3] = partie->joueurs[(indiceJoueur +2) %3]->cartesEnMain->numeroCarte +2;
			messageCartesEnMain[3 + (indiceJoueur +1) %3] = partie->joueurs[(indiceJoueur +1) %3]->cartesEnMain->numeroCarte +2;
		}

		write_client(sock, &clients[indiceJoueur].sin, messageCartesEnMain);
		free(messageCartesEnMain);
	}
}

//Distribue les cartes à chauqe joueur
void distriberLesCartes()
{
	partie->paquet = initPaquet();
	int carteTiree; 

	for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur)
	{
		partie->joueurs[indiceJoueur]->cartesEnMain = NULL;

		for (int indiceCarte = 0; indiceCarte < (partie->nombreDePlis >= 1 ? partie->nombreDePlis : 1); ++indiceCarte)
		{
			carteTiree = tirerUneCarte(&(partie->paquet));

			partie->joueurs[indiceJoueur]->cartesEnMain = insererCarte(partie->joueurs[indiceJoueur]->cartesEnMain, carteTiree);
		}
	}
}

//Envoie les annonces à tous les joueurs
void envoyerMessageTourDAnnonces(int dernierMessTourDAnnonces,Client clients[MAX_CLIENTS], SOCKET sock)
{	
	sleep(1); *

	char* messageTourDAnnonces = malloc(7 * sizeof(char));

	messageTourDAnnonces[0] = 2;
	messageTourDAnnonces[1] = partie->nombreDePlis +1; 
	messageTourDAnnonces[2] = 'N';
	messageTourDAnnonces[3] = 'N';
	messageTourDAnnonces[4] = 'N';

	if (dernierMessTourDAnnonces)
		messageTourDAnnonces[5] = 'N';
	else
		messageTourDAnnonces[5] = partie->indiceJoueurQuiALaParole +1;
	messageTourDAnnonces[6] = '\0';

	printf("on demande au joueur %d de parler\n", partie->indiceJoueurQuiALaParole);

	for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur)
		if (partie->joueurs[indiceJoueur]->nombrePlisAnnoncees != -1) 
			messageTourDAnnonces[indiceJoueur+2] = partie->joueurs[indiceJoueur]->nombrePlisAnnoncees + 1;

	for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur)
		write_client(sock, &clients[indiceJoueur].sin ,messageTourDAnnonces);

	free(messageTourDAnnonces);
}

//Envoie les cartes jouées à tous les joueurs
void envoyerCartesJouees(int signal, Client clients[MAX_CLIENTS], SOCKET sock)
{
	sleep(1); 

	char* messageCartesJouees = malloc(6 * sizeof(char));

	messageCartesJouees[0] = 4; 

	for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur)
		messageCartesJouees[indiceJoueur + 1] = partie->joueurs[indiceJoueur]->carteJouee + 2; // carte +2
	
	if (signal == 0)
		messageCartesJouees[4] = partie->indiceJoueurQuiALaParole +1; 
	
	if (signal == 1)
		messageCartesJouees[4] = 'N';

	messageCartesJouees[5] = '\0';

	for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur)
		write_client(sock, &clients[indiceJoueur].sin, messageCartesJouees);
	free(messageCartesJouees);
}

//Envoie les scores à tous les joueurs
void envoyerLesScores(Client clients[MAX_CLIENTS], SOCKET sock)
{
	sleep(1); 

	char* messageScores = malloc(5 * sizeof(char));

	messageScores[0] = 5; 

	for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur)
		messageScores[indiceJoueur + 1] = partie->joueurs[indiceJoueur]->score + 1; 
	
	messageScores[4] = '\0';

	for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur)
		write_client(sock, &clients[indiceJoueur].sin, messageScores);
	free(messageScores);
}


int main()
{
	srand( time( NULL ) );

	partie = malloc(sizeof(Partie));

	partie->laPartieEstLancee = 0;

	partie->joueurs[0] = NULL;
	partie->joueurs[1] = NULL;
	partie->joueurs[2] = NULL;

	app();

	return 0;
}