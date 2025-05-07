#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 

#include "client.h" //header

//Déclaration des variables globales
int indiceJoueur =-1;    
int NbDeTour = 1;        
int NdDeDistributeur = 0; 
char pseudos[3][TAILLE_MAX_PSEUDO]; 

//Initialise la structure permettant l'affichage
Affichage* init_Affichage()
{
    Affichage* nouvAffichage = (Affichage*)malloc(sizeof(Affichage));
    if (!nouvAffichage){ 
        printf("Erreur : échec de l'allocation mémoire.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 3; i++)
    {
        nouvAffichage->annonce_pli[i] = -1;
        nouvAffichage->plis_fait[i] = 0;
        nouvAffichage->score[i] = 0;
        nouvAffichage->carte_jouee[i] = -1;
    }

    for (int i = 0; i < 5; i++) {
        nouvAffichage->carte_joueur[i] = -1;
    }

    nouvAffichage->pseudo1[0]='\0';
    nouvAffichage->pseudo2[0]='\0';
    nouvAffichage->pseudo3[0]='\0';

    nouvAffichage->dernier_tour = -1;

    return nouvAffichage;
}

//Libérer la mémoire allouer à la structure Affichage
void liberer_Affichage(Affichage* affichage)
{
    if (affichage) free(affichage);
}

//Retourne l'annonce faite par le joueur
int demanderAnnonce(int maxAnnonce, int totalAnnoncee, char *touteLesAnnonce)
{
    int annonce;     
    int isValid = 0; 

    if (estLeDernierAParler(touteLesAnnonce) == 1) 
    {
        do
        {
            printf("Entrez le nombre de plis que vous pensez pouvoir gagner : ");
            if (scanf("%d", &annonce) == 1)
            {
                if (annonce >= 0 && annonce <= maxAnnonce && annonce != (maxAnnonce - totalAnnoncee)) 
                {
                    isValid = 1; 
                }
                else printf("Valeur invalide\n");
            }
            else printf("Entrée non valide\n");
            while (getchar() != '\n');  
        } while (!isValid); 
    }
    else
    {
        do
        {
            printf("Entrez le nombre de plis que vous pensez pouvoir gagner : ");
            if (scanf("%d", &annonce) == 1) 
            {
                if (annonce >= 0 && annonce <= maxAnnonce) 
                {
                    isValid = 1; 
                }
                else printf("Valeur invalide\n");
            }
            else printf("Entrée non valide\n");
            while (getchar() != '\n');
        } while (!isValid);  
    }
    return annonce;
}

//Renvoi 1 si le joueur est le dernier à faire son annonce, 0 sinon 
int estLeDernierAParler(char *touteLesAnnonce)
{
    if (touteLesAnnonce[0] != 'N' && touteLesAnnonce[1] != 'N') return 1;
    else if (touteLesAnnonce[1] != 'N' && touteLesAnnonce[2] != 'N') return 1;
    else if (touteLesAnnonce[0] != 'N' && touteLesAnnonce[2] !='N') return 1;
    else return 0;
}

//Retourne la somme de toutes les anonnces qui ont déjà été faites
int totAnnoncee(char annonce[])
{
    int sum = 0;
    for (int i = 0; i < 3; ++i) 
    {
        if (annonce[i] == 'N') 
        {
            continue;
        }
        else sum += (annonce[i] - 1); 
    }
    return sum;
}

//Retourne la carte jouée par le joueur
int choisirCarteAJouer(int cartes[]) 
{
    int choix;
    while (1) 
    { 
        printf("Choisissez une carte à jouer : ");
        if (scanf("%d", &choix) != 1) 
        { 
            printf("Entrée invalide. Veuillez entrer un nombre.\n");
            while (getchar() != '\n'); 
            continue;
        }

        if (estDansLaMain(cartes, choix)) 
        {
            if (choix == 0) 
            {
                printf("Choisissez si l'excuse est la meilleure (22) ou la pire (0) carte : ");
                while (scanf("%d", &choix) != 1 || (choix != 0 && choix != 22)) 
                {
                    printf("Entrée invalide. Choisissez 0 ou 22 : ");
                    while (getchar() != '\n'); 
                }
            }
            return choix; 
        } 
        else 
        {
            printf("Carte pas dans votre main. Essayez encore.\n");
            while (getchar() != '\n'); 
        }
    }
}

//Retourne 1 si la carte choisie est bien dans la main du joueur, 0 sinon
int estDansLaMain(int cartes[], int choix) 
{
    for (int i = 0; i < 6; i++) 
    {
        if (cartes[i] == choix) return 1; 
    }
    return 0; 
}


//Retourne l'indice du joueur dans la partie et extrait les pseudos de tous les joueurs
int extrairePseudos(char *message, char pseudos[3][TAILLE_MAX_PSEUDO], char *monPseudo) 
{
    int indexMessage = 1;  
    int indexPseudo = 0;
    int indexCharDansPseudo = 0;
    int indiceJoueur = -1; 

    for (int i = 0; i < 3; ++i)
    {
        pseudos[i][0] = '\0';
    }

    while (message[indexMessage] != '\0' && indexPseudo < 3) 
    {
        if (message[indexMessage] == 1) 
        {
            pseudos[indexPseudo][indexCharDansPseudo] = '\0'; 
            if (strcmp(pseudos[indexPseudo], monPseudo) == 0) indiceJoueur = indexPseudo; 
            indexPseudo++;
            indexCharDansPseudo = 0;
        } 
        else 
        {
            if (indexCharDansPseudo < TAILLE_MAX_PSEUDO - 1) pseudos[indexPseudo][indexCharDansPseudo++] = message[indexMessage];
        }
        indexMessage++;
    }

    if (indexPseudo < 3 && indexCharDansPseudo > 0) 
    {
        pseudos[indexPseudo][indexCharDansPseudo] = '\0';
        if (strcmp(pseudos[indexPseudo], monPseudo) == 0) indiceJoueur = indexPseudo;
    }

    return indiceJoueur;
}

//Formate les pseudos pour qu'ils respectent le format nécessaire à l'affichage
char *formatage_pseudo(char *pseudo)
{
    char pseudo_formate[32];
    int compteur_carac = 0;
    int i = 0;

    while(pseudo[i] != '\0' && i < TAILLE_MAX_PSEUDO)
    {
        compteur_carac ++;
        i++;
    }

    for(int i = 0; i < compteur_carac; i++)
    {
        pseudo_formate[i] = pseudo[i];
    }

    for(int i = compteur_carac; i< TAILLE_MAX_PSEUDO -1; i++)
    {
        pseudo_formate[i] = ' ';
    }
    pseudo_formate[TAILLE_MAX_PSEUDO -1] = '\0';

    for(int i = 0; i < TAILLE_MAX_PSEUDO; i++)
    {
        pseudo[i] = pseudo_formate[i];
    }

    return pseudo;
}

//Retourne l'indice du joueur qui a gagné le pli
int quiAGagnePli(char *buffer)
{
    int indiceMax = -1;
    int max = 0;
    for (int i = 1; i < 4; ++i) 
    {
        if(buffer[i] > max)
        {
            indiceMax = i - 1;
            max = buffer[i];
        }
    }
    return indiceMax;
}

//Modifie le buffer  et l'affichage en fonction du buffer
void recoitMessage(char *buffer, char *name, Affichage* affichage) 
{
    switch (buffer[0]) 
    {
        case 1: 
            
            if(affichage->dernier_tour == 1) 
            {
                for (int indiceJoueur = 0; indiceJoueur < 3; ++indiceJoueur) 
                {
                    int indiceCarte = 3 + indiceJoueur;
                    if (buffer[indiceCarte] != 1) 
                    {    
                        affichage->carte_jouee[indiceJoueur] =  buffer[indiceCarte] - 2; 
                    } 
                    else affichage->carte_jouee[indiceJoueur] = -1; 
                }

                afficher(affichage);
                printf("\nC'est le dernier tour !\n");
                printf("Vous voyez les cartes des 2 autres joueurs\n");
                sleep(10);
            }
            else  
            {
                for (int i = 1; i < 6; ++i)
                {
                    affichage->carte_joueur[i-1] = buffer[i] - 2; 
                }

                afficher(affichage);
            }
            buffer[0] = '\0'; //Vide le buffer
            break;
        case 2: 

            for (int i = 2; i < 5; ++i)
            {
                if (buffer[i] != 'N') affichage->annonce_pli[i - 2] = buffer[i] - 1;
            }

            if (buffer[5] != 'N') 
            {
                afficher(affichage);

                if ((buffer[5] - 1) == indiceJoueur) 
                {
                    printf("\nC'est à toi de de faire une annonce !\n"); 

                    char touteLesAnnonce[3]; 
                    memcpy(touteLesAnnonce, &buffer[2], 3);

                    int maxAnnonce = buffer[1] - 1; 
                    if (affichage->dernier_tour == 1) maxAnnonce = 1; 
                    int totalAnnoncee = totAnnoncee(touteLesAnnonce); 
                    affichage->annonce_pli[indiceJoueur] = demanderAnnonce(maxAnnonce, totalAnnoncee, touteLesAnnonce); 

                    buffer[0] = 2;          
                    buffer[1] = buffer[5];  
                    buffer[2] = affichage->annonce_pli[indiceJoueur] + 1;
                    buffer[3] = '\0';        
                }
                else  buffer[0] = '\0'; 
            }
            else buffer[0] = '\0';

            afficher(affichage);
            break;
        case 3: 

            indiceJoueur = extrairePseudos(buffer, pseudos, name); 

            snprintf(affichage->pseudo1, TAILLE_MAX_PSEUDO, "%s", pseudos[0]);
            snprintf(affichage->pseudo2, TAILLE_MAX_PSEUDO, "%s", pseudos[1]);
            snprintf(affichage->pseudo3, TAILLE_MAX_PSEUDO, "%s", pseudos[2]);

            afficher(affichage);

            if (affichage->pseudo3[0] != '\0') affichage->dernier_tour = 0;
            
            buffer[0] = '\0'; 
            break;
        case 4:

            for (int i = 1; i < 4; ++i)
            {
                if (buffer[i] != 'N') affichage->carte_jouee[i - 1] = buffer[i] - 2;
            }
            
            if (buffer[4] != 'N' && affichage->dernier_tour != 1) 
            {
                afficher(affichage);

                if ((buffer[4] - 1) == indiceJoueur) 
                {
                    printf("\nC'est à toi de jouer !\n");

                    affichage->carte_jouee[indiceJoueur]= choisirCarteAJouer(affichage->carte_joueur); 

                    buffer[0] = 4;          
                    buffer[1] = buffer[4];  
                    buffer[2] = affichage->carte_jouee[indiceJoueur] + 2;  
                    buffer[3] = '\0';       
                }
                else buffer[0] ='\0'; 
            } 
            else
            {
                int indiceGagnant = quiAGagnePli(buffer);
                affichage->plis_fait[indiceGagnant] ++;
                afficher(affichage);
                buffer[0] = '\0'; 
            } 
            break;
        case 5: 

            for (int i = 0; i < 3; i++)
            {
                affichage->annonce_pli[i] = -1;
                affichage->plis_fait[i] = 0;
                affichage->carte_jouee[i] = -1;
            }

            for (int i = 0; i < 5; i++) {
                affichage->carte_joueur[i] = -1;
            }

            for (int i = 1; i < 4; ++i)
            {
                affichage->score[i-1] = buffer[i] - 1;
            }

            afficher(affichage);

            if(affichage->dernier_tour == 1)
            {
                NdDeDistributeur ++;
                if (NdDeDistributeur == 3)
                {
                    afficherClassement(affichage->score,pseudos);
                }
            }

            if(NbDeTour == 5)
            {
                affichage->dernier_tour = 1;
                NbDeTour = 0;
            }
            else 
            {
                affichage->dernier_tour = 0;
                NbDeTour++; 
            }

            buffer[0] = '\0'; 
            break;
        default: 
            printf("\nMessage inconnu reçu du serveur.\n");
            buffer[0] = '\0'; 
            break;
    }
}

//Affiche le classement en fin de partie
void afficherClassement(int score[3], char pseudos[3][TAILLE_MAX_PSEUDO])
{
    system("clear");
    printf("La partie est terminée !\n");

    int classement[3] = {0, 1, 2}; 

    for (int i = 0; i < 2; ++i) 
    {
        for (int j = i + 1; j < 3; ++j)
        {
            if (score[classement[j]] < score[classement[i]])
            {
                int temp = classement[i];
                classement[i] = classement[j];
                classement[j] = temp;
            }
        }
    }

    printf("\nClassement final :\n");
    for (int i = 0; i < 3; ++i)
    {
        char *couleur;
        if (i == 0)
            couleur = "\033[33m"; 
        else if (i == 1)
            couleur = "\033[37m"; 
        else
            couleur = "\033[31m"; 

        printf("%s%d. %s avec %d points\033[0m\n", couleur, i + 1, pseudos[classement[i]], score[classement[i]]);
    }
    sleep(30);
}


//Affiche les informations de jeu
void afficher(Affichage *affichage)
{
    system("clear");

    char *pseudo_formate1 = formatage_pseudo(affichage->pseudo1);
    char *pseudo_formate2 = formatage_pseudo(affichage->pseudo2);
    char *pseudo_formate3 = formatage_pseudo(affichage->pseudo3);
 
    char score_formate1[13];
    char score_formate2[13];
    char score_formate3[13];

    if( affichage->score[0] > 9) sprintf(score_formate1, "    %d     ", affichage->score[0]);
    else sprintf(score_formate1, "     %d     ", affichage->score[0]);

    if( affichage->score[1] > 9) sprintf(score_formate2, "    %d     ", affichage->score[1]);
    else sprintf(score_formate2, "     %d     ", affichage->score[1]);

    if( affichage->score[2] > 9) sprintf(score_formate3, "    %d     ", affichage->score[2]);
    else sprintf(score_formate3, "     %d     ", affichage->score[2]);

    char nombre_pli_annonce_formate1[26];
    char nombre_pli_annonce_formate2[26];
    char nombre_pli_annonce_formate3[26];

    if(affichage->annonce_pli[0] == -1) sprintf(nombre_pli_annonce_formate1, "                         ");
    else sprintf(nombre_pli_annonce_formate1, "            %d            ", affichage->annonce_pli[0]);

    if(affichage->annonce_pli[1] == -1) sprintf(nombre_pli_annonce_formate2, "                         ");
    else sprintf(nombre_pli_annonce_formate2, "            %d            ", affichage->annonce_pli[1]);

    if(affichage->annonce_pli[2] == -1) sprintf(nombre_pli_annonce_formate3, "                         ");
    else sprintf(nombre_pli_annonce_formate3, "            %d            ", affichage->annonce_pli[2]);

    char nombre_pli_fait_formate1[23];
    char nombre_pli_fait_formate2[23];
    char nombre_pli_fait_formate3[23];

    sprintf(nombre_pli_fait_formate1, "          %d          ", affichage->plis_fait[0]);
    sprintf(nombre_pli_fait_formate2, "          %d          ", affichage->plis_fait[1]);
    sprintf(nombre_pli_fait_formate3, "          %d          ", affichage->plis_fait[2]);

    char carte_jouee_formate1[17];
    char carte_jouee_formate2[17];
    char carte_jouee_formate3[17];
    
    if(affichage->carte_jouee[0] == -1) sprintf(carte_jouee_formate1,"                ");
    else if(affichage->carte_jouee[0] < 10) sprintf(carte_jouee_formate1, "       %d       ", affichage->carte_jouee[0]);
    else sprintf(carte_jouee_formate1, "        %d      ", affichage->carte_jouee[0]);

    if(affichage->carte_jouee[1] == -1) sprintf(carte_jouee_formate2,"                ");
    else if(affichage->carte_jouee[1] < 10) sprintf(carte_jouee_formate2, "       %d       ", affichage->carte_jouee[1]);
    else sprintf(carte_jouee_formate2, "        %d      ", affichage->carte_jouee[1]);

    if(affichage->carte_jouee[2] == -1) sprintf(carte_jouee_formate3,"                ");
    else if(affichage->carte_jouee[2] < 10) sprintf(carte_jouee_formate3, "       %d       ", affichage->carte_jouee[2]);
    else sprintf(carte_jouee_formate3, "        %d      ", affichage->carte_jouee[2]);

    printf("\t\t Partie de Tarot Africain\n");
    
    printf("-------------------------------------------------------------------------------------------------------------\n");

    printf("             JOUEUR             |   SCORE   |   NOMBRE PLI ANNONCES   |   NOMBRE PLI FAIT   |   CARTE JOUEE   \n");

    printf("-------------------------------------------------------------------------------------------------------------\n");

    printf("%s |%s|%s|%s|%s\n", pseudo_formate1, score_formate1, nombre_pli_annonce_formate1, nombre_pli_fait_formate1, carte_jouee_formate1); 

    printf("-------------------------------------------------------------------------------------------------------------\n");

    printf("%s |%s|%s|%s|%s\n", pseudo_formate2, score_formate2, nombre_pli_annonce_formate2, nombre_pli_fait_formate2, carte_jouee_formate2);

    printf("-------------------------------------------------------------------------------------------------------------\n");

    printf("%s |%s|%s|%s|%s\n", pseudo_formate3, score_formate3, nombre_pli_annonce_formate3, nombre_pli_fait_formate3, carte_jouee_formate3);

    printf("-------------------------------------------------------------------------------------------------------------\n");

    if(affichage->dernier_tour == 0) 
    {
        printf(" Cartes disponibles : ");

        for(int i = 0; i < 5; i++) 
        {
            if(affichage->carte_joueur[i] != -1)
            {
                printf("%d     ", affichage->carte_joueur[i]);
            }
        }
        printf("\n");
    } 
}