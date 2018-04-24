CC = clang                                                                                                                                                                                
CFLAGS = -g

all: password-server

clean:
	rm -f password-server

client: password-server.c
	$(CC) $(CFLAGS) -o password-server password-server.c
