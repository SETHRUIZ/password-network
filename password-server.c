
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <math.h>
#include "cracker.h"

// Citations
// (1)  http://web.theurbanpenguin.com/adding-color-to-your-output-from-c/ 
//          - we used this site to change color of commandline output

// Constants
#define SERVER_PORT 4009
#define NUM_SLICES 1024
#define END_NUM 8031810560
#define PASSWORD_LEN 8

// Response type
typedef enum {
  REQUEST_MORE,
  PASSWORD_FOUND,
  KEEP_LOOKING,
  SEARCH_SPACE_EXHAUSTED
} response_t;

// Slice (Contains search space)
typedef struct slice {
  double start;
  double end;
  int client_id;
} slice_t;

// Data packet (sent between client and server)
typedef struct packet {
	double starting;
	double ending; 
	int client_id;
	char password [PASSWORD_LEN];
  char hash [33];
	response_t response;
} packet_t;

// Thread node
typedef struct thread_node {
	pthread_t thread;
	struct thread_node* next;
} thread_node_t;

// Threads list 
typedef struct threads {
	 thread_node_t* head_thread;
} threads_list_t;

// Thread arg
typedef struct thread_arg {
  int socket_fd;
  int client_number;
} thread_arg_t;

// Global variables
threads_list_t* threads_list;
slice_t slices[NUM_SLICES];
char HASH[100];
bool CLIENT_FOUND = false;
int s = 0;
pthread_mutex_t lock;
int UNEXPLORED_SLICE = 0;

// Function signatures
int get_search_space(); 
void append_thread_node(pthread_t thread);
void join_threads(); 
void append_node(int client_id, int port, char* ipstr);
void init_thread_list();
void init_slices(); 
void shut_down(); 
void* time_out_fn(void* p);
void* client_thread_fn(void* p);

/* Main Driver function */
int main() {

  // Get the hash
  printf("\033[1;35m");
  printf("Enter md5 hash: ");
  printf("\033[0;0m");
  scanf("%s", HASH);

  // Set up a socket
  s = socket(AF_INET, SOCK_STREAM, 0);
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
  
  // Print Port information
  printf("\033[1;35m"); 
  printf("Listening on port %d\n", ntohs(addr.sin_port));
  printf("\033[0;0m");   

  int client_count = 0;
  init_thread_list();
  init_slices();

  // Repeatedly accept connections
  while(true) {
 
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(struct sockaddr_in);

   // Accept a client connection
    int client_socket = accept(s, (struct sockaddr*)&client_addr, &client_addr_len);
    
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ipstr, INET_ADDRSTRLEN);
	
    printf("\033[0;35m"); 
    printf("Client %d connected from %s\n", client_count, ipstr);
    printf("\033[0;0m"); 
    // Set up arguments for the client thread
    thread_arg_t* args = malloc(sizeof(thread_arg_t));

    if(args == NULL) {
      perror("Something went wrong thread args malloc\n");
      exit(EXIT_FAILURE);
    }

    args->socket_fd = client_socket;
    args->client_number = client_count;
    
    // Create thread for communicating with accepted client
    pthread_t thread;

    if(pthread_create(&thread, NULL, client_thread_fn, args)) {
      perror("pthread_create failed");
      exit(EXIT_FAILURE);
    }

    append_thread_node(thread);
    client_count++;
  }
 return 0;
}

//METHOD IMPLEMENTATIONS

/* Creates a linked list of threads */
void init_thread_list() {
  threads_list = malloc(sizeof(threads_list_t));

  if(threads_list == NULL) {
    perror("Failed to malloc threads list\n");
    exit(EXIT_FAILURE);
  }

  threads_list->head_thread = NULL;
}

/* Creates the search space slices */
void init_slices() {
  double SLICE_SIZE = END_NUM / NUM_SLICES;

  for(int i = 0; i < NUM_SLICES; i++) {
    slice_t slice;
    slice.start = i * SLICE_SIZE;

    if(i == NUM_SLICES - 1) { 
      slice.end = slice.start + SLICE_SIZE;
    } else {
    slice.end = (slice.start + SLICE_SIZE) - 1;
  }

    slice.client_id = 0;
    slices[i] = slice;
  }
}

/* Appends thread to threads linked list */
void append_thread_node(pthread_t thread) {
	//Allocate memory for the new thread
	thread_node_t* new_thread = malloc(sizeof(thread_node_t));

	if(new_thread == NULL) {
		perror("Malloc failed thread create linked list\n");
		exit(EXIT_FAILURE);
	}

	new_thread->thread = thread;
  new_thread->next = NULL;

	if(threads_list->head_thread == NULL) {
		threads_list->head_thread = new_thread;
		return;
	}

	thread_node_t* cur = threads_list->head_thread;
	while(cur->next != NULL) {
		cur = cur->next;
	}
	cur->next = new_thread;
}

/* Joins all the threads */
void join_threads() {
	thread_node_t* cur = threads_list->head_thread;
	while(cur != NULL) {
		pthread_join(cur->thread, NULL);
		cur = cur->next;
	}
}

/* Shutdown procedures */
void shut_down() {
  join_threads();
  close(s);
  exit(EXIT_SUCCESS);
}

/* Waits for 15 seconds before shutting down server */
void* time_out_fn(void* p) {
printf("\033[1;35m"); 
printf("Waiting for remaining clients now...\n");
sleep(15);
printf("\033[0;0m");
printf("\033[1;35m"); 
printf("TIME OUT FINISHED, SERVER SHUTTING DOWN\n");
printf("\033[0;0m"); 
shut_down();
return NULL;
} 

/* Handles communication with the client */
void* client_thread_fn(void* p) {
  // Unpack the thread arguments
  thread_arg_t* args = (thread_arg_t*)p;
  int socket_fd = args->socket_fd;
  int client_number = args->client_number;
  free(args);
  
  // Duplicate the socket_fd so we can open it twice, once for input and once for output
  int socket_fd_copy = dup(socket_fd);
  if (socket_fd_copy == -1) {
    perror("dup failed");
    exit(EXIT_FAILURE);
  }

  // Give the client initial search space
  int index;   
  pthread_mutex_lock(&lock);
  if((index = get_search_space()) == -1) {
        perror("Something went wrong with getting seach space\n");
        exit(EXIT_FAILURE);
    }
  pthread_mutex_unlock(&lock);

  packet_t packet;
  packet.starting = slices[index].start;
  packet.ending = slices[index].end;
  packet.client_id = client_number;
  strcpy(packet.hash, HASH);
  packet.response = KEEP_LOOKING;
  write(socket_fd_copy, &packet, sizeof(packet_t));

  //Keep dishing out new search spaces until password is found
  while (!CLIENT_FOUND) {
      // Get response from client
      read(socket_fd_copy, &packet, sizeof(packet_t));
        
        //Respond back to client appropriately
        switch(packet.response) {
          
          case PASSWORD_FOUND:
            CLIENT_FOUND = true;
            packet.client_id = client_number;
            packet.response = PASSWORD_FOUND;
            write(socket_fd_copy, &packet, sizeof(packet_t)); 
            printf("\033[1;92m"); 
            printf("Client %d found password: %s\n", packet.client_id, packet.password);
            printf("\033[0;0m"); 

          //Wait for 15 seconds, until server shutdown
  		  	pthread_t thread;
   		 	if(pthread_create(&thread, NULL, time_out_fn, NULL)) {
      		perror("pthread_create failed");
     		 exit(EXIT_FAILURE);
    		}
              break;

          case REQUEST_MORE: 
              //If search space has been exhausted, close client
              pthread_mutex_lock(&lock);
              if((index = get_search_space()) == -1) {
                pthread_mutex_unlock(&lock);
                packet.client_id = client_number;
                printf("\033[1;31m"); 
                printf("Search space exhausted, Client %d disconnected.\n", packet.client_id);
                printf("\033[0;0m"); 
                packet.response = SEARCH_SPACE_EXHAUSTED;
                write(socket_fd_copy, &packet, sizeof(packet_t));
               } else {
                pthread_mutex_unlock(&lock);
            // Dish out another search space
            strcpy(packet.hash, HASH);
            packet.starting = slices[index].start;
            packet.ending = slices[index].end;
            packet.client_id = client_number;
            packet.response = KEEP_LOOKING;
            write(socket_fd_copy, &packet, sizeof(packet_t));
          }
            break;

          default:
              break;
        }
  }

 //If password found, inform all clients
  packet.response = PASSWORD_FOUND;
  write(socket_fd_copy, &packet, sizeof(packet_t));
  printf("\033[1;31m"); 
  printf("Client %d disconnected.\n", packet.client_id);
  printf("\033[0;0m"); 
  return NULL;
}

/* Returns the index of an unexplored search space */
int get_search_space() {
    if(UNEXPLORED_SLICE < NUM_SLICES) {
      printf("\033[0;36m"); 
      printf("slice: %s\n", num_to_string_converter(slices[UNEXPLORED_SLICE].start));
      printf("\033[0;0m"); 
      return UNEXPLORED_SLICE++;
    }
  return -1;
}


