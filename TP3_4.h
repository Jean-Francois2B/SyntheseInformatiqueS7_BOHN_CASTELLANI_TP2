
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>



#define ERREUR "Il n'y a pas le bon nombre d'arguments\n"
#define CONNECTION "Vous êtes connecté au serveur %s, sur le port %s, et utilisez le fichier %s\n"
#define TAILLE_BUFFER 512
#define OPCODE_TAILLE 2
#define ASCII_TAILLE 1
#define BLOCKID_TAILLE 2
