CC = clang                                                                                                                                                                                
CFLAGS = -g -lm -lpthread -lcurl -lcrypto 

LIBS = crypto pthread

all: password-server password-client

clean:
	rm -f password-server password-client

password-client: password-client.c
	$(CC) $(CFLAGS) -o password-client password-client.c cracker.c

password-server: password-server.c
	$(CC) $(CFLAGS) -o password-server password-server.c
