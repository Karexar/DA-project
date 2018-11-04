#ifndef FORWARD_H
#define FORWARD_H

#include <stdio.h>
#include <stdlib.h>


typedef struct Forward{
	char* msg;
	int src;
} Forward;

void init_forward();
void add_forward(char* msg, int src);
void free_forward();

#endif