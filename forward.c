#include "forward.h"

static Forward* forwards;
static int forwards_total_len = 1000;
static int forwards_len = 0;

void init_forward(){
	forwards = calloc(forwards_total_len, sizeof(Forward));
	if (!forwards) {
		printf("Error : calloc failed in init_forward\n");
		exit(0);
	}
}

void add_forward(char* msg, int src){
	if (forwards_len >= forwards_total_len) {
		forwards_total_len*=2;
		forwards = realloc(forwards, forwards_total_len*sizeof(Forward));
		if (!forwards) {
			printf("Error : realloc failed in add_forward\n");
			exit(0);
		}

	}
	forwards[forwards_len].src = src;
	forwards[forwards_len].msg = msg;
	++forwards_len;
}

void free_forward(){
	for (int i=0;i<forwards_len;++i) {
		if (forwards[i].msg != NULL) {
			free(forwards[i].msg);
			forwards[i].msg = NULL;
		}
	}
	free(forwards);
	forwards = NULL;
}

