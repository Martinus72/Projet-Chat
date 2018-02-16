/**
*	Server source code
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
#include "sfunct.h"

/* Reading thread in continious from the given socket */
void* reading_thread(void *s_descriptor){
	int socket_descriptor,
		i;
	socket_descriptor = (int) s_descriptor;

	char msg[4096];

	while(1){
		MsgToReturn = reading(socket_descriptor);

		// Check if client want leave
		if (strstr(MsgToReturn, "!quit" ) != NULL ){
			printf("Client %d want to leave\n", socket_descriptor);
			replying(socket_descriptor, "disconnected");
			printf("Logout accepted\n");
			sleep(2);

			// Free up space in socket list
			for (i=0; i< countActiveConnection; i++){
				if (socketList[i] == socket_descriptor){
					socketList[i] = 0;
				}
			}
			countActiveConnection--;
			break;

		} else if (strstr(MsgToReturn, "!nb" ) != NULL ){
			sprintf(msg, "%d client on the server\n", countActiveConnection);
			MsgToReturn = msg;
		}
		printf("Client %d receip message : %s\n",socket_descriptor, MsgToReturn);
	}
}

/* Server's thread receipt and return messages */
void* writing_thread(void *structure){
	infosThread* infos;
	int i = 0;	
	int new_socket_descriptor,
		tabDeSockets[NB_MAX_CO];

	infos  = (infosThread*) structure;		
	
	while(1){
		//Check if message, return to all client
		if (strcmp(MsgToReturn, "") != 0){
			for (i = 0; i < countActiveConnection; i++){
				if(socketList[i] != 0){
					replying(socketList[i], MsgToReturn);
				}
			}	
			MsgToReturn = "";
		}
	}
}

main(int argc, char **argv) {
	int socket_descriptor, 
		i,
		n,
		cancel; 		
		
	char in_msg[256];
	
	infosThread * infos = malloc(sizeof(int)+sizeof(int)+4096);

	pthread_t thread_Write;
	pthread_t thread_Read;
	
	// Init server
	socket_descriptor = initServer(NB_MAX_CO);

	// Init list of socket
	for (i = 0; i < NB_MAX_CO; i++){
		socketList[i] = 0;			
	}

	if (pthread_create(&thread_Write, NULL, writing_thread, (void*)infos)  ==  0) {
		printf("INFO - Server's write thread initiated\n");
	} else printf("ERROR - Server's write thread not initiated\n");

	// Waiting connection
    while(1) {
	
		// Check if the server isn't full
		if(countActiveConnection < NB_MAX_CO){

			int new_socket_descriptor = acceptNewCo(socket_descriptor);

			// Add in socket list
			addSocketList(new_socket_descriptor);

			if (pthread_create(&thread_Read, NULL, reading_thread, (void *)new_socket_descriptor)  ==  0) {
				printf("INFO - Server's read thread initiated\n");
			} else printf("ERROR - Server's read thread not initiated\n");

			printf(in_msg,"Hi ! There is a new client on the server :) !\n");

			for (i=0; i< countActiveConnection; i++){
					if(socketList[i] != 0){
				replying(socketList[i],in_msg);
				}
			}
		}
		else {
			printf("Server full, please wait...\n"); 
			sleep(15);
		}
	}
}

