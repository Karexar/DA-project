#ifndef VECTOR_CLOCK_H
#define VECTOR_CLOCK_H

#include <stdio.h>
#include <stdlib.h>

#include "process.h"
#include "utils.h"
#include "dependencies.h"

/*
This file manage vector clocks. So for each process, we have a list of
Vector_clock_elem. The size of the list corresponds to the total number 
of processes. Thus the size is only known at runtime, when we read the 
membership file.
Each Vector_clock_elem represents a process_id, and the msg count already
delivered by that process. 
When we broadcast a message, we need to send a vector clock along with 
our message, but we only need to send the entries corresponding to the 
processes we depend on. We need to keep the entire vector clock locally
because we could receive a message with a vector clock containing processes
we do not depend on. In this case we need the corresponding entries 
because we need to wait for the entries of the local VC and the received 
VC to correspond, even if we do not depend on the specified processes. 
For example : p1 broadcast to p2 which broadcast to p3, and p3 will receive 
the msg with a vector clock indicating [[4, 2], [5, 3]] for example, 
indicating that the src of the message depends on 4 and 5, and that 2 and 3 
were the corresponding VC entries when he broadcasted. In fact, p3 don't 
need to know who sent the msg. The only thing p3 has to know is that in order to
deliver the message, it has to receive at least 2 msg from p4 and 3 msg from p5.
*/

typedef struct Vector_clock_elem{
		int process_id;
		int msg_count;
} Vector_clock_elem;

void init_vector_clock();
void increment_vc(int id);
char* get_VC_of_dependencies();
void parse_vc(char* vc_str, Vector_clock_elem** vc, int* vc_size);
bool compare_vector_clock(Vector_clock_elem* vc, int vc_size);
void print_vector_clock();
void free_vector_clock();

#endif
