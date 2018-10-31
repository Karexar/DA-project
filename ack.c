#include "ack.h"

static Msg_sent* msg_sent_first = NULL;
static Msg_sent* msg_sent_last = NULL;
double time_out = 1.0; // TODO : adjust

void add_msg_sent(char* msg, int dst_process_id){
	if (msg_sent_first == NULL) {
		msg_sent_first = malloc(sizeof(Msg_sent));
		msg_sent_last = msg_sent_first;
	}
	else {
		msg_sent_last->next = malloc(sizeof(Msg_sent));
		msg_sent_last = msg_sent_last->next;
	}
	msg_sent_last->msg = msg;
	msg_sent_last->dst_id = dst_process_id; //get_sock_addr_from_process_id(dst_process_id);
	msg_sent_last->t_start = get_cur_time();
	msg_sent_last->next = NULL;
}

void remove_msg_sent(char* ack_msg, int process_id){
	char ack_msg_cut[16];
	memcpy(ack_msg_cut, ack_msg+2, strlen(ack_msg)-1);

	struct Msg_sent* prev = NULL;
	struct Msg_sent* cur = msg_sent_first;

	bool not_found = true;
	while(not_found && cur) {
		if (strcmp(cur->msg, ack_msg_cut) == 0 && cur->dst_id == process_id){
			not_found = false;
			if (prev) {
				prev->next = cur->next;
			}
			else {
				msg_sent_first = cur->next;
			}
			prev = cur;
			cur = cur->next;
			free(prev);
		}
		else{
			prev = cur;
			cur = cur->next;
		}
	}
}

bool ack_times_up(double t_start) {
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
		while(cur_msg_sent && ack_times_up(cur_msg_sent->t_start)) {
			printf("Resend packet '%s' to process %d\n", cur_msg_sent->msg, cur_msg_sent->dst_id);
			perfect_links_send(cur_msg_sent->msg, cur_msg_sent->dst_id);
			if (prev_msg_sent){
				prev_msg_sent->next = cur_msg_sent->next;
				if (!cur_msg_sent->next) {
					msg_sent_last = prev_msg_sent; 
				}
				cur_msg_sent = cur_msg_sent->next;
			}
			else {
				msg_sent_first = cur_msg_sent->next;
				prev_msg_sent = msg_sent_first;
				if (msg_sent_first) {
					cur_msg_sent = msg_sent_first->next;
				}
				else {
					cur_msg_sent = NULL;
				}
			}
		}
	}
}

void print_msg_sent(){
	printf("First : %p\n", msg_sent_first);
	Msg_sent* tmp = msg_sent_first;
	while (tmp != NULL) {
		printf("Msg : %s at %p\n", tmp->msg, tmp);
		printf("  Process id : %d\n", tmp->dst_id);
		printf("  Next : %p\n", tmp->next);
		tmp = tmp->next;
	}
	printf("Last : %p\n", msg_sent_last);
}

void free_msg_sent(){
	Msg_sent* tmp = msg_sent_first;
	while (tmp != NULL) {
		Msg_sent* tmp_next = tmp->next;
		free(tmp->msg);
		free(tmp);
		tmp = tmp_next;
	}
	msg_sent_first = NULL;
	msg_sent_last = NULL;
}