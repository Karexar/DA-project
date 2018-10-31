#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#include "process.h"

void init_socket(int process_id);
void print_socket(int index);
struct sockaddr_in * get_sock_addr_from_process_id(int process_id);
int get_id_from_sock_addr(struct sockaddr_in * sock_addr);
int get_sock_fd();
void free_socket();

#endif
