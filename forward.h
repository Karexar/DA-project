#ifndef FORWARD_H
#define FORWARD_H

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "ack.h"
#include "delivered.h"
#include "setting.h"


typedef struct Forward{
	char* payload;
	int src;
	char* vc_str;
} Forward;

void init_forward();
bool not_forwarded_yet(char* msg, int src);
void add_forward(char* payload, int src, char* vc_str);
void check_forward_to_deliver();
void print_forward();
void free_forward();

#endif