CC = clang                                                                                                                                                                                
CFLAGS = -g -lm -lpthread -lcurl -lcrypto

LIBS = crypto pthread

all: cracker

clean:
	rm -f cracker

client: cracker.c
	$(CC) $(CFLAGS) -o cracker cracker.c 
