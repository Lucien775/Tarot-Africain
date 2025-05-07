.PHONY : all clean install uninstall

# Variables
CC = gcc
CFLAGS = -Wall
BINDIR = ./bin

# Cibles principales
all: client server

# Nettoyage des fichiers objets
clean:
	rm -f *.o client server 

uninstall:
	rm -f $(BINDIR)/client $(BINDIR)/server

# Installation des exécutables dans le répertoire `bin`
install:
	mkdir -p $(BINDIR)
	cp client $(BINDIR)
	cp server $(BINDIR)

# Compilation de l'exécutable client
client: client.o jeuCoteClient.o
	$(CC) $(CFLAGS) -o client client.o jeuCoteClient.o

# Compilation de l'exécutable server
server: jeuCoteServer.o server.o cartes.o
	$(CC) $(CFLAGS) -o server jeuCoteServer.o server.o cartes.o

# Compilation des fichiers objets
client.o: client.c client.h
	$(CC) $(CFLAGS) -c -o client.o client.c

jeuCoteClient.o: jeuCoteClient.c client.h
	$(CC) $(CFLAGS) -c -o jeuCoteClient.o jeuCoteClient.c

jeuCoteServer.o: jeuCoteServer.c jeuCoteServer.h server.h cartes.h
	$(CC) $(CFLAGS) -c -o jeuCoteServer.o jeuCoteServer.c

server.o: server.c jeuCoteServer.h server.h
	$(CC) $(CFLAGS) -c -o server.o server.c

cartes.o: cartes.c cartes.h
	$(CC) $(CFLAGS) -c -o cartes.o cartes.c

