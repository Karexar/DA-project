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
char* get_word(FILE* f) {
	int length = 32;
	char *s = (char*)malloc(length);
	int j = 0;
	bool keep_going = 1;
	while (j < length-1 && keep_going) {
		s[j] = fgetc(f);
		if (s[j] == '\n' || s[j] == ' ') {
			keep_going = 0;
		}
		else {
			++j;
		}
	}
	s[j] = '\0';
	return s;
}