#include "algo.h"

static fd_set readfds;

// Initialize everything we need for the best effort broadcast
void init_beb() {
	init_delivered();
	// Initialize non blocking receiver
	fcntl(get_sock_fd(), F_SETFL, O_NONBLOCK);
}

// Initialize everything we need for the uniform reliable
// broadcast algorithm
void init_urb() {
	init_beb();
	init_forward();
	init_ack();
}

void init_causal(char* membership_file) {
	init_dependencies(membership_file);
	init_vector_clock();
	init_urb();
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
void perfect_links_send(char msg_type, int msg_src, char* payload, char* vc_str, int dst){
	// Message has the form (all are separated by \n)
	// Type : 's' for simple packet, 'a' for ack packet
	// Source process : the very origin of the message
	// Payload : the sequence number
	// Vector clock : the message count for each dependency
	int len = 13+strlen(payload)+1;
	if (vc_str != NULL) {
		len += strlen(vc_str);
	}
	char msg[len];
	sprintf(msg, "s\n%d\n%s", msg_src, payload);
	if (vc_str != NULL && vc_str[0] != '\0') {
		sprintf(msg, "%s\n%s", msg, vc_str);
	}
	// First we send the packet over UDP
	if (DEBUG_PRINT) {
		printf("P%d send : {type:%c src:%d payload%s vc:%s}\n", get_process_id(), msg_type, msg_src, payload, vc_str);
	}
	send_udp_packet(msg, dst);
	// Then we add it to the msg_sent list
	add_msg_sent(msg_src, payload, vc_str, dst);
}

/*void perfect_links_deliver(char* msg, char msg_type, int msg_src, 
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
		remove_msg_sent(msg, from_id);
		free(msg);
		msg = NULL;
	}
	else {
		printf("Error : unknown message type\n");
		exit(1);
	}
}*/

void broadcast(char* payload, int msg_src, char* vc, bool log_it) {
	// Update logs
	if (log_it) {
		add_logs(msg_src, payload, BROADCAST);
	}

	// Broadcast the message by iterating over all other processes
	for (int i=0;i<get_process_count();++i) {
		int dst_id = get_id_from_index(i);
		if (dst_id != get_process_id()) {
			perfect_links_send('s', msg_src, payload, vc, dst_id);
	    }
	}
}


void urb_deliver(int msg_src, char* payload, char* vc_str){
	// check if the message is not already in the forward table
	// and if the source of the message is not this process (self)
	if (not_forwarded_yet(payload, msg_src) && msg_src != get_process_id()) {
		// If it's not forwarded, we add the message to the forward
		// list and broadcast the message to every processes
		add_forward(payload, msg_src, vc_str);
		if (DEBUG_PRINT) {
			printf("Retransmit to all: {src:%d, payload:%s, vc:%s}\n", msg_src, payload, vc_str);
		}
		broadcast(payload, msg_src, vc_str, false);
	}
	else {
		// The message is already in the forward list
		// There is nothing to do left with this messaged
		free(payload);
		payload = NULL;
		if (vc_str) {
			free(vc_str);
			vc_str=NULL;
		}
	}
}

/*void FIFO_listen() {
	FD_ZERO(&readfds);
	FD_SET(get_sock_fd(), &readfds);

	struct timespec sleep_time;
	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = 1000;

	int rv = select(get_sock_fd()+1, &readfds, NULL, NULL, (struct timeval*) &sleep_time);
	if (rv == 1) {
		struct sockaddr_in src_sock_ip;
		char* msg = receive_udp_packet(&src_sock_ip);
		char msg_type = '\0';
		int msg_src = -1;
		parse_message(msg, &msg_type, &msg_src);

		if (DEBUG_PRINT){
			printf("Received : '%c %d %s' from process %d\n", msg_type, msg_src, msg, 
				get_id_from_port(ntohs(src_sock_ip.sin_port)));
		}
		//perfect_links_deliver(msg, msg_type, msg_src, &src_sock_ip);
		int from_id = get_id_from_sock_addr(&src_sock_ip);
		urb_deliver(msg, msg_type, msg_src, from_id);
	}

	// resend packet after a timeout in msg_sent if no ack received 
	resend_packets_if_needed();

	// to ensure urb
	check_forward_to_deliver();
}*/

//===============  Causal broadcast  =================

void causal_broadcast(char* payload) {
	// Update logs
	add_logs(get_process_id(), payload, BROADCAST);

	// Broadcast the message by iterating over all other processes
	for (int i=0;i<get_process_count();++i) {
		int dst_id = get_id_from_index(i);
		if (dst_id != get_process_id()) {
			// Message has the form
			// Type : s for simple packet, a for ack packet
			// Source process : the very origin of the message
			// Payload : the sequence number
			// VC : the vector clock entries of the source dependencies 
			perfect_links_send('s', get_process_id(), payload, get_VC_of_dependencies(), dst_id);
	    }
	}

	// Increase corresponding entry
	increment_vc(get_process_id());
}

void causal_listen() {
	FD_ZERO(&readfds);
	FD_SET(get_sock_fd(), &readfds);

	struct timespec sleep_time;
	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = 1000;

	int rv = select(get_sock_fd()+1, &readfds, NULL, NULL, (struct timeval*) &sleep_time);
	if (rv == 1) {
		// Get the message content
		struct sockaddr_in src_sock_ip;
		char* msg = receive_udp_packet(&src_sock_ip);

		// Parse the message string
		char msg_type = '\0';
		int msg_src = -1;
		char* payload = NULL;
		char* vc_str = NULL;
		parse_message(msg, &msg_type, &msg_src, &payload, &vc_str);
		free(msg);
		msg = NULL;

		int from_id = get_id_from_sock_addr(&src_sock_ip);

		if (DEBUG_PRINT){
			printf("Receive type:%c payload:%s from: %d, src:%d, vc:%s\n", msg_type, payload, from_id, msg_src, vc_str);
		} 

		// First we have to manage the acks
		if (msg_type == 'a') {
			// If the message is an ack, we remove the waiting entry in msg_sent
			remove_msg_sent(msg_src, payload, from_id);
			// There is nothing less to do, we can free the payload
			free(payload);
			payload = NULL;
		}
		else if (msg_type == 's') {
			// If the message is not an ack, we have to ack it
			if (DEBUG_PRINT) {
				printf("Sending ack {src:%d payload:%s from:%d}\n", msg_src, payload, from_id);
			}
	
			int len = 13+strlen(payload)+1;
			if (vc_str) {
				len += strlen(vc_str);
			}
			char ack_msg[len];
			sprintf(ack_msg, "a\n%d\n%s", msg_src, payload);
			send_udp_packet(ack_msg, from_id);
			// Add the sender to the ack list for this message
			// because if it sends it, it means he has this message. 
			add_ack(payload, msg_src, from_id);
			// Now we have to verify the vc to deliver the message or 
			// to add it to the vc_pending. 
			Vector_clock_elem* vc = NULL;
			int vc_size = 0;
			parse_vc(vc_str, &vc, &vc_size);
			if (compare_vector_clock(vc, vc_size)) {
				// The local vc is greater or equal to the received vc
				// for all entries, we can urb_deliver
				urb_deliver(msg_src, payload, vc_str);
			}
			else {
				// The local vc is lower than the received vc for at 
				// least one entry, we add the message to the vc_pending
				// list. We will urb_deliver it later.
				add_VC_pending(msg_src, payload, vc, vc_size, vc_str, from_id);
			}

		}
		else {
			printf("Error : unknown message type\n");
			exit(1);
		}
	}
	// resend packet after a timeout in msg_sent if no ack received 
	resend_packets_if_needed();
	// to ensure urb
	check_forward_to_deliver();
	// Check the pending msg and urb_deliver those whose VC is now equal 
	// or less than the local VC
	check_pending_vc_to_urb_deliver();
}

