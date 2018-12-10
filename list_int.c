#include "list_int.h"


List_int* create_list_int(int initial_size) {
	List_int* list = (List_int*)malloc(sizeof(List_int));
	if (list == NULL) {
		printf("Error : failed to malloc list_int in create_list_int\n");
		exit(1);
	}
	list->size = 0;
	list->size_allocated = initial_size;
	list->list = (int*)malloc(list->size_allocated*sizeof(int));
	if (list->list == NULL) {
		printf("Error : failed to malloc list->list in create_list_int\n");
		exit(1);
	}
	return list;
}

void add_int(List_int* list, int value) {
    if (list->size >= list->size_allocated) {
    	list->size_allocated *= 2;
    	list->list = realloc(list->list, list->size_allocated*sizeof(int));
    	if (list->list == NULL) {
    		printf("Error : failed to realloc list->list in add_int\n");
			exit(1);
    	}
    }
    list->list[list->size] = value;
    ++list->size;
}

void print_list_int(List_int* list){
	for (int i=0;i<list->size;++i) {
		printf("%d ", list->list[i]);
	}
	printf("\n");
}

void free_list_int(List_int* list) {
	free(list->list);
	list->list = NULL;
	free(list);
	list = NULL;
}