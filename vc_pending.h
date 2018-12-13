#ifndef VC_PENDING_H
#define VC_PENDING_H

#include <stdio.h>
#include <stdlib.h>
#include "vector_clock.h"
#include "algo.h"

typedef struct VC_pending{
	int src;
	char* payload;
	Vector_clock_elem* vc;
	int vc_size;
	int from_id;
	char* vc_str;
} VC_pending;

void init_VC_pending();
void add_VC_pending(int src, char* payload, Vector_clock_elem* vc, int vc_size, char* vc_str, int from_id);
void check_pending_vc_to_urb_deliver();
void print_VC_pending();
void free_VC_pending();

#endif