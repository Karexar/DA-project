#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"

#define	LOG_SIZE	1024

typedef enum{BROADCAST, DELIVER} Log_type;

void init_logs();
void add_logs(int src, char* payload, Log_type log_type);
void write_logs();

#endif
