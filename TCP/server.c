#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "jeuCoteServer.h"

//Variables globales
Client clients[MAX_CLIENTS];
int actual;

//Fonction principale qui gèrent le jeu
void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   actual = 0;
   int max = sock;

   fd_set rdfs;

   printf("serveur ouvert\n");
   
   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs);
      FD_SET(STDIN_FILENO, &rdfs);
      FD_SET(sock, &rdfs);

      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         socklen_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         if(read_client(csock, buffer) == -1)
         {
            continue;
         }

         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = { csock };
         strncpy(c.name, buffer, BUF_SIZE - 1);
         clients[actual] = c;
         actual++;

         char codePlusPseudo[BUF_SIZE +1];
         codePlusPseudo[0] = 1;

         for (int i = 0; i < BUF_SIZE; ++i) codePlusPseudo[i+1] = buffer[i]; 

         printf("nouveau client : %s\n", buffer);
         recoitUnMessage(codePlusPseudo);
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client client = clients[i];
               int c = read_client(clients[i].sock, buffer);
               if(c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(i, &actual);
                  strncpy(buffer, client.name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(client, actual, buffer, 1);
               }
               else
               {
                  recoitUnMessage(buffer);
               }
               break;
            }
         }
      }
   }
   clear_clients(actual);
   end_connection(sock);
}

//Ferme la socket de tous les clients
void clear_clients(int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

//Supprime un client
void remove_client(int to_remove, int *actual)
{
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   (*actual)--;
}

//Envoie un message à tous les clients
void send_message_to_all_clients(Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(i, message);
      }
   }
}

//Initialise la socket
int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
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

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
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
int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

//Ecrit au client
void write_client(int indiceSocket, const char *buffer)
{  
   if (indiceSocket < actual)
   {
      int i = 0;

      printf("On envoie au client %d : ", indiceSocket);
      while (buffer[i] != '\0') printf("%d ", buffer[i++]);
      printf("\n");

      if(send(clients[indiceSocket].sock, buffer, strlen(buffer), 0) < 0)
      {
         perror("send()");
         exit(errno);
      }
   }
}