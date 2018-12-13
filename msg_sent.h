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
#include "process.h"
#include "utils.h"

/* 
	This structure contains all information about a message that 
	has been sent by a process. Each time a process send a 
	message, it adds a msg_sent to a static list, and it will 
	iterate periodically on the list to resend some messages if
	needed. A time_stamp is included in the structure in order
	to resend the message in case of timeout.
*/
typedef struct Msg_sent{
	int src;
	int dst;
	char* payload;
	char* vc_str;
	double t_start;
	struct Msg_sent* next;
} Msg_sent;

/*
	Check if the given {msg_src, payload, msg_dst} is already 
	present in the linked list.
*/
bool msg_already_in_msg_sent(int msg_src, char* payload, int msg_dst);

/*
	Add a message sent to the linked list. 
	msg_src : origin of the message (process who created it)
	payload : content of the message (a sequence number in our case)
	vc_str : the vector clock containing the message count for 
	         each dependency of the source
	dst : the process to which the message was sent
*/
void add_msg_sent(int msg_src, char* payload, char* vc_str, int dst);

/*
	Remove a message from the linked list. 
	msg_src : origin of the message (process who created it)
	payload : content of the message (a sequence number in our case)
	dst : the process to which the message was sent
	Note that we don't need the vector clock here because we ensured 
	there is no multiple messages with same payload, destination,
	and source.  
*/
void remove_msg_sent(int msg_src, char* payload, int msg_dst);
bool msg_sent_times_up(double t_start);
double get_cur_time();
void resend_packets_if_needed();
void print_msg_sent();
void print_first_n_msg_sent(int n);
void free_msg_sent();

#endif