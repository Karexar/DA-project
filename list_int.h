#ifndef LIST_INT_H
#define LIST_INT_H

#include <stdio.h>
#include <stdlib.h>

typedef struct List_int {
    int size;
    int size_allocated;
    int* list;
} List_int;

List_int* create_list_int(int initial_size);
void add_int(List_int* list, int value);
void print_list_int(List_int* list);
void free_list_int(List_int* list);

#endif
