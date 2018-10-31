#include "log.h"

static char logs[LOG_SIZE];

// Initialize the log file as an empty file
void init_logs(int process_id){
	logs[0] = '\0';
	char log_file_name[20];
	sprintf(log_file_name, "da_proc_%d.out", process_id);
	FILE *log_file = fopen(log_file_name, "w");
	if(log_file == NULL) {
		printf("Cannot open '%s'\n", log_file_name);
		exit(0);
	}
	else {
		// Nothing to do, we created an empty file
		fclose(log_file);
	}
}

// Add a log the logs variable
// If the logs variable is full, write the content on disk
void add_logs(int seq, Log_type log_type, int process_id){
	if (log_type == BROADCAST) {
		char log[14];
		sprintf(log, "b %d\n", seq);
		if (LOG_SIZE - strlen(logs) > 13) {
			strcat(logs, log);
		}
		else {
			// The logs variable is full, we write the content on disk
			write_logs(process_id);
			// Update the logs variable with the current log
			strcpy(logs, log);
		}
	}
	else if (log_type == DELIVER) {
		// TODO
	}
	else {
		printf("Error : unknown Log_type");
		exit(0);
	}
}

void write_logs(int process_id) {
	if (strlen(logs) > 0) {
		char log_file_name[20];
		sprintf(log_file_name, "da_proc_%d.out", process_id);
		FILE *log_file = fopen(log_file_name, "a");
		if(log_file == NULL) {
			printf("Cannot open '%s'\n", log_file_name);
			exit(0);
		}
		else {
			fprintf(log_file, "%s", logs);
			fclose(log_file);
		}
	}
}
