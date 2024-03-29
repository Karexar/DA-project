#include "ack.h"

static Ack* acks;
static int acks_total_len = 1000;
static int acks_len = 0;
static int from_total_len = 0;

void init_ack() {
	acks = calloc(acks_total_len, sizeof(Ack));
	if (!acks) {
		printf("Error : calloc failed in init_ack\n");
		exit(1);
	}
	from_total_len = get_process_count()-1;
	for (int i=0;i<acks_total_len;++i) {
		acks[i].src = -1;
		acks[i].msg = NULL;
		create_empty_from_list(i);
	}
}

void create_empty_from_list(int index) {
	acks[index].from = (int*)malloc(from_total_len*sizeof(int));
	if (!acks[index].from) {
		printf("Error : calloc failed in create_empty_ack\n");
		exit(1);
	}
	for (int j=0;j<from_total_len;++j) {
		acks[index].from[j] = -1;
	}
}

int get_acks_len(){
	return acks_len;
}

int find_ack(char* msg, int src) {
	if (msg != NULL && src > 0) {
		int i = 0;
		while (i < acks_len) {
			if (acks[i].msg != NULL) {
				if (strcmp(acks[i].msg, msg) == 0 && acks[i].src == src) {
					return i;
				}
			}
			++i;
		}
	}
	return -1;
}

void add_ack(char* msg, int src, int from) {
	// First we try to find the corresponding [msg, src]
	int index = find_ack(msg, src);
	if (index == -1) {
		// Not found, we fill a new Ack
		if (acks_len >= acks_total_len) {
			acks_total_len*=2;
			acks = realloc(acks, acks_total_len*sizeof(Ack));
			if (!acks) {
				printf("Error : realloc failed in add_ack\n");
				exit(1);
			}
			for (int i=acks_len;i<acks_total_len;++i) {
				create_empty_from_list(i);
			}

		}
		acks[acks_len].src = src;
		acks[acks_len].msg = (char*)malloc(strlen(msg)+1);
		if (!acks[acks_len].msg) {
			printf("Error : malloc failed in add_ack\n");
			exit(1);
		}
		strcpy(acks[acks_len].msg, msg);
		acks[acks_len].from[0] = from;
		++acks_len;
	}
	else {
		// An Ack already exists with this [msg, src]
		// Check if the process is already in the ack for this message
		bool process_not_in_ack_yet = true;
		for (int i=0;i<from_total_len;++i) {
			if (acks[index].from[i] == from) {
				process_not_in_ack_yet = false;
			}
		}

		if (process_not_in_ack_yet) {
			bool not_added = true;
			int i = 0;
			while(not_added && i < from_total_len) {
				if (acks[index].from[i] == -1) {
					acks[index].from[i] = from;
					not_added = false;
				}
				++i;
			}
			if (not_added) {
				printf("Error : cannot add 'from' to Ack\n");
				exit(1);
			}
		}
	}
}

bool acked_by_half(char* msg, int src) {
	int index = find_ack(msg, src);
	if (index >= 0) {
		int count = 0;
		for(int i=0;i<from_total_len;++i) {
			if (acks[index].from[i] != -1) {
				++count;
			}
		}
		//printf("count %d/%d\n", count+1, get_process_count());
		// +1 because this process has also the message
		if(count+1 > get_process_count() / 2.0) {
			return true;
		}
	}
	return false;
}


void print_ack() {
	for(int i=0;i<acks_len;++i) {
		printf("[%d,%s,[", acks[i].src, acks[i].msg);
		for(int j=0;j<from_total_len;++j) {
			printf("%d,", acks[i].from[j]);
		}
		printf("]]");
	} 
	if (acks_len > 0) {
		printf("\n");
	}
}

void free_ack() {
	for (int i=0;i<acks_len;++i) {
		if(acks[i].msg!=NULL) {
			free(acks[i].msg);
			acks[i].msg = NULL;
		}
		if(acks[i].from!=NULL) {
			free(acks[i].from);
			acks[i].from = NULL;
		}
	}
	free(acks);
	acks=NULL;
}