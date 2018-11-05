#ifndef MSG_SENT_H
#define MSG_SENT_H

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#include <time.h>

#include "socket.h"
#include "algo.h"
#include "setting.h"

typedef struct Msg_sent{
	char* payload;
	int msg_src;
	int msg_dst;
	double t_start;
	struct Msg_sent* next;
} Msg_sent;

void add_msg_sent(char* msg, int msg_src, int dst_process_id);
void remove_msg_sent(char* msg, int msg_dst, int msg_src);
bool msg_sent_times_up(double t_start);
double get_cur_time();
void resend_packets_if_needed();
void print_msg_sent();
void print_first_n_msg_sent(int n);
void free_msg_sent();

#endif