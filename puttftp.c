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
    hints.ai_family = AF_UNSPEC;//On autorise les transmissions en IPv4 et IPv6
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP; //On autorise le protocole UDP
	
	struct addrinfo *resultat;
    if(getaddrinfo(serveur,port,&hints, &resultat) != 0)
    {
        printf("getaddrinfo() n'a pas fonctionné.\n");
        exit(EXIT_FAILURE);
    }
    
    
    //--------------Q3-------------------
	int fd = open(nomFichier, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	int socketfd = socket(resultat->ai_family, resultat->ai_socktype, resultat->ai_protocol);


	//--------------Q5.a-------------------
	//Construction de la requête WRQ
	char* wrq;
	int N = OPCODE_TAILLE + strlen(nomFichier) + ASCII_TAILLE  + strlen("netascii") + ASCII_TAILLE ;
	wrq = malloc(N*sizeof(char));
	//Code 02 qui est lié à l'Opcode WRQ
	wrq[0]=0;
	wrq[1]=2;
	strcpy(wrq+OPCODE_TAILLE, nomFichier);
	strcpy(wrq+strlen(nomFichier)+OPCODE_TAILLE+1, "octet");
	//Envoi de la donnée au serveur
	int nbCharSend = sendto(socketfd,wrq,4+strlen(nomFichier)+strlen("octet"),0,resultat->ai_addr, resultat->ai_addrlen);
	if (nbCharSend == -1){
		perror("Erreur dans le premier envoi de données");
		exit(EXIT_FAILURE);
	}


	//--------------Q5.b-------------------
	struct sockaddr addresse;						
	socklen_t addresse_len = sizeof(addresse);
	char* bufferRecu = malloc(TAILLE_BUFFER * sizeof(char));
	char* bufferEnvoye = malloc(TAILLE_BUFFER * sizeof(char));
	bufferRecu[0] = 0; 	//Opcodes
	bufferEnvoye[1] = 3;
	bufferEnvoye[3] = 1;

	int octetEnvoye = TAILLE_BUFFER - OPCODE_TAILLE - BLOCKID_TAILLE;

	while(octetEnvoye == TAILLE_BUFFER - OPCODE_TAILLE - BLOCKID_TAILLE) {
		printf("Données %d\n", bufferEnvoye[3]);
		octetEnvoye = read(fd, bufferEnvoye + OPCODE_TAILLE + BLOCKID_TAILLE, TAILLE_BUFFER - (OPCODE_TAILLE + BLOCKID_TAILLE));
		printf("%s\n", bufferEnvoye + OPCODE_TAILLE + BLOCKID_TAILLE);

		if(octetEnvoye < 0){
			printf("La lecture de la trame a échoué");
			exit(EXIT_FAILURE);
		}

		if(sendto(socketfd, bufferEnvoye, TAILLE_BUFFER, 0, &addresse, addresse_len) < 0) {
			printf("L'envoi de la trame a échoué");
			exit(EXIT_FAILURE);
		}

		if(recvfrom(socketfd, bufferEnvoye, TAILLE_BUFFER,0, &addresse, &addresse_len) < 0) {
			printf("La requête WRQ a échouée, la trame ACK n'a pas été reçue");
			exit(EXIT_FAILURE);
		}

		bufferEnvoye[3]++;
	}
	
	//On libère toute la mémoire empruntée
	close(socketfd);
	close(fd);
	freeaddrinfo(resultat);
	free(wrq);
	exit(EXIT_SUCCESS);
}
