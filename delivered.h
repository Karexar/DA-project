#ifndef DELIVERED_H
#define DELIVERED_H

#include <stdio.h>
#include <stdlib.h>

#include "process.h"

typedef struct Delivered{
		int src_id;
		char *msg;
} Delivered;

void init_delivered(int total_broadcast);
void add_delivered(int port, char* msg);
bool not_delivered_yet(int src_id, char* msg);
void print_delivered();
void free_delivered();

#endif
