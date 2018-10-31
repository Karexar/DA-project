#include "socket.h"

static int sockfd; 
static struct sockaddr_in* sock_ips;

void init_socket(int process_id) {
	//Process **p = get_process_list();
	sock_ips = calloc(5, sizeof(struct sockaddr_in));
	// Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 

    for (int i=0;i<get_process_count();++i) {
    	memset(&(sock_ips[i]), 0, sizeof(sock_ips[i])); 
    	sock_ips[i].sin_family = AF_INET; // IPv4 
    	int process_id = i+1;
    	sock_ips[i].sin_addr.s_addr = inet_addr(get_ip_from_id(process_id)); 
    	sock_ips[i].sin_port = htons(get_port_from_id(process_id)); 
    }

    // Bind the socket with the process address 
    if (bind(get_sock_fd(), (const struct sockaddr *)&(sock_ips[process_id-1]),  
            sizeof(sock_ips[process_id-1])) < 0 ) 
    { 
        perror("bind failed\n"); 
        exit(EXIT_FAILURE); 
    } 
}

void print_socket(int index) {
	printf("%s\n", inet_ntoa(sock_ips[index].sin_addr));
	printf("%d\n", ntohs(sock_ips[index].sin_port));
}

struct sockaddr_in * get_sock_addr_from_process_id(int process_id) {
	return &(sock_ips[process_id-1]);
}

int get_id_from_sock_addr(struct sockaddr_in * sock_addr) {
	return get_id_from_port(ntohs(sock_addr->sin_port));
}

int get_sock_fd() {
	return sockfd;
}

void free_socket() {
	free(sock_ips);
}
