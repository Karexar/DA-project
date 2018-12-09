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
#include "setting.h"
#include "clock.h"
#include "dependencies.h"
#include "vector_clock.h"

// udp
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>


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
	printf("Writing output for process %d.\n", process_id);
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

	// Set signal handlers
	signal(SIGUSR2, start);
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
	init_causal(filename);

	// Initialize random number generator (debug only)
	init_random();
	
	// Wait until start signal
	// Start listening for incoming UDP packets
	while(wait_for_start) {
		FIFO_listen();
	}

	//broadcast messages
	printf("Broadcasting %d messages.\n", total_broadcast);
	
	int seq = 1;
	for (int i = 0 ; i < total_broadcast ; ++i) {
		// Convert the sequence number into string
		char* payload = (char*)malloc(16);
		sprintf(payload, "%d", seq);
		++seq;
		broadcast(payload, process_id, true);
	}
	


	//wait until stopped
	bool printed = false;
	set_timeout(10.0);
	while(1) {
		FIFO_listen();

		if (DEBUG_PRINT) {
			if (times_up() && !printed) {
				print_msg_sent();
				printf("Ack\n");
				print_ack();
				printf("Forward\n");
				print_forward();
				printf("Delivered\n");
				print_delivered();
				printed = true;
			}
		}
	}

	// Free 
	free_socket();
	free_delivered();
	free_msg_sent();
	free_process_list();
	free_forward();
	free_ack();
	free_dependencies();
	free_vector_clock();
}
