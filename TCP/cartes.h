#ifndef CARTES_H
#define CARTES_H

struct carte 
{
    int numeroCarte;
    struct carte* suivant;
};
typedef struct carte carte;


carte* newCarte(int n);
carte* insererCarte(carte* paquet, int n);
carte* initPaquet();
carte* supprimerCarte(carte* paquet, int n);
int tirerUneCarte(carte** paquet);
void afficherPaquet(carte* paquet);
void freePaquet(carte* paquet);

#endif 