#ifndef DEPENDENCIES_H
#define DEPENDENCIES_H

#include <stdio.h>
#include <stdlib.h>
#include "list_int.h"
#include "process.h"

void init_dependencies(char* membership_file);
void print_dependencies();
void free_dependencies();


#endif