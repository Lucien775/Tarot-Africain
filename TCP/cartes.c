#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cartes.h" 

//Crée une nouvelle carte
carte* newCarte(int n)
{
    carte* newC = malloc(sizeof(carte));
    if (!newC) 
    {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    newC->numeroCarte = n;
    newC->suivant = NULL;

    return newC;
}

//Ajoute une carte dans le paquet
carte* insererCarte(carte* paquet, int n)
{
    carte* newC = newCarte(n);

    if (paquet == NULL) return newC;

    carte* courant = paquet;
    while (courant->suivant != NULL) courant = courant->suivant;

    courant->suivant = newC;

    return paquet;
}

//Initialise la paquet
carte* initPaquet()
{
    carte* paquet = NULL;
    for (int n = 0; n < 22; n++) 
    {
        paquet = insererCarte(paquet, n);
    }
    return paquet;
}

//Supprime une carte du paquet
carte* supprimerCarte(carte* paquet, int n)
{
    if (paquet == NULL) return paquet;

    if (paquet->numeroCarte == n) 
    {
        carte* temp = paquet->suivant;
        free(paquet);
        return temp;
    }

    carte* courant = paquet;
    while (courant->suivant != NULL && (courant->suivant)->numeroCarte != n) 
    {
        courant = courant->suivant;
    }

    if (courant->suivant != NULL) 
    {
        carte* temp = courant->suivant;
        courant->suivant = courant->suivant->suivant;
        free(temp);
    }

    return paquet;
}

//Tire une carte au hasard 
int tirerUneCarte(carte** paquet)
{
    carte* courant = *paquet;

    int i = 0;
    while (courant != NULL)
    {
        courant = courant->suivant;
        i++;
    }

    if (i == 0) return -1;

    int indiceCarteTiree = rand() % i;

    courant = *paquet;
    for (int i = 0; i < indiceCarteTiree; ++i)
        courant = courant->suivant;

    if (courant != NULL) {
        int carteTiree = courant->numeroCarte;
        *paquet = supprimerCarte(*paquet, carteTiree);
        return carteTiree;
    }

    return -1;
}

//Affiche le paquet
void afficherPaquet(carte* paquet)
{
    carte* courant = paquet;
    while (courant != NULL) 
    {
        printf("%d ", courant->numeroCarte);
        courant = courant->suivant;
    }
    printf("\n");
}

// Libération mémoire
void freePaquet(carte* paquet)
{
    while (paquet != NULL) 
    {
        carte* temp = paquet;
        paquet = paquet->suivant;
        free(temp);
    }
}
