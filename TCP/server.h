#ifndef SERVER_H
#define SERVER_H


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <netdb.h> 

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#define CRLF        "\r\n"
#define PORT         1977
#define MAX_CLIENTS 3

#define BUF_SIZE    1024

#ifndef CLIENT_H
#define CLIENT_H

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
}Client;

#endif 


void app(void);
int init_connection(void);
void end_connection(int sock);
int read_client(SOCKET sock, char *buffer);
void write_client(SOCKET sock, const char *buffer);
void send_message_to_all_clients(Client client, int actual, const char *buffer, char from_server);
void remove_client(int to_remove, int *actual);
void clear_clients(int actual);

#endif 