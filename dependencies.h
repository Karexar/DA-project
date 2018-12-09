#ifndef DEPENDENCIES_H
#define DEPENDENCIES_H

#include <stdio.h>
#include <stdlib.h>
#include "list_int.h"
#include "process.h"
#include "utils.h"

void init_dependencies(char* membership_file);
int get_size_dependencies();
bool is_a_dependency(int process_id);
void print_dependencies();
void free_dependencies();


#endif