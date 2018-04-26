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

char* num_to_string_converter(double base26){
  int pass[7];
  for(int i = 0; i < 7; i++){
    pass[6 - i] = ((int)(base26/(pow((double)26, (double)i))) % 26);
  }
  char * password = malloc(sizeof(char) * 7);
  for(int i = 0; i < 7;i++){
    password[i] = int_to_char_map(pass[i]);
}
  return password;
}

double string_to_num_converter(char * str){
  double cur = 0;
  for(int i = 0; i < 7; i++){
    cur += (double)char_to_int_map(str[6 - i]) * (pow((double)26, (double) i));
      }
  return cur;
}

int main(){
  char passy[7];
  strcpy(passy, num_to_string_converter(676));
  printf("%s \n", passy);
  int woo;
  woo = (int)string_to_num_converter(passy);
  printf(" %d \n", woo);
  return 0;
}
  
