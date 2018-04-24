#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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

  char buffer[256];
  int bytes_read = read(s, buffer, 256);
  if(bytes_read < 0) {
	perror("read failed");
	exit(2);
  }

  printf("Server sent: %s\n", buffer);

  close(s);
}
