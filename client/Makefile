CC = clang                                                                                                                                                                                
CFLAGS = -g

all: password-client

clean:
	rm -f password-client

client: password-client.c
	$(CC) $(CFLAGS) -o password-client password-client.c
