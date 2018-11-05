#ifndef ACK_H
#define ACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "process.h"

typedef struct Ack{
		int src;
		char* msg;
		int* from;
} Ack;


void init_ack();
void create_empty_from_list(int index);
int get_acks_len();
int find_ack(char* msg, int src);
void add_ack(char* msg, int src, int from);
bool acked_by_half(char* msg, int src);
void free_ack();
void print_ack();

#endif