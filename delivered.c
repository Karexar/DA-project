#include "delivered.h"

static Delivered *delivered;
static int delivered_total_len = 0;
static int delivered_len = 0;
static int* last_seq_delivered;

void init_delivered() {
	delivered_total_len = 5000;
	delivered = calloc(delivered_total_len, sizeof(Delivered));

	last_seq_delivered = (int*)malloc(get_process_count()*sizeof(int));
	for(int i=0;i<get_process_count();++i) {
		last_seq_delivered[i] = 0;
	}
}

int get_last_seq_delivered(int process_id) {
	return last_seq_delivered[get_index_from_id(process_id)];
}

void add_delivered(char* msg, int src_process_id) {
	if (delivered_len >= delivered_total_len) {
		delivered_total_len*=2;
		delivered = realloc(delivered, delivered_total_len*sizeof(Delivered));
		printf("realloc\n");
		if (!delivered) {
			printf("Error : realloc failed in add_delivered\n");
			exit(0);
		}
	}

	// Update logs
	add_logs(src_process_id, msg, DELIVER);
	delivered[delivered_len].src_id = src_process_id;
	delivered[delivered_len].msg = msg;
	++delivered_len;
	int seq = atoi(msg);
	if (seq <= last_seq_delivered[get_index_from_id(src_process_id)]) {
		printf("Error : Delivered message sequence does not respect FIFO\n");
		exit(0);
	}
	last_seq_delivered[get_index_from_id(src_process_id)] = seq;
}

bool not_delivered_yet(char* msg, int src_id) {
	for (int i=0;i<delivered_len;++i) {
		if (delivered[i].src_id == src_id && strcmp(delivered[i].msg, msg) == 0) {
			return false;
		}
	}
	return true;
}

void print_delivered() {
	for (int i=0;i<delivered_len;++i) {
		printf("src : %d\n", delivered[i].src_id);
		printf("msg : %s\n", delivered[i].msg);
	}
}

void free_delivered(){
	for (int i=0;i<delivered_len;++i) {
		free(delivered[i].msg);
		delivered[i].msg = NULL;
	}
	free(delivered);
	delivered = NULL;
	free(last_seq_delivered);
	last_seq_delivered = NULL;
}

