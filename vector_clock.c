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
char* get_VC_of_dependencies() { 
	// First we compute the size
	// 22 is 1 space + 1 return + 2*10 (max size of two int as string)
	char* dep_string = (char*)malloc(get_size_dependencies()*22 + 1);
	dep_string[0] = '\0';
	// Add all vc entries corresponding to a dependency to the string
	for (int i=0;i<size_vector_clock;++i) {
		if(is_a_dependency(vector_clock[i].process_id)) {
			sprintf(dep_string, "%s%d %d\n", dep_string, vector_clock[i].process_id, vector_clock[i].msg_count);
		}
	}
	return dep_string;
}

/*bool compare_vector_clock() {

}

char* get_vector_clock() {

}*/

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