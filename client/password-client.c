#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>

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
  char password[8]
  response_t response;
} packet_t;

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

  while(true) {

    packet_t packet;

    if(read(s, &packet, sizeof(packet_t)) < 0) {
	  perror("read failed");
	  exit(2);
    }

    printf("starting: %d\n", packet.starting);
    printf("ending: %d\n", packet.ending);
    printf("client_id: %d\n", packet.client_id);
    switch(packet.response) {
      case PASSWORD_FOUND:
         printf("response: PASWORD_FOUND");
         break;
      case REQUEST_MORE: 
        printf("response: REQUEST_MORE");
        break;
    }

    // Check if response is NULL???

    if (packet.response != PASSWORD_FOUND) {
      /*
      char password[8];
      strcpy(password,password_search(starting, ending));

      if (strlen(password) == 7) {
        strcpy(packet.password, password);
        packet.response = PASSWORD_FOUND;
        write(socket_fd_copy, &packet, sizeof(packet_t));  
      } else {
        packet.response = REQUEST_MORE;
        write(socket_fd_copy, &packet, sizeof(packet_t)); 
      */
    } else {
      break;
    }
  }
   close(s);
}
