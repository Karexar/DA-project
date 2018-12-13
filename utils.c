#include "utils.h"

// Get the main argument corresponding to the index given in parameter
// Check for the type and exit if an int is expected but a string is given
char* get_main_arg(int argc, char** argv, int index, Type type) {
	if (argc == index && index == 3) {
		return "5"; // default number of messages to broadcast
	}
	else if (argc > index) {
		if (type == INTEGER && strspn(argv[index], "0123456789") != strlen(argv[index])) {
			printf("Error : integer expected, string found\n");
			exit(1);
		}
		else {
			return argv[index];
		}
	}
	else {
		printf("Error : too few main arguments\n");
		exit(1);
	}
}

// Get the next word (space or return separeted)
// Assume the cursor is at the beginning of the word
// Let the cursor after the first separator
// NEED TO FREE
char* get_word(FILE* f, bool* end_of_line, bool* end_of_file) {
	int length = 32;
	char *s = (char*)malloc(length);
	int j = 0;
	bool keep_going = 1;
	while (j < length-1 && keep_going) {
		s[j] = fgetc(f);
		if (s[j] == '\n' || s[j] == ' ' || s[j] == EOF) {
			keep_going = 0;
			if (s[j] == '\n' && end_of_line != NULL) {
				*end_of_line = true;
			}
			else if (s[j] == EOF && end_of_file != NULL) {
				*end_of_file = true;
			}
		}
		else {
			++j;
		}
	}
	s[j] = '\0';
	return s;
}

/*
	Parse a message to retrieve all elements
	msg : the message to parse
	msg_type : to store the message type ('a' for ack, 's' for regular message)
	msg_src : to store the origin of the message (the process who send it at first)
	payload : to store the content of the message (here the sequence number)
	vc_str : to store the vector clock containing the message count for each 
			 dependency of the message source when it sent the message. Here the 
			 vector clock is stored as string.
*/
void parse_message(char* msg, char* msg_type, int* msg_src, char** payload, char** vc_str) {
	// parse the message type
	int i = 0;
	int len_msg = strlen(msg);
	*msg_type = msg[0];
	i+=2;

	// parse the message source
	int len_src = 0;
	char c = msg[i];
	while (c != '\n' && i < len_msg) {
		++i;
		++len_src;
		c = msg[i];
	}
	char* tmp_msg_src = (char*)malloc(len_src+1);
	sprintf(tmp_msg_src, "%.*s", len_src, msg+(i-len_src));
	*msg_src = atoi(tmp_msg_src);
	free(tmp_msg_src);
	tmp_msg_src = NULL;

	// Parse the payload
	int len_payload = 0;
	++i;
	c = msg[i];
	while (c != '\n' && i < len_msg) {
		++i;
		++len_payload;
		c = msg[i];
	}
	(*payload) = (char*)malloc(len_payload+1);
	sprintf(*payload, "%.*s", len_payload, msg+(i-len_payload));

	// parse the vc
	int len_vc = 0;
	++i;
	if (i < len_msg) {
		c = msg[i];
		while (i < len_msg) {
			++i;
			++len_vc;
			c = msg[i];
		}
		(*vc_str) = (char*)malloc(len_vc+1);
		sprintf(*vc_str, "%.*s", len_vc, msg+(i-len_vc));
	}
}

