#ifndef cfunct
#define cfunct

const char* pseudo = "";

char* couleur = "white";

/* Change the display color */
void setColor(char *color)
{
   if (strcmp(color,"black") == 0){
      printf("\033[30m");
   } else if (strcmp(color,"red") == 0) {
      printf("\033[31m");
   } else if (strcmp(color,"blue") == 0) {
      printf("\033[34m");
   } else if (strcmp(color,"green") == 0) {
      printf("\033[32m");
   } else {
      // White by default
      printf("\033[37m");
   }
}

/* Display the welcome menu */
void menu() {
   setColor("green");
   printf(" -------------------------------------------------------------------------------\n");
   printf("|                                                                               |\n");
   printf("########################### - Welcom on Tchat App' - ###########################\n");
   printf("|                                                                               |\n");
   printf("################ - Copyright : Alexandre Goux & Martin Levrard - ###############\n");
   printf("|                                                                               |\n");
   printf(" -------------------------------------------------------------------------------\n"); 
   printf( "                              >> Version 1.7 <<\n");
   setColor("white");

   printf("Loading...\n\n");
   sleep(2);
   printf("$$ System Bot : Hello, I'm the system. You can contact me any time with special commands :) \n");
   printf("$$ System Bot : Enter the command '!help' for more details. Enyoy ! \n");
}

/* Send message to the given socket */
void replying(int socket_descriptor, char* msg){
    if ((write(socket_descriptor, msg, strlen(msg))) < 0) {
      perror("ERROR - Message not send to the server");
      exit(1);
    }
}

/* Read message from the given socket */
void reading(int socket_descriptor){
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
         setColor(color);
         tmp = strstr(tmp, "$$");
      }
      if (strcmp(tmp, "disconnected") == 0) {
         printf("Wait during disconnection...\n");
         sleep(3);
         exit(0);
      }
      printf("\033[1A");
      printf("\033[80C");
      printf("\n\n");
      printf("\033[1A");
      printf("%s\n", tmp);
      setColor(couleur);
      printf("\033[1B");
   }     
}

/* Get and handle the given text */
char* getMsg(){

   char *mesg = malloc(512);
   char *tmpmesg = malloc(512);
   char *ptr;
   char *newName = malloc(512);
   char *newColor = malloc(512);

   int i=0;
   int ok=0;

   // Init mesg
   for(i=0; i < strlen(mesg); i++){
      mesg[i]='\0';     
   }

   // Get the given text
   scanf("%[^\n]", mesg);
   getchar();

   if (strcmp(mesg,"") == 0){
      mesg = "No message\n";
      ok = 1;
   } else if (strstr(mesg, "!pseudo" ) != NULL){
      ptr = strstr(mesg, "!pseudo");
      newName = &ptr[6];
      sprintf(tmpmesg,"change pseudo to %s",newName);
      printf("You change your pseudo to %s \n", newName);
      strcat(newName," : ");
      pseudo = newName;
      ok = 1;
      mesg = tmpmesg;
   } else if(strstr(mesg, "!listColors" ) != NULL){
      printf("\n* Available colors :\n  * black\n  * blue\n  * red\n  * white\n\n");
   }else if(strstr(mesg, "!help" ) != NULL){
      printf("\n* Available cmd :\n  - !quit : Leave chat\n  - !pseudo <pseudo> : Change pseudo\n  - !nb : Display the number of connected clients\n  - !color <color> : Change color message\n  - !listColors: Display available colors\n\n");
   } else if(strstr(mesg, "!color" ) != NULL){
      ptr = strstr(mesg, "!color");
      newColor = &ptr[7];
      couleur = newColor;
      setColor(newColor);
      if ((strcmp(couleur,"black") == 0) || (strcmp(couleur,"blue") == 0) || (strcmp(couleur,"red") == 0) || (strcmp(couleur,"white") == 0)) {  
         printf("Color changed with success \n");
      } else {
         printf("Color not found. Color set to white by default \n");
      }        
   }
   return mesg;
}

#endif