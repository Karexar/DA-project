#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include "utils.h"
#include "udp.h"
#include "algo.h"
#include "msg_sent.h"
#include "process.h"
#include "log.h"
#include "delivered.h"
#include "random.h"
#include "timer.h"
#include "forward.h"
#include "ack.h"

// udp
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>

// non blocking socket
#include <sys/select.h> 
#include <fcntl.h>

static int wait_for_start = 1;

static int process_id;

static void start(int signum) {
	wait_for_start = 0;
}

static void stop(int signum) {
	//reset signal handlers to default
	signal(SIGTERM, SIG_DFL);
	signal(SIGINT, SIG_DFL);

	//immediately stop network packet processing
	printf("Immediately stopping network packet processing.\n");

	//write/flush output file if necessary
	printf("Writing output.\n");
	write_logs(process_id);

	//exit directly from signal handler
	exit(0);
}


int main(int argc, char** argv) {
	// Get the main parameters
	process_id = atoi(get_main_arg(argc, argv, 1, INTEGER));
	char *filename = get_main_arg(argc, argv, 2, STRING);
	int total_broadcast = atoi(get_main_arg(argc, argv, 3, INTEGER));

	printf("Initializing process %d.\n", process_id);

	char tmp_type = NULL;
	int tmp_src = -1;
	char* tmp_msg = (char*)malloc(50);
	sprintf(tmp_msg, "s\n23\nSalut");

	parse_message(tmp_msg, &tmp_type, &tmp_src);
	printf("type : %c\n", tmp_type); 
	printf("src : %d\n", tmp_src);
	printf("payload : %s\n", tmp_msg);

	free(tmp_msg);

	// Set signal handlers
	signal(SIGUSR1, start);
	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	// Read the membership file to extract 
	// the id, ip and port for each process
	init_process(filename, process_id);

	// Initialize sockets
	init_socket(process_id);

	// Initialize log by creating an empty file. 
	// Since we can write to a file before it crashes, we store 
	// the information in a variable to avoid too much file access
	// and write it to disk only when the variable is full
	init_logs(process_id);

	// Initialize everything we need concerning the distributed algorithms
	init_urb();

	// Initialize non blocking receiver
	fd_set readfds;
	fcntl(get_sock_fd(), F_SETFL, O_NONBLOCK);

	// Initialize random number generator (debug only)
	init_random();
	/*
	// Wait until start signal
	// Start listening for incoming UDP packets
	while(wait_for_start) {
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);

		struct timespec sleep_time;
		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = 1000;

		int rv = select(sockfd+1, &readfds, NULL, NULL, (struct timeval*) &sleep_time);
		if (rv == 1) {
			char* msg = receive_udp_packet(sockfd, &src_sock_ip);
			printf("Received : '%s' from ip %s and port %d\n", msg, 
					inet_ntoa(src_sock_ip.sin_addr), ntohs(src_sock_ip.sin_port));
			free(msg);
		}
		// TODO : shall we do something for incoming packet before start signal ? 
		//nanosleep(&sleep_time, NULL);
	}*/

	//perfect_links_send("salut", 2);
	//print_msg_sent();

	sleep(1);

	//broadcast messages
	printf("Broadcasting %d messages.\n", total_broadcast);
	
	int seq = 0;
	for (int i = 0 ; i < total_broadcast ; ++i) {
		printf("Sending 's %d %d' to process %d\n", process_id, seq, i);
		// Convert the sequence number into string
		char* payload = (char*)malloc(16);
		sprintf(payload, "%d", seq);
		++seq;
		broadcast(payload, process_id);
	}




/*
	struct sockaddr_in src_sock_ip;
	char* msg = receive_udp_packet(&src_sock_ip);
	printf("Received : '%s' from ip %s and port %d\n", msg, 
			inet_ntoa(src_sock_ip.sin_addr), ntohs(src_sock_ip.sin_port));
	perfect_links_deliver(msg, &src_sock_ip);
	print_msg_sent();

	msg = receive_udp_packet(&src_sock_ip);
	printf("Received : '%s' from ip %s and port %d\n", msg, 
			inet_ntoa(src_sock_ip.sin_addr), ntohs(src_sock_ip.sin_port));
	perfect_links_deliver(msg, &src_sock_ip);
	print_msg_sent();

	msg = receive_udp_packet(&src_sock_ip);
	printf("Received : '%s' from ip %s and port %d\n", msg, 
			inet_ntoa(src_sock_ip.sin_addr), ntohs(src_sock_ip.sin_port));
	perfect_links_deliver(msg, &src_sock_ip);
	print_msg_sent();

	msg = receive_udp_packet(&src_sock_ip);
	printf("Received : '%s' from ip %s and port %d\n", msg, 
			inet_ntoa(src_sock_ip.sin_addr), ntohs(src_sock_ip.sin_port));
	perfect_links_deliver(msg, &src_sock_ip);
	print_msg_sent();
*/
	//print_msg_sent();
	//wait until stopped

	bool printed = false;
	set_timeout(10.0);

	while(1) {
		FD_ZERO(&readfds);
		FD_SET(get_sock_fd(), &readfds);

		struct timespec sleep_time;
		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = 1000;

		if (times_up() && !printed) {
			print_msg_sent();
			printed = true;
		}

		int rv = select(get_sock_fd()+1, &readfds, NULL, NULL, (struct timeval*) &sleep_time);
		if (rv == 1) {
			struct sockaddr_in src_sock_ip;
			char* msg = receive_udp_packet(&src_sock_ip);
			char msg_type = NULL;
			int msg_src = -1;
			parse_message(msg, &msg_type, &msg_src);

			printf("Received : '%c %d %s' from process %d\n", msg_type, msg_src, msg, 
					get_id_from_port(ntohs(src_sock_ip.sin_port)));
			//perfect_links_deliver(msg, msg_type, msg_src, &src_sock_ip);
			urb_deliver(msg, msg_type, msg_src, &src_sock_ip);
		}

		// resend packet after a timeout in msg_sent if no ack received 
		resend_packets_if_needed();

		// to ensure urb
		check_forward_to_deliver();

		nanosleep(&sleep_time, NULL);

		//struct timespec sleep_time;
		//sleep_time.tv_sec = 1;
		//sleep_time.tv_nsec = 0;
		//nanosleep(&sleep_time, NULL);
	}

	// Free 
	free_socket();
	free_delivered();
	free_msg_sent();
	free_process_list();
}
