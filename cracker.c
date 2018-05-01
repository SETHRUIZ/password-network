#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>


#define MAX_USERNAME_LENGTH 24
#define PASSWORD_LENGTH 7
//dont just divide up by alphabet, divide up by start and end word
#define NUM_THREADS 30

//TODO
// figure out num threads
// pass in three values from client

//Thread arguments
typedef struct thread_args {
 double start_num;
 double end_num;
 uint8_t* hash[MD5_DIGEST_LENGTH+1];
  char holder[7];
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

  if(argc != 4) {
    fprintf(stderr, "Usage: %s <path to password directory file>\n", argv[0]);
    exit(1);  
  }

  // Read in the password file
  password_entry_t* password_entries = read_password_file(argv[1]);
  double start = argv[2];
  double end = argv[3];
  //probably have to floor or ceiling this and keep track of that
  double slice_size = floor((end - start)/NUM_THREADS);
  double start_num = 0;
  double end_num;



  for(int i = 0; i < NUM_THREADS; i++) {

    //create the args
    thread_args_t args;
    args.passwords = password_entries;
    start_num +=  slice_size;
    end_num = start + slice_size - 1;
    args.start_num = start;
    args.end_num = end;
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



//crack_passwords_thread
//  @param void_args: thread_args_t
//  @return: thread_args_t type
void* crack_passwords_thread(void* void_args)) {

  thread_args_t* args = (thread_args_t*) void_args;
  uint8_t* hash = args->hash;
        
  //check for  passwords starting from start num to end num
  check_range(args->start_nums, args->end_num, PASSWORD_LENGTH, hash, args->holder);
  free(start_word);

  return (void*) args;  
                                                            }

//TODO


//check_range
// @param start_word: a string
// @param k: an integer that indicates index of character that needs to be changed
// @param hash: a hashed password
void check_range(double start_num, double end_num,  uint8_t* hash, char *holder) {
  double cur;
  for(cur = start_num; cur <= end_num; cur++){
    if(check_all_converter(hash, num_to_string_converter(start))){
      strcpy(holder, start_word);
      return;
    }
  }
  char *nope = "nope";
  strcpy(holder, nope);
  return;
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
bool check_all_converter(uint8_t* hash, char* word){

  uint8_t password_ciphertest[MD5_DIGEST_LENGTH];
  MD5((unsigned char*)word, strlen(word), password_ciphertest);

  //compare word's MD5 hash with the given hash  
  if(memcmp(hash, password_ciphertest, MD5_DIGEST_LENGTH) == 0) {	
    return true;
  } else {
    return false;
  }
}
