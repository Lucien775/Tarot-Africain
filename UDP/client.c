#include <stdio.h>      
#include <stdlib.h>     
#include <errno.h>      // Pour la gestion des erreurs
#include <string.h>     

#include "client.h"     //header du côté client

// Fonction principale de l'application
void app(const char *address, const char *name)
{
   SOCKADDR_IN sin = { 0 };
   SOCKET sock = init_connection(address, &sin);
   char buffer[BUF_SIZE];
   fd_set rdfs;                             

   write_server(sock, &sin, name);              

   Affichage* affichage = init_Affichage();

   while(1)
   {
      FD_ZERO(&rdfs);
      FD_SET(sock, &rdfs);

      if(select(sock + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      else if(FD_ISSET(sock, &rdfs))
      {
         int n = read_server(sock, &sin, buffer);
         if(n == 0)
         {
            printf("Le serveur s'est déconnecté.\n");
            break;
         }

         recoitMessage(buffer, name, affichage); 

         if (buffer[0] != '\0')
         { 
            write_server(sock,&sin,buffer); 
         }
         else continue; 
      }
   }
   printf("Fermeture de la connexion.\n");
   end_connection(sock);
   liberer_Affichage(affichage);
}


// Fonction pour initialiser une connexion socket
int init_connection(const char *address, SOCKADDR_IN *sin)
{
   SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
   struct hostent *hostinfo;

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   hostinfo = gethostbyname(address);
   if (hostinfo == NULL)
   {
      fprintf (stderr, "Unknown host %s.\n", address);
      exit(EXIT_FAILURE);
   }

   sin->sin_addr = *(IN_ADDR *) hostinfo->h_addr;
   sin->sin_port = htons(PORT);
   sin->sin_family = AF_INET;

   return sock;
}

// Fonction pour terminer une connexion socket
void end_connection(int sock)
{
   closesocket(sock);
}

// Fonction pour lire un message depuis le serveur
int read_server(SOCKET sock, SOCKADDR_IN *sin, char *buffer)
{
   int n = 0;
   size_t sinsize = sizeof *sin;

   if((n = recvfrom(sock, buffer, BUF_SIZE - 1, 0, (SOCKADDR *) sin, (socklen_t *)&sinsize)) < 0)
   {
      perror("recvfrom()");
      exit(errno);
   }

   buffer[n] = 0;

   return n;
}
// Fonction pour envoyer un message au serveur
void write_server(SOCKET sock, SOCKADDR_IN *sin, const char *buffer)
{
   if(sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *) sin, sizeof *sin) < 0)
   {
      perror("sendto()");
      exit(errno);
   }
}

// Fonction principale
int main(int argc, char **argv)
{
   if(argc < 2)
   {
      printf("Usage : %s [address] [pseudo]\n", argv[0]);
      return EXIT_FAILURE;
   }

   app(argv[1], argv[2]); 

   return EXIT_SUCCESS;  
}