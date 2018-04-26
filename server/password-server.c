
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 4444

//Response types
typedef enum {
  REQUEST_MORE,
  PASSWORD_FOUND,
} response_t;


//Data packets sent between client and server
typedef struct packet {
	int starting;
	int ending; 
	int client_id;
	char password [8];
	response_t response;
} packet_t;

//TODO add the global that keeps track of the number of clients we have, that will create the client ids we need

//CLIENT STRUCTS
typedef struct client {
 struct client* next;
 int client_id;
 int port;
 char ipstr[INET_ADDRSTRLEN]; 
} client_node_t;

typedef struct client_list {
  client_node_t* head;
} client_list_t;

client_list_t* client_list;



///////  CLIENT LINKED LIST METHODS   ////////////

//Appends client node to linked list
void append_node(int client_id, int port, char* ipstr) {

  //allocate memory for the new client node
  client_node_t* new_node = malloc(sizeof(client_node_t));
  if(new_node == NULL) {
    perror("Malloc failed");
    exit(2);
  }

  //populate the client node fields
  new_node->client_id = client_id;
  new_node->port = port;
  strcpy(new_node->ipstr, ipstr);
  new_node->next = NULL;

  //if list is empty, add client to empty list
  if(client_list->head == NULL) {
    client_list->head = new_node;
    return;
  }

  //otherwise add to end of list
  client_node_t* cur = client_list->head;
  while(cur->next != NULL) {
    cur = cur->next;
  }  
  cur->next  = new_node;
  return;
}


//Initializes the client node list
void init_client_list() {
  client_list = malloc(sizeof(client_list_t));
  client_list->head = NULL;
}



typedef struct thread_arg {
  int socket_fd;
  int client_number;
} thread_arg_t;

void* client_thread_fn(void* p) {
  // Unpack the thread arguments
  thread_arg_t* args = (thread_arg_t*)p;
  int socket_fd = args->socket_fd;
  int client_number = args->client_number;
  free(args);
  
  // Duplicate the socket_fd so we can open it twice, once for input and once for output
  int socket_fd_copy = dup(socket_fd);
  if(socket_fd_copy == -1) {
    perror("dup failed");
    exit(EXIT_FAILURE);
  }


  packet_t packet;

  packet.starting = 10;
  packet.ending = 20;
  packet.client_id = client_number;
  packet.is_found = false;

  write(socket_fd_copy, &packet, sizeof(packet_t));

 
  // Open the socket as a FILE stream so we can use fgets
  FILE* input = fdopen(socket_fd, "r");
  FILE* output = fdopen(socket_fd_copy, "w");
  
  // Check for errors
  if(input == NULL || output == NULL) {
    perror("fdopen failed");
    exit(EXIT_FAILURE);
  }
  
  // Read lines until we hit the end of the input (the client disconnects)
  char* line = NULL;
  size_t linecap = 0;
  while(getline(&line, &linecap, input) > 0) {
    // Print a message on the server
    printf("Client %d sent %s", client_number, line);
    
    // Send the message to the client. Flush the buffer so the message is actually sent.
    fprintf(output, "%s", line);
    fflush(output);
  }
  
  // When we're done, we should free the line from getline
  free(line);
  
  // Print information on the server side
  printf("Client %d disconnected.\n", client_number);
  
  return NULL;
}

int main() {
  // Set up a socket
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if(s == -1) {
    perror("socket");
    exit(2);
  }

  // Listen at this address. We'll bind to port 0 to accept any available port
  struct sockaddr_in addr = {
    .sin_addr.s_addr = INADDR_ANY,
    .sin_family = AF_INET,
    .sin_port = htons(SERVER_PORT)
  };

  // Bind to the specified address
  if(bind(s, (struct sockaddr*)&addr, sizeof(struct sockaddr_in))) {
    perror("bind");
    exit(2);
  }

  // Become a server socket
  listen(s, 2);
  
  // Get the listening socket info so we can find out which port we're using
  socklen_t addr_size = sizeof(struct sockaddr_in);
  getsockname(s, (struct sockaddr *) &addr, &addr_size);
  
  // Print the port information
  printf("Listening on port %d\n", ntohs(addr.sin_port));
  
  int client_count = 0;

  // Repeatedly accept connections
  while(true) {
    // Accept a client connection
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(struct sockaddr_in);
    int client_socket = accept(s, (struct sockaddr*)&client_addr, &client_addr_len);
    
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ipstr, INET_ADDRSTRLEN);
    
    printf("Client %d connected from %s\n", client_count, ipstr);
    
    // Set up arguments for the client thread
    thread_arg_t* args = malloc(sizeof(thread_arg_t));
    args->socket_fd = client_socket;
    args->client_number = client_count;
    
    // Create the thread
    pthread_t thread;
    if(pthread_create(&thread, NULL, client_thread_fn, args)) {
      perror("pthread_create failed");
      exit(EXIT_FAILURE);
    }
    
    client_count++;
  }
  close(s);
}


