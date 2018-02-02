/*----------------------------------------------
Serveur à lancer avant le client
------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
// On linux
#include <linux/types.h> 
// On macOS
//#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h> 		
#include <string.h> 		 
#include <pthread.h> 
#include <stddef.h> 

#define TAILLE_MAX_NOM 256
#define NOMBRE_MAX_CO 4
#define PORT 4000

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

typedef struct {
	int sock;
	int *ptrTab;// = int sockets[NOMBRE_MAX_CON];
	int nbCo;
} infosThread ;

int sockets[NOMBRE_MAX_CO];
const char* message_a_renvoyer = "";
int nbClient = 0;


/* Retourne l'index de la première occurence de cs dans ct */
int str_istr (const char *cs, const char *ct)
{
   int index = -1;

   if (cs != NULL && ct != NULL)
   {
      char *ptr_pos = NULL;

      ptr_pos = strstr (cs, ct);
      if (ptr_pos != NULL)
      {
         index = ptr_pos - cs;
      }
   }
   return index;
}

/* Initialisation du serveur */
int initServer (int nb_max_client) {
    int socket_descriptor; 	/* descripteur de socket */
    sockaddr_in adresse_locale; 	/* adresse de socket local */
    hostent *	ptr_hote; 		/* info sur une machine hote */
    servent *	ptr_service; 		/* info sur service */
    char machine[TAILLE_MAX_NOM+1]; 	/* nom de la machine locale */

	// recuperation du nom de la machine
    gethostname(machine,TAILLE_MAX_NOM);		

    // recuperation de la structure d'adresse en utilisant le nom
    if ((ptr_hote = gethostbyname(machine)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son nom.");
		exit(1);
    }

    // copie de ptr_hote vers adresse_locale
    bcopy((char*)ptr_hote->h_addr, (char*)&adresse_locale.sin_addr, ptr_hote->h_length);
    adresse_locale.sin_family		= ptr_hote->h_addrtype; 	/* ou AF_INET */
    adresse_locale.sin_addr.s_addr	= INADDR_ANY; 			/* ou AF_INET */

    /*-----------------------------------------------------------*/
    /* SOLUTION 2 : utiliser un nouveau numero de port */
    adresse_locale.sin_port = htons(PORT);
    /*-----------------------------------------------------------*/

    printf("numero de port pour la connexion au serveur : %d \n",
		   ntohs(adresse_locale.sin_port) /*ntohs(ptr_service->s_port)*/);

    // creation de la socket
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le client.");
		exit(1);
    }

    // association du socket socket_descriptor à la structure d'adresse adresse_locale
    if ((bind(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
		perror("erreur : impossible de lier la socket a l'adresse de connexion.");
		exit(1);
    }

    // initialisation de la file d'ecoute
    listen(socket_descriptor, nb_max_client);

    return socket_descriptor;
}

/* Envoie un message sur un socket */
void rep (int sock, const char* msg) {
	if ((str_istr(msg, "/cmd") == -1) && (str_istr(msg,"message vide...") == -1) && (str_istr(msg,"/nb") == -1) && (str_istr(msg,"/color") == -1)){
	write(sock, msg, strlen(msg)+1);
	}
}

/* Lit un message sur la socket passée en paramètre */
char* lire(int sock){
	int longueur;
	char* buffer = malloc(4096);
	char* retour = malloc(4096);

	if ((longueur = read(sock, buffer, 4096)) <= 0){
		printf("Problème avec la longueur de %d du message du client %d.\n",longueur, sock);
    		return;
	} 
	return buffer;
}

/* Thread qui permet d'automatiser la reception et le renvoi des messages */
void* thread_ecriture(void *args){
	infosThread* infos;
	int i = 0;	
	int nouv_socket_descriptor;
	int tabDeSockets[NOMBRE_MAX_CO];
	infos  = (infosThread*) args;		
	
	while(1){
		//Si il y a un message, on l'envoie à tout les connectés
		if (strcmp(message_a_renvoyer,"") != 0){
			for (i=0; i< nbClient; i++){
				if(sockets[i] != 0){
					rep(sockets[i], message_a_renvoyer);
				}
			}	
			// init le message à renvoyer
			message_a_renvoyer = "";
		}
	}
}

/* Thread qui permet de lire en continu les messages */
void* thread_lecture(void *args){
	int socket_descriptor, 
	longueur, 
	i;

	socket_descriptor = (int) args;
	char mssg[4096];

	while(1){

		message_a_renvoyer = lire(socket_descriptor);

		if (strstr(message_a_renvoyer, "/exit" ) != NULL ){
			printf("Le client %d veut se déconnecter\n",socket_descriptor);
			rep(socket_descriptor, "deconnecte");
			printf("Deconnexion acceptée.\n");
			sleep(2);

			//on va chercher la socket dans tabsocket et la remplacer par 0
			//ce qui signifiera que la place est libre
			for (i=0; i< nbClient; i++){
				if (sockets[i] == socket_descriptor){
					sockets[i] = 0;
				}
			}
			nbClient --;
			break;
		} else if (strstr(message_a_renvoyer, "/nb" ) != NULL ){
			printf("Le client %d demande le nombre de personnes\n",socket_descriptor);
			sprintf(mssg,"Il y a %d personne(s) qui participent à cette conversation\n",nbClient);
			message_a_renvoyer=mssg;
		}

		printf("Client %d réception d'un message : %s\n",socket_descriptor,message_a_renvoyer);
	}
}

/* Accepte un nouveau client */
int accepterCo(int socket_descriptor){
	int nouv_socket_descriptor;
	int longueur_adresse_courante;

	// Adresse du client
	sockaddr_in adresse_client_courant;	

	longueur_adresse_courante = sizeof(adresse_client_courant);
	
	/* adresse_client_courant sera renseigné par accept via les infos du connect */
	if ((nouv_socket_descriptor = accept(socket_descriptor, (sockaddr*)(&adresse_client_courant), &longueur_adresse_courante)) < 0) {
		perror("erreur : impossible de se connecter avec le client" );
		exit(1);
	}
	printf(" Client connecte sur le socket %d depuis %s:%d\n",nouv_socket_descriptor,(char *)inet_ntoa(adresse_client_courant.sin_addr),htons(adresse_client_courant.sin_port));	
	
	return nouv_socket_descriptor;
}

main(int argc, char **argv) {

    int nouv_socket_descriptor,
		socket_descriptor,
		i,
		n,
		cancel;
		
	char message_in[256];
	
	sockaddr_in* tabAdresses[NOMBRE_MAX_CO];

	pthread_t threadEcrire;
	pthread_t threadLire;

	infosThread * infos = malloc(sizeof(int)+sizeof(int)+4096);
	
	// Init du serveur
	socket_descriptor = initServer(NOMBRE_MAX_CO);

	// Vide le tableau des sockets
	for (i=0; i< NOMBRE_MAX_CO; i++){

		sockets[i] = 0;			
	}

	// On lance le thread d'écriture
	if (pthread_create(&threadEcrire, NULL, thread_ecriture, (void*)infos)  ==  0){
		printf("thread écriture lancé\n");
	} else printf("erreur \n");

	// Attente des connexions
    while(1) {
		// Si le nb max connexion non atteint
		if(nbClient < NOMBRE_MAX_CO){

			nouv_socket_descriptor = accepterCo(socket_descriptor);
			int socketAdded = 0;


			// Check si case libre
			for (i=0; i< NOMBRE_MAX_CO; i++){		
				if (sockets[i] == 0)	{
					sockets[i] = nouv_socket_descriptor;
					socketAdded = 1;
					nbClient++;
					break;
				}		
			}		
			// Si case non libre, on ajoute à la fin
			if (socketAdded == 0) {
				sockets[nbClient] = nouv_socket_descriptor;
				nbClient++;
			}

			// on lance le thread de lecture
			if (pthread_create(&threadLire, NULL, thread_lecture, (void *)nouv_socket_descriptor)  ==  0){
				printf("thread lecture lancé\n");
			} else printf("erreur de lancement du thread\n");

			sprintf(message_in,"Une nouvelle personne entre sur le chat\n          %d personne(s) dans le chat\n", nbClient);

			for (i=0; i< nbClient; i++){
				if(sockets[i] != 0){
					rep(sockets[i], message_in);
				}
			}
		}

		// Limite de connexion atteinte
		else {
			printf(" Nombre de client maximal atteint!\n     Les connexions sont désormais fermées/mise en attente...\n"); 
			//sleep(15);
		}
	}
}
