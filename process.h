#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

typedef struct Process{
		int id;
		char *ip;
		int port;
} Process;

void init_process(char* filename, int id);
void create_process_list(char* filename);
int get_process_id();
char* get_ip_from_id(int process_id);
int get_id_from_index(int index);
int get_index_from_id(int id);
int get_port_from_id(int process_id);
int get_process_count();
int get_id_from_port(int port);
void free_process_list();

#endif