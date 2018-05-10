#ifndef CRACKER_H_
#define CRACKER_H_

char int_to_char_map(int number);

int char_to_int_map(char character);

char* num_to_string_converter(double base26);

double string_to_num_converter(char * str);

int md5_string_to_bytes(const char* md5_string, uint8_t* bytes);

char* find_password(double starting, double ending, char* hash);

void* crack_passwords_thread(void* void_args);

void check_range(double start_num, double end_num,  uint8_t* hash, char *holder);

int md5_string_to_bytes(const char* md5_string, uint8_t* bytes);

bool check_all_converter(uint8_t* hash, char* word);

#endif