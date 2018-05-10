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

//Response types
typedef enum {
  REQUEST_MORE,
  PASSWORD_FOUND,
  KEEP_LOOKING
} response_t;

//Data packets sent between client and server
typedef struct packet {
  double starting;
  double ending;
  int client_id;
  char password[8];
  char hash[33];
  response_t response;
} packet_t;

int client_id = 0;

int main(int argc, char *argv[]) {

  struct hostent* server = gethostbyname(argv[1]);
  if(server == NULL) {
	fprintf(stderr, "Unable to find host %s\n", argv[1]);
	exit(1);
  }


  int s = socket(AF_INET, SOCK_STREAM, 0);
  if(s == -1) {
	perror("socket failed");
	exit(2);
  }

  struct sockaddr_in addr = {
	.sin_family = AF_INET,
	.sin_port = htons(4444)
  };


  bcopy((char*)server->h_addr, (char*)&addr.sin_addr.s_addr, server->h_length);


  if(connect(s, (struct sockaddr*)&addr, sizeof(struct sockaddr_in))) {
	perror("connect failed");
	exit(2);
  }

 //TODO, REMOVE THESE 3 LINES
  srand(time(0));
  int i = 0;
  char passwords[5][8] = {"NOPE", "NOPE", "NOPE", "NOPE", "abcdefg" };

  while(true) {

    packet_t packet;
    char password[8];

    if(read(s, &packet, sizeof(packet_t)) < 0) {
	  perror("read failed");
	  exit(2);
    }

    client_id = packet.client_id;
    printf("hash: %s\n", packet.hash);
    printf("starting: %f\n", packet.starting);
    printf("ending: %f\n", packet.ending);


    
    switch(packet.response) {
      case REQUEST_MORE: 
        close(s);
        perror("Error: We've lost connection to the server.\n");
        exit(EXIT_FAILURE);
      case PASSWORD_FOUND:
        printf("Password found, disconnecting...\n");
        close(s);
        exit (EXIT_SUCCESS);
      case KEEP_LOOKING:
        printf("Searching...\n");

        //TODO REPLACE SLEEP WITH SETH'S PASSWORD LOOKUP HERE
          // - get the starting double with packet.starting
          // - get the ending double with packet.ending
          // - get the hash by doing a memcpy of packet.hash
          // - pass it to seth's password lookup method

        sleep(rand() % 10);

        //If password has not been found, request for a new search space
        if (strcmp(passwords[i++], "NOPE") == 0) {
          packet.response = REQUEST_MORE;
          write(s, &packet, sizeof(packet_t)); 
        } else {
          //else we found password, send password back to server
          strcpy(packet.password, passwords[i-1]);
          packet.response = PASSWORD_FOUND;
          printf("I FOUND THE PASSWORD: ");
          printf("%s\n", packet.password);
          write(s, &packet, sizeof(packet_t));
        }

        //TODO: consider adding default case(good programming practice)
    }
  }
  return 0;
}
