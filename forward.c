#include "forward.h"

static Forward* forwards;
static int forwards_total_len = 1000;
static int forwards_len = 0;

void init_forward(){
	forwards = calloc(forwards_total_len, sizeof(Forward));
	if (!forwards) {
		printf("Error : calloc failed in init_forward\n");
		exit(1);
	}
}

bool not_forwarded_yet(char* payload, int src) {
	int i = 0;
	while (i < forwards_len) {
		if (forwards[i].payload != NULL) {
			if (strcmp(forwards[i].payload, payload) == 0 && forwards[i].src == src) {
				return false;
			}
		}
		++i;	
	}
	return true;
}

void add_forward(char* payload, int src, char* vc_str){
	char* new_payload = (char*)malloc(strlen(payload)+1);
	strcpy(new_payload, payload);
	char* new_vc_str = NULL;
	if (vc_str != NULL) {
		new_vc_str = (char*)malloc(strlen(vc_str)+1);
		strcpy(new_vc_str, vc_str);
	}
	if (forwards_len >= forwards_total_len) {
		forwards_total_len*=2;
		forwards = realloc(forwards, forwards_total_len*sizeof(Forward));
		if (!forwards) {
			printf("Error : realloc failed in add_forward\n");
			exit(1);
		}

	}
	forwards[forwards_len].src = src;
	forwards[forwards_len].payload = new_payload;
	forwards[forwards_len].vc_str = new_vc_str;
	++forwards_len;
}

void check_forward_to_deliver() {
	for (int i=0;i<forwards_len;++i) {
		if (forwards[i].payload != NULL) {
			int seq = atoi(forwards[i].payload);
			if (seq == get_last_seq_delivered(forwards[i].src)+1) {
				// If it respects FIFO order
				if (acked_by_half(forwards[i].payload, forwards[i].src) && 
					not_delivered_yet(forwards[i].payload, forwards[i].src)) {
					// The [src, payload] is present in a majority of process
					// Since a majority of process is correct, the [src, payload]
					// is present at least in one correct process. 
					// By reliable broadcast, if a correct process deliver a message m
					// then every correct process deliver m
					// Since the message is present in this correct process, it means 
					// the process has broadcast it, and by beb broadcast, every message
					// sent by pi is eventually received by pj, thus all correct message 
					// will receive it. 
					if (DEBUG_PRINT) {
						printf("Delivering [%d, %s]\n", forwards[i].src, forwards[i].payload);
					}
					add_delivered(forwards[i].payload, forwards[i].src);
				}
			}
		}
	}
}

void print_forward() {
	for (int i=0;i<forwards_len;++i) {
		printf("[%d,%s,%s]\n", forwards[i].src, forwards[i].payload, forwards[i].vc_str);
	}
}

void free_forward(){
	for (int i=0;i<forwards_len;++i) {
		if (forwards[i].payload != NULL) {
			free(forwards[i].payload);
			forwards[i].payload = NULL;
		}
		if (forwards[i].vc_str != NULL) {
			free(forwards[i].vc_str);
			forwards[i].vc_str = NULL;
		}
	}
	free(forwards);
	forwards = NULL;
}

