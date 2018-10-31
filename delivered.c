#include "delivered.h"

static Delivered *delivered;
static int delivered_total_len = 0;
static int delivered_len = 0;

void init_delivered(int total_broadcast) {
	// TODO : increase size if needed (we are not sure every process broadcast the same amount)
	delivered_total_len = total_broadcast*(get_process_count()-1);
	delivered = calloc(delivered_total_len, sizeof(Delivered));

}

void add_delivered(int src_process_id, char* msg) {
	delivered[delivered_len].src_id = src_process_id;
	delivered[delivered_len].msg = msg;
	++delivered_len;
}

bool not_delivered_yet(int src_id, char* msg) {
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
	}
	free(delivered);
}

