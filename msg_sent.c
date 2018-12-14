#include "msg_sent.h"

static Msg_sent* msg_sent_first = NULL;
static Msg_sent* msg_sent_last = NULL;
double time_out = 0.2;

bool msg_already_in_msg_sent(int msg_src, char* payload, int msg_dst) {
	struct Msg_sent* cur = msg_sent_first;
	bool not_found = true;
	while(not_found && cur) {
		if (strcmp(cur->payload, payload) == 0 && cur->dst == msg_dst && cur->src == msg_src){
			not_found = false;
		}
		else{
			cur = cur->next;
		}
	}
	return !not_found;
}

void add_msg_sent(int msg_src, char* payload, char* vc_str, int dst){
	if (!msg_already_in_msg_sent(msg_src, payload, dst)) {
		if (msg_sent_first == NULL) {
			msg_sent_first = malloc(sizeof(Msg_sent));
			if (!msg_sent_first){
				printf("Error : cannot allocate memory for msg_sent_first");
				exit(1);
			}
			msg_sent_last = msg_sent_first;
		}
		else {
			msg_sent_last->next = malloc(sizeof(Msg_sent));
			if (!msg_sent_last->next){
				printf("Error : cannot allocate memory for msg_sent_first");
				exit(1);
			}
			msg_sent_last = msg_sent_last->next;
		}
		msg_sent_last->src = msg_src;
		char* new_payload = (char*)malloc(strlen(payload)+1);
		strcpy(new_payload, payload);
		msg_sent_last->payload = new_payload;
		msg_sent_last->vc_str = NULL;
		if (vc_str != NULL) {
			char* new_vc_str = (char*)malloc(strlen(vc_str)+1);
			strcpy(new_vc_str, vc_str);
			msg_sent_last->vc_str = new_vc_str;
		}
		msg_sent_last->dst = dst;
		msg_sent_last->t_start = get_cur_time();
		msg_sent_last->next = NULL;
	}
}

void remove_msg_sent(int msg_src, char* payload, int msg_dst) {
	struct Msg_sent* prev = NULL;
	struct Msg_sent* cur = msg_sent_first;

	bool not_found = true;
	while(not_found && cur) {
		if (strcmp(cur->payload, payload) == 0 && cur->dst == msg_dst && cur->src == msg_src){
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
			free(prev->payload);
			prev->payload = NULL;
			if (prev->vc_str){
				free(prev->vc_str);
				prev->vc_str = NULL;
			}
			free(prev);
			prev = NULL;
			if (DEBUG_PRINT) {
				printf("Removing from msg_sent : (payload:%s src:%d, dst:%d)\n", payload, msg_src, msg_dst);
			}
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
			if (DEBUG_PRINT) {
				printf("Resend packet {src:%d payload:%s vc:%s dst:%d}\n", 
					cur_msg_sent->src, cur_msg_sent->payload, cur_msg_sent->vc_str, 
					cur_msg_sent->dst);
			}
			// We save the values because we need to remove the msg_sent before 
			// calling perfect_links_send, otherwise perfect_links_send will
			// detect that the message is already in the list, and will ingore it
			int src = cur_msg_sent->src;
			char payload[strlen(cur_msg_sent->payload)];
			strcpy(payload, cur_msg_sent->payload);

			char* vc_str = NULL;
			if (cur_msg_sent->vc_str != NULL) {
				vc_str = (char*)malloc(strlen(cur_msg_sent->vc_str));
				strcpy(vc_str, cur_msg_sent->vc_str);
			}
			int dst = cur_msg_sent->dst;

			// Remove this msg sent from the linked list
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
					// If there is at least one message
					cur_msg_sent = msg_sent_first->next;
				}
				else {
					// If there is no message left in the list
					cur_msg_sent = NULL;
					msg_sent_last = NULL;
				}
			}

			// Resend it
			perfect_links_send('s', src, payload, vc_str, dst);
			if (vc_str != NULL) {
				free(vc_str);
			}
		}
	}
}

void print_msg_sent(){
	printf("First : %p\n", msg_sent_first);
	Msg_sent* tmp = msg_sent_first;
	while (tmp != NULL) {
		printf("{src:%d payload:%s vc:%s dst:%d}\n", tmp->src, tmp->payload, tmp->vc_str, tmp->dst);
		tmp = tmp->next;
	}
}

void print_first_n_msg_sent(int n){
	Msg_sent* tmp = msg_sent_first;
	printf("%p", msg_sent_first);
	for (int i=0;i<n;++i){
		if (tmp) {
			printf("{src:%d payload:%s vc:%s dst:%d}", tmp->src, tmp->payload, tmp->vc_str, tmp->dst);
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
		if (tmp->vc_str) {
			free(tmp->vc_str);
			tmp->vc_str = NULL;
		}
		free(tmp);
		tmp = tmp_next;
	}
	msg_sent_first = NULL;
	msg_sent_last = NULL;
}