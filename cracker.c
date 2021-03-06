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
#include <pthread.h>
#include "cracker.h"

//Citations 
//  TODO 

// Constants
#define MAX_USERNAME_LENGTH 24
#define PASSWORD_LENGTH 7
#define NUM_THREADS 30

// Thread args
typedef struct thread_args {
 double start_num;
 double end_num;
 uint8_t hash[MD5_DIGEST_LENGTH+1];
  char holder[PASSWORD_LENGTH + 1];
  bool password_found;
} thread_args_t;

// Global variables
thread_args_t thread_args[NUM_THREADS]; 
pthread_t threads[NUM_THREADS];  

// Function signatures
int md5_string_to_bytes(const char* md5_string, uint8_t* bytes);
int char_to_int_map(char character);
void check_range(double start_num, double end_num,  uint8_t* hash, char *holder, bool* password_found);
void* crack_passwords_thread(void* void_args);
char* create_word(char c);
char int_to_char_map(int number);
bool check_all_converter(uint8_t* hash, char* word);

/* Searches for matching password given a range*/
char* find_password(double starting, double ending, char* hash) {
  uint8_t hash_bytes[MD5_DIGEST_LENGTH+1];
  md5_string_to_bytes(hash, hash_bytes);
  double SLICE_SIZE = ceil((ending - starting) / NUM_THREADS);
  double start_num = starting;

  for(int i = 0; i < NUM_THREADS; i++) {
    // Create slice args
    thread_args_t args;
    args.password_found = false;
    args.start_num = start_num;
    args.end_num = start_num + SLICE_SIZE;

    // Update start_num
    start_num += SLICE_SIZE + 1;

    memmove(args.hash, hash_bytes,MD5_DIGEST_LENGTH + 1);
    thread_args[i] = args;

    // Create thread, pass args and run it
    pthread_t thread;
    if(pthread_create(&threads[i], NULL, crack_passwords_thread, &thread_args[i]) != 0) {
      perror("Failed to create thread");
      exit(2);
    } 
  }

  // Join all threads
  for(int d = 0; d < NUM_THREADS; d++) {
    pthread_join(threads[d], NULL);
  }

  // If any thread found password, return password
  for(int j = 0; j < NUM_THREADS; j++){ 
    if(thread_args[j].password_found) {
     return thread_args[j].holder;
    }
  }
  // Password not found
  return "NOPE";
}

// Switch ints to characters
char int_to_char_map(int number){
  switch(number){
  case 0 :
     return 'a';
  case 1:
    return 'b';
  case 2:
    return 'c';
  case 3:
    return 'd';
  case 4 :
    return 'e';
  case 5 :
    return 'f';
  case 6 :
    return 'g';
  case 7 :
    return 'h';
  case 8 :
    return 'i';
  case 9 :
    return 'j';
  case 10 :
    return 'k';
  case 11 :
    return 'l';
  case 12 :
    return 'm';
  case 13 :
    return 'n';
  case 14 :
    return 'o';
  case 15 :
    return 'p';
  case 16 :
    return 'q';
  case 17 :
    return 'r';
  case 18 :
    return 's';
  case 19 :
    return 't';
  case 20 :
    return 'u';
  case 21 :
    return 'v';
  case 22 :
    return 'w';
  case 23 :
    return 'x';
  case 24 :
    return 'y';
  case 25 :
    return 'z';
  default :
    return '\0';
  }
}

// Switch characters to ints
int char_to_int_map(char character){
  switch(character){
  case 'a' :
     return 0;
  case 'b' :
    return 1;
  case 'c' :
    return 2;
  case 'd' :
    return 3;
  case 'e' :
    return 4;
  case 'f' :
    return 5;
  case 'g' :
    return 6;
  case 'h' :
    return 7;
  case 'i' :
    return 8;
  case 'j' :
    return 9;
  case 'k' :
    return 10;
  case 'l' :
    return 11;
  case 'm' :
    return 12;
  case 'n' :
    return 13;
  case 'o' :
    return 14;
  case 'p' :
    return 15;
  case 'q' :
    return 16;
  case 'r' :
    return 17;
  case 's' :
    return 18;
  case 't' :
    return 19;
  case 'u' :
    return 20;
  case 'v' :
    return 21;
  case 'w' :
    return 22;
  case 'x' :
    return 23;
  case 'y' :
    return 24;
  case 'z' :
    return 25;
  default :
    return 26;
  }
}

/* Converts a double to a string by looping filling an int array with the 
appropriate int gained from dividing 
the double by 26 to the power of it's inverse digit 
then converting the array of ints to a string
by switching each int with it's corresponding char */
char* num_to_string_converter(double base26){
  int pass[7];
  
  for(int i = 0; i < 7; i++) {
    int num = (int) (fmod((base26 / pow(26, i)), 26));
    pass[6 - i] = num;
  }

  char* password = malloc(sizeof(char) * 7);
  if(password == NULL) {
    perror("Something went wrong with password malloc\n");
    exit(EXIT_FAILURE);
  }

  for(int i = 0; i < 7;i++){
    password[i] = int_to_char_map(pass[i]);
}
  return password;
}

/* Converts a string to a double by looping through each charcter in the string
starting with the leftmost charcter
this character is switched with the corresponding int
this int is then multiplied by 26 to the power of it's inverse postion in the string */
double string_to_num_converter(char * str){
  double cur = 0;
  for(int i = 0; i < 7; i++){
    cur += (double)char_to_int_map(str[6 - i]) * (pow((double)26, (double) i));
      }
  return cur;
}

//crack_passwords_thread
//  @param void_args: thread_args_t
//  @return: thread_args_t type
void* crack_passwords_thread(void* void_args) {
  thread_args_t* args = (thread_args_t*) void_args;
  //check for  passwords starting from start num to end num
  check_range(args->start_num, args->end_num, args->hash, args->holder, &args->password_found);
  return (void*) args;
}

//check_if the password exists in the given range
// @param start_num: a double
// @param end_num: a double
// @param hash: a hashed password
// @param holder: a string
void check_range(double start_num, double end_num,  uint8_t* hash, char *holder, bool* password_found) {
  double cur;
  for(cur = start_num; cur <= end_num; cur++){
    char* cur_word = num_to_string_converter(cur);
    if(check_all_converter(hash, cur_word)){
      strcpy(holder, cur_word);
      *password_found = true;
      return;
    }
    free(cur_word);
  }
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
 * \returns           0 on success,-1 otherwise
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
  }
  return false;
}
