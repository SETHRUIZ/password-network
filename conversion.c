
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

//switch function from ints to characters
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
	  case 26 :
		  return 'A';
	  case 27 :
		  return 'B';
	  case 28 :
		  return 'C';
	  case 29 :
		  return 'D';
	  case 30 :
		  return 'E';
	  case 31 :
		  return 'F';
	  case 32 :
		  return 'G':
	  case 33 :
		  return 'H';
	  case 34 :
		  return 'I';
	  case 35 :
		  return 'J';
	  case 36 :
		  return 'K';
	  case 37 :
		  return 'L';
	  case 38 :
		  return 'M';
	  case 39 :
		  return 'N';
	  case 40 :
		  return 'O';
	  case 41 :
		  return 'P';
	  case 42 :
		  return 'Q';
	  case 43 :
		  return 'R';
	  case 44 :
		  return 'S';
	  case 45 :
		  return 'T';
	  case 46 :
		  return 'U';
	  case 47 :
		  return 'V';
	  case 48 :
		  return 'W';
	  case 49 :
		  return 'X';
	  case 50 :
		  return 'Y';
	  case 51 :
		  return 'Z';
	  case 52 :
		  return '0';
	  case 53 :
		  return '1';
	  case 54 :
		  return '2';
	  case 55 :
		  return '3';
	  case 56 :
		 return '4';
	  case 57 :
		  return '5';
	  case 58 :
		  return '6';
	  case 59 :
		  return '7';
	  case 60 :
		  return '8';
	  case 61 :
		  return '9';
	  case 62 :
		  return '!';
	  case 63 :
		  return '@';
	  case 64 :
		  return '#';
	  case 65 :
		  return '$';
	  case 66 :
		  return '%';
	  case 67 :
		  return '^';
	  case 68 :
		  return '&';
	  case 69 :
		  return '*';
	  case 70 :
		  return '(';
	  case 71 :
		  return ')';
	  case 72 :
		  return '?';
	  case 73 :
		  return '/';
	  case 74 :
		  return '<';
	  case 75 :
		  return '>';
	  case 76 :
		  return '~';
  default :
    return '\0';
  }
}

// switch function from characters to ints
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
/*converts a double to a string by looping filling an int array with the 
appropriate int gained from dividing 
the double by 76 to the power of it's inverse digit 
then converting the array of ints to a string
by switching each int with it's corresponding char
*/

char* num_to_string_converter(double base76){
  int pass[7] = {0};
  for(int i = 0; i < 7; i++){
    int num = (int) (fmod((base26 / pow(76, i)) , 76));
    pass[6 - i] =  num;
  }

  char * password = malloc(sizeof(char) * 7);
  for(int i = 0; i < 7;i++){
    password[i] = int_to_char_map(pass[i]);
	}

  return password;
}

/*converts a string to a double by looping through each charcter in the string
starting with the leftmost charcter
this character is switched with the corresponding int
this int is then multiplied by 26 to the power of it's inverse postion in the string
*/
double string_to_num_converter(char * str){
  double cur = 0;
  for(int i = 0; i < 7; i++){
    cur += (double)char_to_int_map(str[6 - i]) * (pow((double)76, (double) i));
      }
  return cur;
}

//example main
int main(){
  char passy[7];
  strcpy(passy, num_to_string_converter(2147483648.000));
  //expects aaaaaaa
  printf(" %s\n",passy);
  return 0;
}
  
