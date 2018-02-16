/**
*	Client source code
*	Alexandre Goux, Martin Levrard
**/

#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h> 
#include <stddef.h>

#include "extension.h"
#include "cfunct.h"

#define PORT 4001

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;
typedef struct servent 		servent;

typedef struct {
	int sock;
} infosClient ;


const char* groupe = "";


/* Continious messages send thread */
void* writing_thread(void *structure){
	infosClient* infos;
	infos  = (infosClient*) structure;

	char * msg = malloc(512);
	char * toSend = malloc(512);
	char * split = malloc(8);

	int socket_descriptor;
	int i = 0;
	int j = 0;

	socket_descriptor = infos->sock;
	split = "$$$";

	while(1){
		for (i = 0; i < strlen(couleur); i++){
			toSend[i] = couleur[i];
			toSend[i+1] = '\0';
			j = i;
		}
		j++;

		toSend[j] = split[1];
		toSend[j+1] = '\0';
		j++;

		toSend[j] = split[2];
		toSend[j+1] = '\0';
		
		for (i = 0; i < strlen(pseudo); i++){
			j++;
			toSend[j] = pseudo[i];
			toSend[j+1] = '\0';
		}

		msg="";
		msg = getMsg();
		toSend = strcat(toSend, msg);
		replying(socket_descriptor,toSend);
	}
}

/* Reading thread in continious from the given socket */
void* reading_thread(void *d){
   int socket_descriptor;
   socket_descriptor = (int) d;

   while(1){
      reading(socket_descriptor);
   }
}

int main(int argc, char **argv) {
    int 	socket_descriptor;
    sockaddr_in local_adress;
    hostent *	ptr_host;
    servent *	ptr_service; 		
	
    char *  tmppseudo = malloc(50);
    char *  tmpgroupe = malloc(50);

    // message to send
    char * 	msg = malloc(512);
	
	pthread_t thread_Write;
	pthread_t thread_Read;
   
    printf("Your pseudo : ");
    scanf("%s",tmppseudo);
    pseudo = strcat(tmppseudo, " : ");

    printf("Your groupe name : ");
    scanf("%s",tmpgroupe);
    groupe = strcat(tmpgroupe, " : ");

    // Get adress structure with name
    if ((ptr_host = gethostbyname("localhost")) == NULL) {
		perror("ERROR - Server not found");
		exit(1);
    }
    
    // Copy ptr_hote to  local_addr
    bcopy((char*)ptr_host->h_addr, (char*)&local_adress.sin_addr, ptr_host->h_length);
    local_adress.sin_family = AF_INET;
    
    // Use the given port number
    local_adress.sin_port = htons(PORT);
        
    // Socket creation
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("ERROR - Socket doesn't created");
		exit(1);
    }
    
    // Connection with the server
    if ((connect(socket_descriptor, (sockaddr*)(&local_adress), sizeof(local_adress))) < 0) {
		perror("ERROR - Connection with the server impossible");
		exit(1);
    }
    
    printf("Connection established\n\n");

	infosClient * infosCl = malloc(sizeof(int)+512);
	infosCl->sock = socket_descriptor;

	if (pthread_create(&thread_Write, NULL, writing_thread, (void *)infosCl)  ==  0){
		printf("INFO - Client's write thread initiated\n");
	} else {
		printf("ERROR - Client's write thread not initiated\n");
		exit(1);
	}
	

	if (pthread_create(&thread_Read, NULL, reading_thread, (void *)socket_descriptor)  ==  0){
		printf("INFO - Client's read thread initiated\n");
	} else {
		printf("ERROR - Client's read thread not initiated\n");
		exit(1);
	}

	// display menu
	menu();
	while(1){
	}
}
