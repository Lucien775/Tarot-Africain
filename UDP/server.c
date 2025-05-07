#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "jeuCoteServer.h"

//Variables globales
Client clients[MAX_CLIENTS];

//Fonction principale qui gèrent le jeu
void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   int actual = 0;  
   
   fd_set rdfs;

   printf("serveur ouvert\n");

   while(1)
   {  
      FD_ZERO(&rdfs);
      FD_SET(STDIN_FILENO, &rdfs);  
      FD_SET(sock, &rdfs);         /

      if (select(sock + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      if (FD_ISSET(STDIN_FILENO, &rdfs))
      {
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         SOCKADDR_IN csin = {0};
         int n = read_client(sock, &csin, buffer);

         if (n <= 0) continue;  

         if (check_if_client_exists(clients, &csin, actual) == 0)
         {
            if (actual < MAX_CLIENTS)
            {
               char codePlusPseudo[BUF_SIZE + 1];
               codePlusPseudo[0] = 1; 
               strncpy(codePlusPseudo + 1, buffer, BUF_SIZE);  

               printf("Nouveau client : %s\n", buffer);

               Client c = {csin};
               strncpy(c.name, buffer, BUF_SIZE - 1);
               clients[actual++] = c;

               recoitUnMessage(codePlusPseudo, clients, sock);
            }
         }
         else
         {
            Client *client = get_client(clients, &csin, actual);
            if(client == NULL) continue;
            recoitUnMessage(buffer, clients, sock);
         }
      }
   }
   end_connection(sock);
}

//Vérifie si le client s'est déjà connecté à ce serveur 
int check_if_client_exists(Client *clients, SOCKADDR_IN *csin, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      if(clients[i].sin.sin_addr.s_addr == csin->sin_addr.s_addr
        && clients[i].sin.sin_port == csin->sin_port)
      {
         return 1;
      }
   }

   return 0;
}

//Ajoute un nouveau client
Client* get_client(Client *clients, SOCKADDR_IN *csin, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      if(clients[i].sin.sin_addr.s_addr == csin->sin_addr.s_addr
        && clients[i].sin.sin_port == csin->sin_port)
      {
         return &clients[i];
      }
   }

   return NULL;
}

//Initialise la socket
int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   return sock;
}

//Ferme la socket
void end_connection(int sock)
{
   closesocket(sock);
}

//Lit un message d'un client
int read_client(SOCKET sock, SOCKADDR_IN *sin, char *buffer)
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

//Ecrit au client
void write_client(SOCKET sock, SOCKADDR_IN *sin, const char *buffer)
{
   int i = 0;
   while (buffer[i] != '\0') printf("%d ", buffer[i++]);
   printf("\n");

   if(sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *) sin, sizeof *sin) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

