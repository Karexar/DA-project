#include "vector_clock.h"

static Vector_clock_elem* vector_clock;
static int size_vector_clock;

void init_vector_clock() {
	size_vector_clock = get_process_count();
	vector_clock = (Vector_clock_elem*)malloc(size_vector_clock*sizeof(Vector_clock_elem));
	Process** p = get_processes();
	for (int i=0;i<size_vector_clock;++i) {
		vector_clock[i].process_id = p[i]->id;
		vector_clock[i].msg_count = 0;
	}
}

void increment_vc(int id) {
	bool not_found = true;
	int i = 0;
	while(not_found && i < size_vector_clock) {
		if(vector_clock[i].process_id == id) {
			not_found = false;
			++(vector_clock[i].msg_count);
		}
		++i;
	}
	if (not_found) {
		printf("Error : cannot find id in increment_vc\n");
		exit(1);
	}
}

// need to free
// return a string containing on each line a process id and the 
// number of message received by this process, with a space 
// as separator. The last line do not have a carriage return. 
char* get_VC_of_dependencies() { 
	// First we compute the size
	// 22 is 1 space + 1 return + 2*10 (max size of two int as string)
	char* dep_string = (char*)malloc(get_size_dependencies()*22 + 1);
	dep_string[0] = '\0';
	// Add all vc entries corresponding to a dependency to the string
	for (int i=0;i<size_vector_clock;++i) {
		if(is_a_dependency(vector_clock[i].process_id)) {
			if (i+1==size_vector_clock) {
				sprintf(dep_string, "%s%d %d", dep_string, vector_clock[i].process_id, vector_clock[i].msg_count);
			}
			else {
				sprintf(dep_string, "%s%d %d\n", dep_string, vector_clock[i].process_id, vector_clock[i].msg_count);
			}
		}
	}
	return dep_string;
}

void parse_vc(char* msg, char** payload, Vector_clock_elem** vc, int* vc_size) {
	int i = 0;
	int nb_lines = 1;
	// count the number of lines
	while (i < strlen(msg)) {
		if (msg[i] == '\n') {
			++nb_lines;
		}
		++i;
	}
	*vc_size = nb_lines-1;
	if (nb_lines > 1) {
		// rewind and read the first one (the payload)
		*payload=(char*)malloc(11);
		i=0;
		while (msg[i] != '\n') {
			(*payload)[i] = msg[i];
			++i;
		}
		(*payload)[i] = '\0';
		++i;
		if (msg[i] != '\0') {
			*vc=(Vector_clock_elem*)malloc(nb_lines*sizeof(Vector_clock_elem));
			for (int line=0;line<nb_lines;++line) {
				// read id
				char id_string[11];
				int j= 0;
				while (msg[i+j]!=' ') {
					id_string[j] = msg[i+j];
					++j;
				}
				id_string[j] = '\0';
				// read msg count
				i = i+j+1;
				char msg_count_string[11];
				j = 0;
				while (msg[i+j]!='\n' && msg[i+j]!='\0') {
					msg_count_string[j] = msg[i+j];
					++j;
				}
				msg_count_string[j]='\0';
				i = i+j+1;
				(*vc)[line].process_id = atoi(id_string);
				(*vc)[line].msg_count = atoi(msg_count_string);
			}
		}
		else {
			*vc_size = 0;
		}
	}
}


bool compare_vector_clock(Vector_clock_elem* vc, int vc_size) {
	for (int i=0;i<vc_size;++i) {
		// find the corresponding entry in the current vc
		for (int j=0;j<size_vector_clock;++j) {
			// if the current process receive less messages from 
			// the dependency than what the src received, we have to wait
			if (vc[i].process_id == vector_clock[j].process_id &&
				vc[i].msg_count > vector_clock[j].msg_count) {
				return false;
			}
		}
	}
	return true;
}

void print_vector_clock() {
	printf("Vector clock : \n");
	for (int i=0;i<size_vector_clock;++i) {
		printf("%d %d\n",vector_clock[i].process_id, vector_clock[i].msg_count);
	}
}

void free_vector_clock() {
	if (vector_clock != NULL) {
		free(vector_clock);
		vector_clock = NULL;
	}
}