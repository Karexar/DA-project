#ifndef UTILS
#define UTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum{false, true} bool;

typedef enum {
	INTEGER, 
	STRING
} Type;

char* get_main_arg(int argc, char** argv, int index, Type type);
char* get_word(FILE* f);


#endif