/*----------------------------------------------
Serveur à lancer avant le client
------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
// On linux
//#include <linux/types.h>
// On macOS
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <stddef.h>
#define TAILLE_MAX_NOM 256


#define NOMBRE_MAX_CO 4

int sockets[NOMBRE_MAX_CO];
int nbClient = 0;

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

/*------------------------------------------------------*/
void envoi (int sock) {
    char buffer[256];
    int longueur;

    if ((longueur = read(sock, buffer, sizeof(buffer))) <= 0)
    	return;

    printf("message lu : %s \n", buffer);

    buffer[longueur+1] ='\0';

    for (int i = 0; i < nbClient; i++){
      write(sockets[i],buffer,strlen(buffer)+1);
      printf("message envoye. \n");
      printf("Tableau Socket tour numero : %d \n", i);
    }

    return;
}

int acceptConnexion(int socket_descriptor) {
    int longueur_adresse_courante,
        nouv_socket_descriptor;

    sockaddr_in adresse_client_courant;
    longueur_adresse_courante = sizeof(adresse_client_courant);

    /* adresse_client_courant sera renseigné par accept via les infos du connect */
    if ((nouv_socket_descriptor = accept(socket_descriptor, (sockaddr*)(&adresse_client_courant), &longueur_adresse_courante)) < 0) {
      perror("erreur : impossible d'accepter la connexion avec le client.");
      exit(1);
    }

    printf("Un client s'est connecté. \n");
    return nouv_socket_descriptor;
}
/* Ajout du nouveau client dans un tableau */
void addToList(int socket_descriptor) {
  // Boolean
  int socketAdded = 0;

  for (int i = 0; i < NOMBRE_MAX_CO; i++){
    if (sockets[i] == 0)	{
      sockets[i] = socket_descriptor;
      socketAdded = 1;
      nbClient++;
      break;
    }
  }
  // Si case non libre, on ajoute à la fin
  if (socketAdded == 0) {
    sockets[nbClient] = socket_descriptor;
    nbClient++;
  }
}

main(int argc, char **argv) {
    int socket_descriptor, 		/* descripteur de socket */
	      nouv_socket_descriptor, 	/* [nouveau] descripteur de socket */
			  longueur_adresse_courante; 	/* longueur d'adresse courante d'un client */

    sockaddr_in 	adresse_locale, 		/* structure d'adresse locale*/
			            adresse_client_courant; 	/* adresse client courant */

    hostent*		ptr_hote; 			/* les infos recuperees sur la machine hote */
    servent*		ptr_service; 			/* les infos recuperees sur le service de la machine */

    char 		machine[TAILLE_MAX_NOM+1]; 	/* nom de la machine locale */

    gethostname(machine,TAILLE_MAX_NOM);		/* recuperation du nom de la machine */

    /* recuperation de la structure d'adresse en utilisant le nom */
    if ((ptr_hote = gethostbyname(machine)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son nom.");
		exit(1);
    }

    /* initialisation de la structure adresse_locale avec les infos recuperees */

    /* copie de ptr_hote vers adresse_locale */
    bcopy((char*)ptr_hote->h_addr, (char*)&adresse_locale.sin_addr, ptr_hote->h_length);
    adresse_locale.sin_family		= ptr_hote->h_addrtype; 	/* ou AF_INET */
    adresse_locale.sin_addr.s_addr	= INADDR_ANY; 			/* ou AF_INET */

    /*-----------------------------------------------------------*/
    /* SOLUTION 2 : utiliser un nouveau numero de port */
    adresse_locale.sin_port = htons(5000);
    /*-----------------------------------------------------------*/

    printf("Numero de port utilisé : %d \n",
		   ntohs(adresse_locale.sin_port) /*ntohs(ptr_service->s_port)*/);

    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
  		perror("erreur : impossible de creer la socket de connexion avec le client.");
  		exit(1);
    }

    /* association du socket socket_descriptor à la structure d'adresse adresse_locale */
    if ((bind(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
  		perror("erreur : impossible de lier la socket a l'adresse de connexion.");
  		exit(1);
    }

    /* initialisation de la file d'ecoute */
    listen(socket_descriptor, 5);

    /* attente des connexions et traitement des donnees recues */
    for(;;) {

      /* Accepter les connexions */
      nouv_socket_descriptor = acceptConnexion(socket_descriptor);

      addToList(nouv_socket_descriptor);

      envoi(nouv_socket_descriptor);

      /* Ferme la connexion */
  		//close(nouv_socket_descriptor);

    }

}
