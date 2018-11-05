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

bool not_forwarded_yet(char* msg, int src) {
	int i = 0;
	while (i < forwards_len) {
		if (forwards[i].msg != NULL) {
			if (strcmp(forwards[i].msg, msg) == 0 && forwards[i].src == src) {
				return false;
			}
		}
		++i;	
	}
	return true;
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

void check_forward_to_deliver() {
	for (int i=0;i<forwards_len;++i) {
		if (forwards[i].msg != NULL) {
			int seq = atoi(forwards[i].msg);
			if (seq == get_last_seq_delivered(forwards[i].src)+1) {
				// If it respects FIFO order
				if (acked_by_half(forwards[i].msg, forwards[i].src) && 
					not_delivered_yet(forwards[i].msg, forwards[i].src)) {
					// The [src, msg] is present in a majority of process
					// Since a majority of process is correct, the [src, msg]
					// is present at least in one correct process. 
					// By reliable broadcast, if a correct process deliver a message m
					// then every correct process deliver m
					// Since the message is present in this correct process, it means 
					// the process has broadcast it, and by beb broadcast, every message
					// sent by pi is eventually received by pj, thus all correct message 
					// will receive it. 
					if (DEBUG_PRINT) {
						printf("Delivering [%d, %s]\n", forwards[i].src, forwards[i].msg);
					}
					add_delivered(forwards[i].msg, forwards[i].src);
				}
			}
		}
	}
}

void print_forward() {
	for (int i=0;i<forwards_len;++i) {
		printf("[%d,%s]\n", forwards[i].src, forwards[i].msg);
	}
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

