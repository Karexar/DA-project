#include "algo.h"

// Initialize everything we need for the best effort broadcast
void init_beb() {
	init_delivered();
}

// Initialize everything we need for the uniform reliable
// broadcast algorithm
void init_urb() {
	init_beb();
	init_forward();
	init_ack();
}

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
void perfect_links_send(char* payload, int msg_src, int dst_process_id){
	// Message has the form
	// Type : s for simple packet, a for ack packet
	// Source process : the very origin of the message
	// Payload : the sequence number
	char msg[strlen(payload)+16];
	sprintf(msg, "s\n%d\n%s", msg_src, payload);
	// First we send the packet over UDP
	send_udp_packet(msg, dst_process_id);
	// Then we add it to the list
	add_msg_sent(payload, msg_src, dst_process_id);


	/*struct timespec tstart={0,0}, tend={0,0};
	clock_gettime(CLOCK_MONOTONIC, &tstart);*/

	/*clock_gettime(CLOCK_MONOTONIC, &tend);
	printf("%.5f seconds\n", ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));*/
}

void perfect_links_deliver(char* msg, char msg_type, int msg_src, 
						   struct sockaddr_in * src_sock_addr){
	int from_id = get_id_from_sock_addr(src_sock_addr);
	// If this is not an ack, we send an ack back to the source
	if (msg_type == 's') {
		if (DEBUG_PRINT) {
			printf("Sending ack 'a %d %s' to process %d\n", msg_src, msg, from_id);
		}
		char ack_msg[strlen(msg)+16];
		sprintf(ack_msg, "a\n%d\n%s", msg_src, msg);
		send_udp_packet(ack_msg, from_id);

		// If the message is not yet delivered, we deliver it
		if(not_delivered_yet(msg, from_id)) {
			add_delivered(msg, from_id);
		}
		else {
			free(msg);
			msg = NULL;
		}
	}
	// If this is an ack, we remove the waiting entry
	else if (msg_type == 'a') {
		remove_msg_sent(msg, from_id, msg_src);
		free(msg);
		msg = NULL;
	}
	else {
		printf("Error : unknown message type\n");
		exit(0);
	}
}

void broadcast(char* payload, int msg_src) {
	// Update logs
	add_logs(msg_src, payload, BROADCAST);
	// Broadcast the message by iterating over all other processes
	for (int i=0;i<get_process_count();++i) {
		if (i != get_process_id()-1) {
			int msg_dst = i+1;
			perfect_links_send(payload, msg_src, msg_dst);
	    }
	}
}


void urb_deliver(char* msg, char msg_type, int msg_src, 
						   struct sockaddr_in * src_sock_addr){
	int from_id = get_id_from_sock_addr(src_sock_addr);
	// If this is not an ack, we send an ack back to the source
	if (msg_type == 's') {
		if (DEBUG_PRINT) {
			printf("Sending ack 'a %d %s' to process %d\n", msg_src, msg, from_id);
		}
		char ack_msg[strlen(msg)+16];
		sprintf(ack_msg, "a\n%d\n%s", msg_src, msg);
		send_udp_packet(ack_msg, from_id);

		// First we add the sender to the ack list for this message
		// because if it sends it, it means he has this message. 
		add_ack(msg, msg_src, from_id);

		// Then we check if the message is not already in the forward table
		// and if the source of the message is not this process (self)
		if (not_forwarded_yet(msg, msg_src) && msg_src != get_process_id()) {
			// If it's not forwarded, we add the message to the forward
			// list and broadcast the message to every processes
			add_forward(msg, msg_src);
			if (DEBUG_PRINT) {
				printf("Broadcast [%d, %s]\n", msg_src, msg);
			}
			broadcast(msg, msg_src);
		}
		else {
			// The message is already in the forward list
			// There is nothing to do left with this message
			free(msg);
			msg = NULL;
		}
	}
	// If this is an ack, we remove the waiting entry
	else if (msg_type == 'a') {
		remove_msg_sent(msg, from_id, msg_src);
		free(msg);
		msg = NULL;
	}
	else {
		printf("Error : unknown message type\n");
		exit(0);
	}
}

