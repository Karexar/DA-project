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