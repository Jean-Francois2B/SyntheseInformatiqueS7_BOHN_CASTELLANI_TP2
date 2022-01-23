
#include "TP3_4.h"

int main(int argc, char *argv[]){
	
	//--------------Q1-------------------
	char * serveur;
	char * port;
	char * nomFichier;
	
	// Commande a envoyer : nom_du_programme "host:port file"
	// On doit donc recevoir 3 arguments
	
	// On vérifie si on a eu tous les arguments
	if(argc != 3){
		printf(ERREUR);
		exit(EXIT_FAILURE);
	}
	
	// On sépare l'argument argv[1] pour avoir le host et le port
	serveur = strtok(argv[1], ":");
	port = strtok(NULL,":");
	
	//Si port est nul, alors on envoie 69, le port par défaut du protocole TCP
	if(port==NULL){
		port = malloc(3);
		port = "69";
	}
	
	nomFichier = malloc(strlen(argv[2])); //On alloue la bonne capacité de mémoire à "nomFichier"
	strcpy(nomFichier, argv[2]);
	printf(CONNECTION, serveur, port, nomFichier);	



	//--------------Q2-------------------
	//On configure le client en UDP
    struct addrinfo hints; //Structure qui filtre la réponse
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = PF_UNSPEC;//On autorise les transmissions en IPv4 et IPv6
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP; //On autorise le protocole UDP
	hints.ai_flags = AI_PASSIVE;
	hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    
    
    struct addrinfo *resultat;
	int fd = getaddrinfo(serveur,port,&hints, &resultat);
    if(fd != 0)
    {
        printf("getaddrinfo() n'a pas fonctionné.\n");
        exit(EXIT_FAILURE);
    }


	//--------------Q3-------------------
	int socketfd = socket(resultat->ai_family, resultat->ai_socktype, resultat->ai_protocol);



	//--------------Q4.a-------------------
	char* rrq; //Trame RRQ
	int N = OPCODE_TAILLE+2+strlen(nomFichier)+strlen("octet");
	rrq = malloc(N*sizeof(char));

	//Code 01 qui est lié à l'Opcode RQQ
	rrq[0]=0;
	rrq[1]=1;
	strcpy(rrq+OPCODE_TAILLE, nomFichier);
	strcpy(rrq+OPCODE_TAILLE+strlen(nomFichier)+1,"octet");
	//Envoi de la donnée au serveur
	int nbCharSend = sendto(socketfd,rrq,N,0,resultat->ai_addr, resultat->ai_addrlen);
	if (nbCharSend == -1){
		perror("Erreur dans le premier envoi de données");
		exit(EXIT_FAILURE);
	}


	//--------------Q4.b-------------------
	
	char ack[4]; //Trame ACK
	ack[0] = 0;
	ack[1] = 4;
	
	struct sockaddr adresse;
	socklen_t adresse_len = sizeof(adresse);
	char * buffer = malloc(TAILLE_BUFFER*sizeof(char));
	
	/*
	//Reception d'un unique paquet 
	int nbCharRecu = recvfrom(socketfd, buffer, TAILLE_BUFFER,0, &adresse, &adresse_len); //À correspond aux flags
	if (nbCharRecu == -1){ //Cas où l'on a une erreur de transmission
		perror("Erreur dans la réception de données");
		close(socketfd);
		close(fd);
		freeaddrinfo(resultat);
		free(rrq);
		exit(EXIT_FAILURE);
	}
	printf("Réception de %d octets venant du serveur\n", nbCharRecu);
	ack[2] = buffer[2]; //On complète la trame ACK
	ack[3] = buffer[3];

	int nbCharSend = sendto(socket_fd, ack, 4, 0, &addresse, adresse_len) == -1){ //Envoi de la trame ACK
	if(nbCharSend==-1){
		printf("Erreur dans l'envoie de la requête ACK");
		exit(EXIT_FAILURE);
	}
	*/


	//--------------Q4.c-------------------
	//Reception de plusieurs paquets 
	int nbCharRecu = TAILLE_BUFFER;
	while(nbCharRecu==TAILLE_BUFFER) {
		nbCharRecu = recvfrom(socketfd, buffer, TAILLE_BUFFER,0, &adresse, &adresse_len); //À correspond aux flags
		if (nbCharRecu == -1){ //Cas où l'on a une erreur de transmission
			perror("Erreur dans la réception de données");
			close(socketfd);
			close(fd);
			freeaddrinfo(resultat);
			free(rrq);
			exit(EXIT_FAILURE);
		}
		printf("Réception de %d octets venant du serveur\n", nbCharRecu);
		ack[2] = buffer[2]; //On complète la trame ACK
		ack[3] = buffer[3];

		int nbCharSend = sendto(socketfd, ack, 4, 0, &adresse, adresse_len);//Envoi de la trame ACK
		if(nbCharSend==-1){
			printf("Erreur dans l'envoie de la requête ACK");
			exit(EXIT_FAILURE);
		}
	}

	//On libère toute la mémoire empruntée
	close(socketfd);
	close(fd);
	freeaddrinfo(resultat);
	free(rrq);
	exit(EXIT_SUCCESS);
}

