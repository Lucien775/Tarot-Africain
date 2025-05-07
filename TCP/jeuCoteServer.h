#ifndef JEUCOTESERVER_H
#define JEUCOTESERVER_H 

#define TAILLE_MAX_PSEUDO 32

struct Joueur
{
	char pseudo[TAILLE_MAX_PSEUDO];

	struct carte* cartesEnMain;
	int carteJouee;

	int nombrePlisAnnoncees;
	int nombrePlisGagnes;

	int score;
};
typedef struct Joueur Joueur;

struct Partie
{	
	Joueur* joueurs[3];
	struct carte* paquet;

	int laPartieEstLancee;

	int indiceJoueurQuiALaParole;
	int indiceJoueurQuiDistribue;

	int tourDeJeu;
	int nombreDePlis;
	int nombreDeCartesRestantes;
};
typedef struct Partie Partie;

int recoitUnMessage(char* buffer);

int fusionnerDeuxStrings(int indicePremierCharEcrase, char* str1, char* str2);
int quiAGagne(int carte0, int carte1, int carte2);

void recoitUnNouveauJoueur(char pseudoDuNouveauJoueur[TAILLE_MAX_PSEUDO]);
void recoitAnnonce(int indiceJoueur, int recoitNombrePlis);
void recoitCarteJoueur(int indiceJoueur, int valeurCarteJouee);

void lancerUnTourDeJeu();
void envoyerLesCartes();
void distriberLesCartes();
void envoyerMessageTourDAnnonces(int dernierMessTourDAnnonces);
void envoyerCartesJouees(int dernierMessPli);
void envoyerLesScores();
void envoyerLeClassement();


#endif /* guard */