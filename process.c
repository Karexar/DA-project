#include "process.h"

static Process** p;
static int process_count;
static int process_id;

void init_process(char* filename, int id) {
	process_id = id;
	create_process_list(filename);
}

// Create the list of processes containing id, ip, and port
// Return the list of processes as a double pointer
// NEED TO FREE the list, each processes, and ip. 
void create_process_list(char* filename) {
	FILE *f = fopen(filename, "r");
	if(f == NULL) {
		printf("Cannot open '%s'\n", filename);
		exit(0);
	}
	else {
		// Get the first line
		char s_count[10];
		fgets(s_count, 10, f);
		process_count = atoi(s_count);
		// Allocate the array of pointers to process
		p = (Process**)malloc(process_count*sizeof(Process*));

		for (int i=0;i<process_count;++i) {	
			// for each process, allocate the memory and fill the struct
			p[i] = (Process*)malloc(sizeof(Process));
			char *s = get_word(f, NULL, NULL);
			p[i]->id = atoi(s);
			s = get_word(f, NULL, NULL);
			p[i]->ip = s;
			s = get_word(f, NULL, NULL);
			p[i]->port = atoi(s);
		}
		fclose(f);
	}
}

int get_process_id() {
	return process_id;
}

char* get_ip_from_id(int process_id) {
	int i = 0;
	while (i < process_count) {
		if(p[i]->id == process_id) {
			return p[i]->ip;
		}
		++i;
	}
	printf("Error : cannot find the given id in the process list\n");
	exit(0);
}

int get_id_from_index(int index){
	return p[index]->id;
} 

int get_index_from_id(int id) {
	for (int i=0;i<process_count;++i) {
		if (p[i]->id == id) {
			return i;
		}
	}
	printf("Error : cannot find the index corresponding to the given id\n");
	exit(0);
}

int get_port_from_id(int process_id) {
	int i = 0;
	while (i < process_count) {
		if(p[i]->id == process_id) {
			return p[i]->port;
		}
		++i;
	}
	printf("Error : cannot find the given id in the process list\n");
	exit(0);
}

int get_process_count() {
	return process_count;
}

int get_id_from_port(int port) {
	for (int i=0;i<process_count;++i){
		if (p[i]->port == port) {
			return p[i]->id;
		}
	}
	printf("Error : Could not find id corresponding to that port");
	exit(0);
}

void free_process_list() {
	for (int i=0 ; i < process_count ; ++i){
		free(p[i]->ip);
		p[i]->ip = NULL;
		free(p[i]);
		p[i] = NULL;
	}
	free(p);
	p = NULL;
}
