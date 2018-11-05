#include "udp.h"

void send_udp_packet(const char* msg, int dst_process_id) {
	if (get_random_bool(75)) {
		const struct sockaddr * dst_sock_addr = 
			(const struct sockaddr *)get_sock_addr_from_process_id(dst_process_id);
		sendto(get_sock_fd(), msg, strlen(msg), MSG_CONFIRM, 
				dst_sock_addr, sizeof(*dst_sock_addr)); 
	}
	else {
		if(DEBUG_PRINT){
			printf("LOST\n");
		}
	}
}

// NEED TO FREE
char* receive_udp_packet(struct sockaddr_in * src_sock_addr) {
	int n;
	socklen_t len = sizeof(struct sockaddr_in);
    int MAXLINE = 16;
    char* buffer = (char*)malloc(MAXLINE); 
    memset(src_sock_addr, 0, sizeof(struct sockaddr_in)); 

    n = recvfrom(get_sock_fd(), buffer, MAXLINE, MSG_WAITALL, 
    			 ( struct sockaddr *) src_sock_addr, &len); 
    buffer[n] = '\0'; 
    return buffer;
}
