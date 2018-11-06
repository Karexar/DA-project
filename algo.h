#ifndef ALGO_H
#define ALGO_H

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#include <time.h>

// non blocking socket
#include <sys/select.h> 
#include <fcntl.h>

#include "udp.h"
#include "msg_sent.h"
#include "delivered.h"
#include "forward.h"
#include "ack.h"
#include "process.h"
#include "setting.h"
#include "timer.h"

void init_beb();
void init_urb();
void perfect_links_send(char* payload, int msg_src, int dst_process_id);
void perfect_links_deliver(char* msg, char msg_type, int msg_src, 
						   struct sockaddr_in * src_sock_addr);
void broadcast(char* payload, int msg_src, bool log_it);
void urb_deliver(char* msg, char msg_type, int msg_src, 
						   struct sockaddr_in * src_sock_addr);
void FIFO_listen();

#endif