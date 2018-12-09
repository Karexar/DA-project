#include "dependencies.h"

static int* dependencies;
static int size_dependencies;

void init_dependencies(char* membership_file) {
    FILE *f = fopen(membership_file, "r");
    if(f == NULL) {
        printf("Cannot open '%s'\n", membership_file);
        exit(0);
    }
    else {
        // Skip the lines corresponding to the processes specifications
        char c = '\0';
        int process_count = get_process_count();
        for (int i=0;i<=process_count;++i) {
            do {
                c = fgetc(f);
            }
            while (c != '\n' && c != EOF);
        }
        // We look for the line corresponding to the dependencies of the
        // current process
        int process_id = get_process_id();
        int i = 0;
        bool found = false;
        while (i < process_count && !found) {
            // get next word, corresponding to a process id
            bool end_of_line = false;
            bool end_of_file = false;
            char* word = get_word(f, &end_of_line, &end_of_file);
            int id_read = atoi(word);
            if (id_read == process_id) {
                found = true;
                // We found the line for this process, now we read the
                // dependencies
                List_int* list_dep = create_list_int(5);
                while (!end_of_line && !end_of_file) {
                    word = get_word(f, &end_of_line, &end_of_file);
                    int dependency = atoi(word);
                    add_int(list_dep, dependency);
                }

                size_dependencies = list_dep->size;
                dependencies = (int*)malloc(size_dependencies*sizeof(int));
                for (int j=0;j<size_dependencies;++j) {
                    dependencies[j] = list_dep->list[j];
                }
                free_list_int(list_dep);
            }
            else if (!end_of_line && !end_of_file) {
                c = '\0';
                do {
                    c = fgetc(f);
                }
                while (c != '\n' && c != EOF);
            }
            ++i;
        }

        fclose(f);
    }
}

/*int get_size_dependencies() {
    return size_dependencies;
}

int* get_dependencies() {
    return dependencies;
}*/

void print_dependencies() {
    printf("Dependencies :\n");
    for (int i=0;i < size_dependencies;++i) {
        printf("%d\n", dependencies[i]);
    }
}

void free_dependencies() {
    if (dependencies != NULL) {
        free(dependencies);
        dependencies = NULL;
    }
}
