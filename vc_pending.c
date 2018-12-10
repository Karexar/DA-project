#include "vc_pending.h"

static VC_pending* vc_pending;
static int size = 0;
static int allocated = 1000;

void init_VC_pending() {
	vc_pending = calloc(allocated, sizeof(VC_pending));
	if (!vc_pending) {
		printf("Error : calloc failed in init_VC_pending\n");
		exit(1);
	}
}

void add_VC_pending(int src, char* payload, Vector_clock_elem* vc, int vc_size, int from_id) {
	if (size >= allocated) {
		allocated*=2;
		vc_pending = realloc(vc_pending, allocated*sizeof(VC_pending));
		if (!vc_pending) {
			printf("Error : realloc failed in add_VC_pending\n");
			exit(1);
		}
	}
	vc_pending[size].src = src;
	vc_pending[size].payload = payload;
	vc_pending[size].vc = vc;
	vc_pending[size].vc_size = vc_size;
	vc_pending[size].from_id = from_id;
	++size;
}

void check_pending_vc_to_urb_deliver() {
	// Iterate over all vc pending msg and check the vc
	for (int i=0;i<size;++i) {
		if (vc_pending[i].payload != NULL) {
			if (compare_vector_clock(vc_pending[i].vc, vc_pending[i].vc_size)) {
				// current vc is higher, we can remove the message from vc_pending 
				// and urb_deliver it.
				urb_deliver(vc_pending[i].payload, 's', vc_pending[i].src, vc_pending[i].from_id);
				vc_pending[i].src = -1;
				vc_pending[i].payload = NULL;
				vc_pending[i].vc = NULL;
				vc_pending[i].vc_size = -1;
				vc_pending[i].from_id = -1;
			}
		}
	}
}

void print_VC_pending() {
	for (int i=0;i<size;++i) {
		printf("[%d,%s,[", vc_pending[i].src, vc_pending[i].payload);
		for (int j=0;j<vc_pending[i].vc_size;++j) {
			printf("(%d,%d)", vc_pending[i].vc[j].process_id, vc_pending[i].vc[j].msg_count);
		}
		printf("]]\n");
	}
}

void free_VC_pending() {
	for (int i=0;i<size;++i) {
		free(vc_pending[i].vc);
		vc_pending[i].vc = NULL;
		free(vc_pending[i].payload);
		vc_pending[i].payload = NULL;
	}
	free(vc_pending);
	vc_pending = NULL;
}