/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
client <adresse-serveur> <message-a-transmettre>
------------------------------------------------------------*/
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

#define PORT 4000

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;
typedef struct servent 		servent;

typedef struct {
	int sock;
} infosClient ;

const char* pseudo = "";
char* couleur = "blanc";

// Retourne l'index de la première occurence de cs dans ct
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

// Extrait une sous-chaine de s entre start et end
char *str_sub (char *s, unsigned int start, unsigned int end)
{
   char *new_s = NULL;

   if (s != NULL && start < end)
   {
      new_s = malloc (sizeof (*new_s) * (end - start + 2));
      if (new_s != NULL)
      {
         int i;

         for (i = start; i <= end; i++)
         {
            new_s[i-start] = s[i];
         }
         new_s[i-start] = '\0';
      }
      else
      {
         fprintf (stderr, "Memoire insuffisante\n");
         exit (EXIT_FAILURE);
      }
   }
   return new_s;
}


//modification de couleur
void afficher_couleur(char *couleur)
{
	if (strcmp(couleur,"noir") == 0){
		printf("\033[30m");
	} else if (strcmp(couleur,"rouge") == 0) {
		printf("\033[31m");
	} else if (strcmp(couleur,"vert") == 0) {
		printf("\033[32m");
	} else if (strcmp(couleur,"jaune") == 0) {
		printf("\033[33m");
	} else if (strcmp(couleur,"bleu") == 0) {
		printf("\033[34m");
	} else if (strcmp(couleur,"magenta") == 0) {
		printf("\033[35m");
	} else if (strcmp(couleur,"cyan") == 0) {
		printf("\033[36m");
	} else {
		printf("\033[37m");
	}
}

//Envoie un message sur la socket passée en paramètre
void envoi_message(int socket_descriptor, char* mesg){
    if ((write(socket_descriptor, mesg, strlen(mesg))) < 0) {
	perror("erreur : impossible d'ecrire le message destine au serveur.");
	exit(1);
    }
}


//Lit un message depuis la socket passée en paramètre
void lire_message(int socket_descriptor){
	char buffer [512];
	char *tmp;
	char *msg;
	char *color;
	int index;
	int longueur;
	 /* lecture de la reponse en provenance du serveur */
	if((longueur = read(socket_descriptor, buffer, 512)) > 0) {
		tmp = buffer;
		index = str_istr(tmp, "$$");
		if (index > 0) {
			color = str_sub(tmp, 0, index - 1);
			afficher_couleur(color);
			tmp = strstr(tmp, "$$");
		}
		if (strcmp(tmp, "deconnecte") == 0) {
			printf("vous allez etre déconnecté\n");
			sleep(3);
			exit(0);
		}
		printf("\033[1A");
		printf("\033[80C");
		printf("\n\n");
		printf("\033[1A");
		printf("%s\n", tmp);
		afficher_couleur(couleur);
		printf("\033[1B");
	}		
}


/*Thread qui permet de lire en continu les messages depuis la socket passée en paramètre*/
void* thread_lecture(void *d){
	int socket_descriptor;
	socket_descriptor = (int) d;

	while(1){
		lire_message(socket_descriptor);
		
	}
}



/*Demande au client de saisir le message qu'il veut envoyer*/
char* ecrire_message(){
	char *mesg = malloc(512);
	char *tmpmesg = malloc(512);
	char *ptr;
	char *newnom = malloc(512);
	char *newcouleur = malloc(512);
	int i=0;
	int ok=0;

	//while (ok==0){
		for(i=0;i<strlen(mesg);i++){
			mesg[i]='\0';		
		}
		scanf("%[^\n]",mesg);
		getchar();
		if (strcmp(mesg,"")==0){
			mesg = "message vide...\n";
			ok=1;
		}else if (strstr(mesg, "/name" ) != NULL){
			ptr = strstr(mesg, "/name");
			newnom = &ptr[6];
			sprintf(tmpmesg,"changement de nom en %s",newnom);
			printf("Vous avez changé de nom pour %s \n", newnom);
			strcat(newnom," : ");
			pseudo = newnom;
			ok=1;
			mesg=tmpmesg;
		} else if(strstr(mesg, "/cmdcolor" ) != NULL){
			printf("\n* Couleurs disponibles :\n  * noir\n  * jaune\n  * bleu\n  * vert\n  * rouge\n  * cyan\n  * magenta\n  * blanc\n\n");
		}else if(strstr(mesg, "/cmd" ) != NULL){
			printf("\n* Affichage des commandes :\n  /exit : pour quitter le chat\n  /name <nom> : pour changer de nom\n  /nb : pour connaître le nombre de personnes connectés\n  /color <couleur> : pour changer la couleur de vos messages\n  /cmdcolor : pour connaître les couleurs disponibles\n\n");
		} else if(strstr(mesg, "/color" ) != NULL){
			ptr = strstr(mesg, "/color");
			newcouleur = &ptr[7];
			couleur = newcouleur;
			afficher_couleur(newcouleur);
			if ((strcmp(couleur,"noir") == 0) || (strcmp(couleur,"rouge") == 0) || (strcmp(couleur,"vert") == 0) || (strcmp(couleur,"jaune") == 0) || (strcmp(couleur,"bleu") == 0) || (strcmp(couleur,"magenta") == 0) || (strcmp(couleur,"cyan") == 0) || (strcmp(couleur,"blanc") == 0)) {	
				printf("couleur changé! \n");
			} else {
				printf("cette couleur n'existe pas! \n");
				printf("couleur changé en blanc \n");
			}			
		}
	//}
	return mesg;
}

/* Thread qui permet d'envoyer en continu des messages sur la socket passée en paramètre au travers de la structure */
void* thread_ecriture(void *structure){
	infosClient* infos;
	infos  = (infosClient*) structure;

	char * message = malloc(512);
	//char * pseudoTmp = malloc(512);
	char * a_envoyer = malloc(512);
	char * coupure = malloc(8);

	int socket_descriptor;
	int i = 0;
	int j = 0;
	socket_descriptor = infos->sock;
	coupure = "$$$";
	while(1){
		for (i = 0; i<strlen(couleur); i++){
			a_envoyer[i]=couleur[i];
			a_envoyer[i+1]='\0';
			j = i;
		}
		j++;
		a_envoyer[j] = coupure[1];
		a_envoyer[j+1]='\0';
		j++;
		a_envoyer[j] = coupure[2];
		a_envoyer[j+1]='\0';
		
		for (i = 0; i<strlen(pseudo); i++){
			j++;
			a_envoyer[j]=pseudo[i];
			a_envoyer[j+1]='\0';
		}

		message="";
		message = ecrire_message();
		a_envoyer = strcat(a_envoyer,message);
		envoi_message(socket_descriptor,a_envoyer);
	}
}

int main(int argc, char **argv) {

    int socket_descriptor, 			/* descripteur de socket */
		longueur; 					/* longueur d'un buffer utilisé */
    sockaddr_in adresse_locale; 	/* adresse de socket local */
    hostent *	ptr_host; 			/* info sur une machine hote */
    servent *	ptr_service; 		/* info sur service */

    char *	prog; 					/* nom du programme */
    char *	host; 					/* nom de la machine distante */
    char *  tmppseudo = malloc(50);
    char * 	message = malloc(512); 	/* message a envoyer */

    pthread_t threadEcrire;
	pthread_t threadLire;

    if (argc != 2) {
		perror("usage : ./client <adresse-serveur>");
		exit(1);
    }

    prog = argv[0];
    host = argv[1];

    printf("Entrez un nom pour accéder au chat: ");
    scanf("%s",tmppseudo);
    pseudo = strcat(tmppseudo, " : ");

    if ((ptr_host = gethostbyname(host)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son adresse. \n");
		exit(1);
    }

    /* copie caractere par caractere des infos de ptr_host vers adresse_locale */
    bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
    adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */

    /*-----------------------------------------------------------*/
    /* SOLUTION 2 : utiliser un nouveau numero de port */
    adresse_locale.sin_port = htons(PORT);
    /*-----------------------------------------------------------*/

    printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));

    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("erreur : impossible de creer la socket de connexion avec le serveur.");
		exit(1);
    }

    /* tentative de connexion au serveur dont les infos sont dans adresse_locale */
    if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
		perror("erreur : impossible de se connecter au serveur.");
		exit(1);
    }

    printf("connexion etablie avec le serveur. \n");

    infosClient * infosCl = malloc(sizeof(int)+512);	

	infosCl->sock = socket_descriptor;

	if (pthread_create(&threadEcrire, NULL, thread_ecriture, (void *)infosCl)  < 0){
		printf("erreur \n");
		exit(1);
	}
	
	if (pthread_create(&threadLire, NULL, thread_lecture, (void *)socket_descriptor)  <  0){
		printf("erreur \n");
		exit(1);
	}
	afficher_couleur("rouge");
	printf("*********************************************************************\n");
	printf("*                                                                   *\n");
	printf("*                           CHAT RESEAU                             *\n");
	printf("*                                                                   *\n");
	printf("*********************************************************************\n");
	afficher_couleur("blanc");
	printf("               **** Bienvenu sur ce chat instantané ***** \n");
	printf("           Vous pouvez avoir accès à l'ensembles des commandes \n");
	printf("                      disponible en faisant : /cmd\n\n");
	printf("*********************************************************************\n\n");
	while(1){
		//execution des threads
	}

}
