#include "msg_sent.h"

static Msg_sent* msg_sent_first = NULL;
static Msg_sent* msg_sent_last = NULL;
double time_out = 0.2;

void add_msg_sent(char* payload, int msg_src, int dst_process_id){
	if (msg_sent_first == NULL) {
		msg_sent_first = malloc(sizeof(Msg_sent));
		if (!msg_sent_first){
			printf("Error : cannot allocate memory for msg_sent_first");
			exit(0);
		}
		msg_sent_last = msg_sent_first;
	}
	else {
		msg_sent_last->next = malloc(sizeof(Msg_sent));
		if (!msg_sent_last->next){
			printf("Error : cannot allocate memory for msg_sent_first");
			exit(0);
		}
		msg_sent_last = msg_sent_last->next;
	}
	msg_sent_last->payload = payload;
	msg_sent_last->msg_dst = dst_process_id;
	msg_sent_last->msg_src = msg_src;
	msg_sent_last->t_start = get_cur_time();
	msg_sent_last->next = NULL;
}

void remove_msg_sent(char* ack_payload, int msg_dst, int msg_src){
	struct Msg_sent* prev = NULL;
	struct Msg_sent* cur = msg_sent_first;

	bool not_found = true;
	while(not_found && cur) {
		if (strcmp(cur->payload, ack_payload) == 0 && cur->msg_dst == msg_dst && cur->msg_src == msg_src){
			not_found = false;
			if (!cur->next) {
				msg_sent_last = prev;
			}
			if (prev) {
				prev->next = cur->next;
			}
			else {
				msg_sent_first = cur->next;
			}
			prev = cur;
			cur = cur->next;
			free(prev);
			prev = NULL;
		}
		else{
			prev = cur;
			cur = cur->next;
		}
	}
}

bool msg_sent_times_up(double t_start) {
	return get_cur_time()-t_start > time_out;
}

double get_cur_time(){
	struct timespec cur_time = {0,0};
	clock_gettime(CLOCK_MONOTONIC, &cur_time);
	return (cur_time.tv_sec + 1.0e-9*cur_time.tv_nsec);

}

// If the acks we are waiting for do not arrive after the 
// specified timeout, we resend the corresponding packets, 
// assuming the packets were lost in the network. 
void resend_packets_if_needed() {
	Msg_sent* prev_msg_sent = NULL;
	Msg_sent* cur_msg_sent = msg_sent_first;
	if (cur_msg_sent) {
		while(cur_msg_sent && msg_sent_times_up(cur_msg_sent->t_start)) {
			char* new_msg = (char*)malloc(strlen(cur_msg_sent->payload)+16);
			sprintf(new_msg, "s\n%d\n%s", cur_msg_sent->msg_src, cur_msg_sent->payload);
			if (DEBUG_PRINT) {
				printf("Resend packet 's %d %s' to process %d\n", cur_msg_sent->msg_src, cur_msg_sent->payload, cur_msg_sent->msg_dst);
			}
			perfect_links_send(cur_msg_sent->payload, cur_msg_sent->msg_src, cur_msg_sent->msg_dst);
			if (prev_msg_sent){
				// If there is a message before the current message
				prev_msg_sent->next = cur_msg_sent->next;
				if (!cur_msg_sent->next) {
					msg_sent_last = prev_msg_sent; 
				}
				cur_msg_sent = cur_msg_sent->next;
			}
			else {
				// If this is the first message
				msg_sent_first = cur_msg_sent->next;
				prev_msg_sent = msg_sent_first;
				if (msg_sent_first) {
					// Si il y a au moins message
					cur_msg_sent = msg_sent_first->next;
				}
				else {
					// Si il ne reste aucun message dans la liste
					cur_msg_sent = NULL;
					msg_sent_last = NULL;
				}
			}
			//print_first_n_msg_sent(5);
		}
	}
}

void print_msg_sent(){
	printf("First : %p\n", msg_sent_first);
	Msg_sent* tmp = msg_sent_first;
	while (tmp != NULL) {
		printf("Msg : %s at %p\n", tmp->payload, tmp);
		printf("  Dst id : %d\n", tmp->msg_dst);
		printf("  Src id : %d\n", tmp->msg_src);
		printf("  Next : %p\n", tmp->next);
		tmp = tmp->next;
	}
	printf("Last : %p\n", msg_sent_last);
}

void print_first_n_msg_sent(int n){
	Msg_sent* tmp = msg_sent_first;
	printf("%p", msg_sent_first);
	for (int i=0;i<n;++i){
		if (tmp) {
			printf("{dst:%d, %s}", tmp->msg_src, tmp->payload);
			tmp = tmp->next;
		}
	}
	printf("%p", msg_sent_last);
	printf("\n");
}

void free_msg_sent(){
	Msg_sent* tmp = msg_sent_first;
	while (tmp != NULL) {
		Msg_sent* tmp_next = tmp->next;
		if (tmp->payload) {
			free(tmp->payload);
			tmp->payload = NULL;
		}
		free(tmp);
		tmp = tmp_next;
	}
	msg_sent_first = NULL;
	msg_sent_last = NULL;
}