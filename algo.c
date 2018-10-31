#include "algo.h"


// Send a message to a given destination over perfect links
// We have to ensure validity :
// If pi and pj are correct, then every message sent by 
// pi is eventually delivered by pj. 
// For this we will use acks, but we cannot wait until the 
// ack is received because we may receive several other
// packet. So the idea is to keep track of the message sent
// and the destination in a list. When an ack is received, 
// we remove the [msg, dst] from the list. It doesn't matter
// if we deliver or send others packet in between, we only 
// need to ensure that the packet we sent will reach the 
// destination. 
void perfect_links_send(char* msg, int dst_process_id){

	// First we send the message over UDP
	send_udp_packet(msg, dst_process_id);
	// Then we add it to the list
	add_msg_sent(msg, dst_process_id);


	/*struct timespec tstart={0,0}, tend={0,0};
	clock_gettime(CLOCK_MONOTONIC, &tstart);*/

	/*clock_gettime(CLOCK_MONOTONIC, &tend);
	printf("%.5f seconds\n", ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));*/
}

void perfect_links_deliver(char* msg, 
						   struct sockaddr_in * src_sock_addr){
	int src_id = get_id_from_sock_addr(src_sock_addr);
	// If this is not an ack, we send an ack back to the source
	if (msg[0] != 'a') {
		char ack_msg[16];
		sprintf(ack_msg, "a %s", msg);
		send_udp_packet(ack_msg, src_id);

		// If the message is not yet delivered, we deliver it
		if(not_delivered_yet(src_id, msg)) {
			add_delivered(src_id, msg);
		}
		else {
			free(msg);
			msg = NULL;
		}
	}
	// If this is an ack, we remove the waiting entry
	else {
		remove_msg_sent(msg, src_id);
		free(msg);
	}
}

void perfect_links_deliver_loss(char* msg, 
						   struct sockaddr_in * src_sock_addr){
	int src_id = get_id_from_sock_addr(src_sock_addr);
	// If this is not an ack, we send an ack back to the source
	if (msg[0] != 'a') {
		char ack_msg[16];
		sprintf(ack_msg, "a %s", msg);

		if (get_random_bool(25)) {
			printf("Ack '%s' sent\n", ack_msg);
			send_udp_packet(ack_msg, src_id);
		}
		else {
			printf("Ack '%s' lost\n", ack_msg);
		}

		// If the message is not yet delivered, we deliver it
		if(not_delivered_yet(src_id, msg)) {
			add_delivered(src_id, msg);
		}
		else {
			free(msg);
			msg = NULL;
		}
	}
	// If this is an ack, we remove the waiting entry
	else {
		remove_msg_sent(msg, src_id);
		free(msg);
	}
}

void broadcast(int* seq, int process_id, int process_count) {
	// Update logs
	add_logs(*seq, BROADCAST, process_id);
	// Broadcast the message by iterating over all other processes
	for (int i=0;i<process_count;++i) {
		if (i != process_id-1) {
			// Convert the sequence number into string
			char* msg = (char*)malloc(16);
			sprintf(msg, "%d", *seq);
			int dst_process_id = i+1;
			perfect_links_send(msg, dst_process_id);
			//send_udp_packet(msg, dst_process_id);
	    }
	}
	
	// Increment the sequence number
	++(*seq);
}
