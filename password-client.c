#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <time.h>
#include "cracker.h"

// Response type
typedef enum {
  REQUEST_MORE,
  PASSWORD_FOUND,
  KEEP_LOOKING,
  SEARCH_SPACE_EXHAUSTED
} response_t;

// Data packet (sent between client and server)
typedef struct packet {
  double starting;
  double ending;
  int client_id;
  char password[8];
  char hash[33];
  response_t response;
} packet_t;

// Global variables
int client_id = 0;

int main(int argc, char *argv[]) {
  // Setup 
  struct hostent* server = gethostbyname(argv[1]);
  if(server == NULL) {
	fprintf(stderr, "Unable to find host %s\n", argv[1]);
	exit(1);
  }

  // Connect to a socket
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if(s == -1) {
	perror("socket failed");
	exit(2);
  }

  struct sockaddr_in addr = {
	.sin_family = AF_INET,
	.sin_port = htons(4009)
  };

  bcopy((char*)server->h_addr, (char*)&addr.sin_addr.s_addr, server->h_length);

  // Attempt connection with server
  if(connect(s, (struct sockaddr*)&addr, sizeof(struct sockaddr_in))) {
	perror("connect failed");
	exit(2);
  }

  // Continuosly communicate with server
  while(true) {
    packet_t packet;
    char password[8];

    // Get server response
    if(read(s, &packet, sizeof(packet_t)) < 0) {
	  perror("read failed");
	  exit(2);
    }

    // Update client id
    client_id = packet.client_id;
  
    // Handler server response appropriately
    switch(packet.response) {
      
      case PASSWORD_FOUND:
        printf("Password found, disconnecting...\n");
        close(s);
        exit (EXIT_SUCCESS);
        break;
      
      case KEEP_LOOKING:
        printf("\033[0;36m"); 
      	printf("Searching...\n");
      	printf("\033[0m"); 
        strcpy(password, find_password(packet.starting, packet.ending, packet.hash));

        // If password has not been found, request for a new search space
        if (strcmp(password, "NOPE") == 0) {
          packet.response = REQUEST_MORE;
          write(s, &packet, sizeof(packet_t));
        } else {
          //Found password, send password back to server
          strcpy(packet.password, password);
          packet.response = PASSWORD_FOUND;
          printf("\033[1;32m"); 
          printf("PASSWORD FOUND: ");
          printf("%s\n", packet.password);
          printf("\033[0m"); 
          write(s, &packet, sizeof(packet_t)); 
        }
        break;
        
        case SEARCH_SPACE_EXHAUSTED:
          printf("Search space exhausted, disconnecting...\n");
          close(s);
          exit(EXIT_SUCCESS);
          break;
        
        default:
          perror("We have lost connection from server, disconnecting...\n");
          close(s);
          exit(EXIT_FAILURE);
          break;
    }
  }
  return 0;
}
