#include "log.h"

static char logs[LOG_SIZE];

// Initialize the log file as an empty file
void init_logs(){
	logs[0] = '\0';
	char log_file_name[20];
	sprintf(log_file_name, "da_proc_%d.out", get_process_id());
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
void add_logs(int src, char* payload, Log_type log_type){
	if (log_type == BROADCAST) {
		char log[16];
		sprintf(log, "b %s\n", payload);
		if (LOG_SIZE - strlen(logs) > 16) {
			strcat(logs, log);
		}
		else {
			// The logs variable is full, we write the content on disk
			write_logs();
			// Update the logs variable with the current log
			strcpy(logs, log);
		}
	}
	else if (log_type == DELIVER) {
		char log[32];
		sprintf(log, "d %d %s\n", src, payload);
		if (LOG_SIZE - strlen(logs) > 32) {
			strcat(logs, log);
		}
		else {
			// The logs variable is full, we write the content on disk
			write_logs();
			// Update the logs variable with the current log
			strcpy(logs, log);
		}
	}
	else {
		printf("Error : unknown Log_type");
		exit(0);
	}
}

void write_logs() {
	if (strlen(logs) > 0) {
		char log_file_name[20];
		sprintf(log_file_name, "da_proc_%d.out", get_process_id());
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
