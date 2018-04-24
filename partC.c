#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>


#define MAX_USERNAME_LENGTH 24
#define PASSWORD_LENGTH 6
#define NUM_THREADS 26

//password entry
typedef struct password_entry {
	char username[MAX_USERNAME_LENGTH+1];
	uint8_t password_md5[MD5_DIGEST_LENGTH+1];
	bool cracked;
	char crackedword[7];
	struct password_entry* next;
} password_entry_t;

//Thread arguments
typedef struct thread_args {
	char start_word;
	password_entry_t* passwords;
} thread_args_t;

//function signatures
password_entry_t* read_password_file(const char* filename);
int md5_string_to_bytes(const char* md5_string, uint8_t* bytes);
void print_md5_bytes(const uint8_t* bytes);
void check_all_converter(password_entry_t* passwords, char* word);
void check_range(char* start_word, int k, password_entry_t* passwords);
void* crack_passwords_thread(void* void_args);
char* create_word(char c); 

int main(int argc, char** argv) {

	char alph[NUM_THREADS][PASSWORD_LENGTH + 1]; //This will store all starting words of the same letter "aaaaaa" ... "zzzzzz"
	pthread_t threads[NUM_THREADS];  //holds our threads
	thread_args_t thread_args[NUM_THREADS]; //holds all the thread arguments

	if(argc != 2) {
		fprintf(stderr, "Usage: %s <path to password directory file>\n", argv[0]);
		exit(1);  
	}

	// Read in the password file
	password_entry_t* password_entries = read_password_file(argv[1]);

	char c = 'a';
	for(int i = 0; i < NUM_THREADS; i++) {

		//create the args
		thread_args_t args;
		args.passwords = password_entries;
		args.start_word = c++;
		thread_args[i] = args;

		//pass to thread
		pthread_t thread;

		if(pthread_create(&threads[i], NULL, crack_passwords_thread, &thread_args[i]) != 0) {
			perror("Failed to create thread");
			exit(2);
		} 
	}

	//join all the threads
	for(int d = 0; d < NUM_THREADS; d++) {
		pthread_join(threads[d], NULL);
	}

	//print all the cracked passwords and usernames
	password_entry_t* current = password_entries;
	while(current != NULL) {
		printf("%s ", current->username);
		printf("%s \n", current->crackedword);
		current = current->next;
	}

}


//create_word
// @param c: a char which is a first letter of 6 letter word
// @return : returns a six letter word containing c's 
char* create_word(char c) {

	char* arr = (char*) malloc(sizeof(char) * (PASSWORD_LENGTH + 1));

	for(int i = 0; i < PASSWORD_LENGTH; i++) {
		arr[i] = c;
	}

	arr[PASSWORD_LENGTH] = '\0';
	return arr;
}

//crack_passwords_thread
//  @param void_args: thread_args_t
//  @return: thread_args_t type
void* crack_passwords_thread(void* void_args) {

	thread_args_t* args = (thread_args_t*) void_args;
	password_entry_t* passwords = args->passwords;
	char* start_word = create_word(args->start_word);

	//check for  passwords starting from start_word
	check_range(start_word, PASSWORD_LENGTH, passwords);
	free(start_word);

	return (void*) args;  
}

//check_range
// @param start_word: a string
// @param k: an integer that indicates index of character that needs to be changed
// @param passwords: a poitner to the head of the passwords list
void check_range(char* start_word, int k, password_entry_t* passwords) {
	if(k == 1) {
		check_all_converter(passwords, start_word);
		return;
	}   
	for(char c = 'a'; c <= 'z'; c++) {
		start_word[k - 1] = c;
		check_range(start_word, k - 1, passwords);
	}
	return;
}

/**
 * Read a file of username and MD5 passwords. Return a linked list
 * of entries.
 * \param filename  The path to the password file
 * \returns         A pointer to the first node in the password list
 */
password_entry_t* read_password_file(const char* filename) {
	// Open the password file
	FILE* password_file = fopen(filename, "r");
	if(password_file == NULL) {
		perror("opening password file");
		exit(2);
	}

	// Keep track of the current list
	password_entry_t* list = NULL;

	// Read until we hit the end of the file
	while(!feof(password_file)) {
		// Make space for a new node
		password_entry_t* newnode = (password_entry_t*)malloc(sizeof(password_entry_t));

		// Make space to hold the MD5 string
		char md5_string[MD5_DIGEST_LENGTH * 2 + 1];

		// Try to read. The space in the format string is required to eat the newline
		if(fscanf(password_file, "%s %s ", newnode->username, md5_string) != 2) {
			fprintf(stderr, "Error reading password file: malformed line\n");
			exit(2);
		}

		// Convert the MD5 string to MD5 bytes in our new node
		if(md5_string_to_bytes(md5_string, newnode->password_md5) != 0) {
			fprintf(stderr, "Error reading MD5\n");
			exit(2);
		}

		// Add the new node to the front of the list
		newnode->next = list;
		list = newnode;
	}

	return list;
}

/**
 * Convert a string representation of an MD5 hash to a sequence
 * of bytes. The input md5_string must be 32 characters long, and
 * the output buffer bytes must have room for MD5_DIGEST_LENGTH
 * bytes.
 *
 * \param md5_string  The md5 string representation
 * \param bytes       The destination buffer for the converted md5 hash
 * \returns           0 on success, -1 otherwise
 */
int md5_string_to_bytes(const char* md5_string, uint8_t* bytes) {
	// Check for a valid MD5 string
	if(strlen(md5_string) != 2 * MD5_DIGEST_LENGTH) return -1;

	// Start our "cursor" at the start of the string
	const char* pos = md5_string;

	// Loop until we've read enough bytes
	for(size_t i=0; i<MD5_DIGEST_LENGTH; i++) {
		// Read one byte (two characters)
		int rc = sscanf(pos, "%2hhx", &bytes[i]);
		if(rc != 1) return -1;

		// Move the "cursor" to the next hexadecimal byte
		pos += 2;
	}

	return 0;
}

/**
 * Print a byte array that holds an MD5 hash to standard output.
 * 
 * \param bytes   An array of bytes from an MD5 hash function
 */
void print_md5_bytes(const uint8_t* bytes) {
	for(size_t i=0; i<MD5_DIGEST_LENGTH; i++) {
		printf("%hhx", bytes[i]);
	}
}

//check_all_converter
//  @param: passwords: pointer to the passwords list
//  @param: word: a string
//  @return: returns true if the hashes match, else returns false
//
// given word, compares the md5 hash for that word with hashcheck
void check_all_converter(password_entry_t* passwords, char* word){

	uint8_t password_ciphertest[MD5_DIGEST_LENGTH];
	MD5((unsigned char*)word, strlen(word), password_ciphertest);
	password_entry_t* current = passwords;

	//compare word's MD5 hash with all those in passwords
	while(current != NULL){   
		if(memcmp(current->password_md5, password_ciphertest, MD5_DIGEST_LENGTH) == 0) {
			current->cracked = true; //found a matching password
			strcpy(current->crackedword, word);
			return;
		}

		current = current->next;  
	}
	return;
}






