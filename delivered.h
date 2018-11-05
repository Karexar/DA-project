#ifndef DELIVERED_H
#define DELIVERED_H

#include <stdio.h>
#include <stdlib.h>

#include "process.h"
#include "log.h"

typedef struct Delivered{
		int src_id;
		char *msg;
} Delivered;

void init_delivered();
int get_last_seq_delivered(int process_id);
void add_delivered(char* msg, int src_process_id);
bool not_delivered_yet(char* msg, int src_id);
void print_delivered();
void free_delivered();

#endif
