#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	LOG_SIZE	25

typedef enum{BROADCAST, DELIVER} Log_type;

void init_logs(int process_id);
void add_logs(int seq, Log_type log_type, int process_id);
void write_logs(int process_id);

#endif
