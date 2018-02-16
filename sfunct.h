#ifndef sfunct
#define sfunct

#define TAILLE_MAX_NOM 256
#define NB_MAX_CO 4
#define PORT 4001

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

typedef struct {
   int sock;
   int *ptrTab;
   int nbCo;
} infosThread ;

const char* MsgToReturn = "";

int socketList[NB_MAX_CO];
int countActiveConnection = 0;

/* Send message to the given socket */
void replying (int sock,const char* msg) {
   if ((str_istr(msg,"!help") == -1) && (str_istr(msg,"No message") == -1) && (str_istr(msg,"!nb") == -1) && (str_istr(msg,"!color") == -1)){
   write(sock, msg, strlen(msg)+1);
   }
}

/* Read message from the given socket */
char* reading(int sock){
   int lenght;
   char* buffer = malloc(4096);

   if ((lenght = read(sock, buffer, 4096)) <= 0){
      printf("ERROR - Lenght message problem of %d from client %d.\n", lenght, sock);
      return;
   } 
   return buffer;
}

/* Accept new connection */
int acceptNewCo(int socket_descriptor){
   int new_socket_descriptor,
      current_addr_lenght;
   // Current client's adress
   sockaddr_in current_client_addr; 

   current_addr_lenght = sizeof(current_client_addr);
   
   if ((new_socket_descriptor = accept(socket_descriptor, (sockaddr*)(&current_client_addr), &current_addr_lenght)) < 0) {
      perror("ERROR - Connection not accepted");
      exit(1);
   }
   printf("Connection on %d from %s:%d\n", new_socket_descriptor, (char *)inet_ntoa(current_client_addr.sin_addr), htons(current_client_addr.sin_port)); 
   
   return new_socket_descriptor;
}

/* Add in socket list */
void addSocketList (int new_socket_descriptor) {
   int i;
   int added = 0;

   // Check if there's a free place
   for (i = 0; i < NB_MAX_CO; i++){    
      if (socketList[i] == 0) {
         socketList[i] = new_socket_descriptor;
         added = 1;
         countActiveConnection++;
         break;
      }     
   }     
   // else
   if (added == 0) {
      socketList[countActiveConnection] = new_socket_descriptor;
      countActiveConnection++;
   }
}

/* Server initialisation */
int initServer (int nb_max_client) {
    int socket_descriptor;       
    // Local adress structure
    sockaddr_in   local_addr;
    // Host machine info
   hostent* ptr_hote;         
   // Service machine info
    servent*   ptr_service;         
    // Local machine name
    char machine[TAILLE_MAX_NOM+1];    

    gethostname(machine,TAILLE_MAX_NOM);     
 
    // Get adress structure with name
    if ((ptr_hote = gethostbyname(machine)) == NULL) {
      perror("ERROR - Server name not found");
      exit(1);
    }
        
    // Copy ptr_hote to  local_addr
    bcopy((char*)ptr_hote->h_addr, (char*)&local_addr.sin_addr, ptr_hote->h_length);
    local_addr.sin_family     = ptr_hote->h_addrtype;
    local_addr.sin_addr.s_addr   = INADDR_ANY;

    // Use the given port number
    local_addr.sin_port = htons(PORT);
    
    printf("INFO - Connection on server with port : %d \n", ntohs(local_addr.sin_port));
    
    // Socket creation
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("ERROR - Socket doesn't created");
      exit(1);
    }

    // Bind socket_descriptor to local_addr's adress structure
    if ((bind(socket_descriptor, (sockaddr*)(&local_addr), sizeof(local_addr))) < 0) {
      perror("ERROR - Cannot bind socket to client's socket");
      exit(1);
    }

    // Init listening
    listen(socket_descriptor, nb_max_client);

   return socket_descriptor;
}

#endif