#ifndef UDP_H
#define UDP_H

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#include "utils.h"
#include "process.h"
#include "log.h"
#include "socket.h"
#include "random.h"
#include "setting.h"

void send_udp_packet(const char* msg, int dst_process_id);
char* receive_udp_packet(struct sockaddr_in * src_sock_addr);


#endif