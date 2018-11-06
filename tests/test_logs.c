#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure of one event of a process
static struct ProcessEvent
{
	char type;		// can be 'b' for broadcast or 'd' for delivery
	int senderId;	// ID of sender in case of delivery, otherwise has -1 value
	int seqNumber;	// ID of message
};

// Structure containing the log of one process
static struct ProcessLog
{
	int id;							// ID of process
	struct ProcessEvent *events;	// table containing the events output
	int nbLines;					// total number of lines
};

// Load and store the file into a ProcessLog structure
// Each event in the logs are in a ProcessEvent structure
struct ProcessEvent* loadFile(const char *filename, int *nbLines) {

	FILE *f = fopen(filename, "r");;
    char *line = NULL;
    size_t len = 0;
    size_t read;

    // Allocate the memory for 3000 lines of outputs
    struct ProcessEvent *processEvent = calloc(3000, sizeof(struct ProcessEvent));

    // Check for reading errors
    if (f == NULL)
    {
    	printf("Error while opening the file '%s.\n", filename);
        exit(EXIT_FAILURE);
    }	
    
    int i = 0;	// used to count the number of events

    // Read the file line by line
    while ((read = getline(&line, &len, f)) != -1) {

        // Initialize useful values
        char type;
		int senderId = -1;
		int seqNumber = -1;
		char *token = NULL;	// used to get each character of one event

		// Get the first element of the line
		token = strtok(line, " ");

		// Check for errors if there is any
		if(*token != 'b' && *token != 'd')
		{
			printf("Error: Event not recognized at line %d. Must be broadcast (b) or delivery (d).\n", i);
			exit(1);
		}

		// Check if it is a broadcast (b) or a delivery (d)
		if(*token == 'b') {

			type = 'b';

			// Get the sequence number of the message
			token = strtok(NULL, " ");

			// Check if the value is null
      		if(token == NULL) {
      			printf("Error: No sequence number for broadcast indicated at line %d.\n", i);
      			exit(1);
      		}

      		seqNumber = atoi(token);

		} else {

			type = 'd';

			// Get the ID of the sender o the message
			token = strtok(NULL, " ");

			if(token == NULL) {
      			printf("Error: No sender ID for delivery indicated at line %d.\n", i);
      			exit(1);
      		}

      		senderId = atoi(token);
      		if(senderId <= 0)
      		{	
      			printf("There is a problem with the sender ID at line %d.\n", i);
      		}

			// Get the sequence number of the message
			token = strtok(NULL, " ");

			if(token == NULL) {
      			printf("Error: No sequence number for broadcast indicated at line %d.\n", i);
      			exit(1);
      		}

      		seqNumber = atoi(token);
		}

		   	// Store the current event
			processEvent[i].type = type;
			processEvent[i].senderId = senderId;
			processEvent[i].seqNumber = seqNumber;

        i++;
    }

    *nbLines = i;

    fclose(f);

    if (line)
        free(line);

    return processEvent;
}

// Check log for one process
void testLog(struct ProcessLog *processLogs, int nb_proc)
{

	for(int procIndex=0; procIndex < nb_proc; ++procIndex)
	{
		struct ProcessLog currentLog = processLogs[procIndex];
		int processId = currentLog.id;
		int nbLines = currentLog.nbLines;

		// Iterate over each line of a log of a process
		for(int i=0; i < nbLines; ++i)
		{	
			char type = currentLog.events[i].type;
			int seqNumber = currentLog.events[i].seqNumber;

			// Apply a check depending on the type of event
			if(type == 'b')
			{
				int isBroadcastFailure = broadcastCheck(processLogs, nb_proc, processId, seqNumber);
				if(isBroadcastFailure) {
					printf("There is a broadcast failure with message %d and sender %d.\n", seqNumber, processId);
					exit(1);
				}
			} else if(type == 'd')
			{
				int senderId = currentLog.events[i].senderId;
				int isDeliveryFailure = deliveryCheck(processLogs[senderId - 1], seqNumber);

				if(isDeliveryFailure) {
					printf("There is a delivery failure with message %d and sender %d.\n", seqNumber, senderId);
					exit(1);
				}
			} else
			{
				printf("Type is undefined\n");
				exit(1);
			}
		}
	}

	printf("Tests are successful ! Congratulations :D\n");	
}

// Check if other processes have deliver the message broadcasted by process 'procIndex'
int broadcastCheck(struct ProcessLog *processLogs, int nb_proc, int senderId, int seqNumber)
{
	int isFailure = 1;

	// Check for all processes that the broadcast was well done
	for(int i=0; i < nb_proc; ++i)
	{
		if (i+1 != senderId) 
		{
			struct ProcessEvent *events = processLogs[i].events;
			int nbLines = processLogs[i].nbLines;

			for(int j=0; j < nbLines; ++j)
			{
				if(events[j].type == 'd' && events[j].senderId == senderId && events[j].seqNumber == seqNumber)
				{
					isFailure = 0;
				}
			}

		}
	}
	return isFailure;
}

// Check that the sender of this deliver event has sent the message
int deliveryCheck(struct ProcessLog senderProcess, int seqNumber)
{

	int isFailure = 1;
	struct ProcessEvent *events = senderProcess.events;
	int nbLines = senderProcess.nbLines;

	for(int i=0; i < nbLines; ++i)
	{
		if(events[i].type == 'b' && events[i].seqNumber == seqNumber) 
		{
			isFailure = 0;
		}
	}
	return isFailure;
}


int main(int argc, char** argv) {

	// Check the number of arguments
	if (argc == 2)
	{
		// Get the number of process logs
		int nb_proc = atoi(argv[1]); 

		printf("Number of processes: %d\n", nb_proc);

		// Create a string table containing filename of logs
		char logs_filename[nb_proc][17];

		// Store the file location of logs of processes
		for(int procIndex=0; procIndex < nb_proc; ++procIndex)
		{	
			char proc_name[12] = "../da_proc_";

			// Convert the ID of process into string
  			char number[nb_proc][2];
  			snprintf(number[procIndex], 2, "%d", procIndex+1);

  			// Clear the char table
  			memset(logs_filename[procIndex], 0, 17);

  			// Write the filename in the table
			strcat(logs_filename[procIndex], "../da_proc_");
			strcat(logs_filename[procIndex], number[procIndex]);
			strcat(logs_filename[procIndex], ".out");

			printf("File %d name: %s\n", procIndex+1, logs_filename[procIndex]);
		}
		

		// Store the loaded log in an array of ProcessLogs
		struct ProcessLog processLogs[nb_proc];

		// Open and read the logs of all processes
		for(int i=0; i < nb_proc; i++)
		{
			char **logs;

			// Set the logs and nbLines for the current process
			struct ProcessEvent *events = loadFile(logs_filename[i], &processLogs[i].nbLines);

			processLogs[i].id = i+1;
			processLogs[i].events = events;

			// Show the content of the loaded file
			// Uncomment to print the content in the console
			/*printf("\n Content of the loaded file %d: \n", i);
			for(int j = 0; j < processLogs[i].nbLines; j++){
				printf("%c %d %d\n", processLogs[i].events[j].type, 	processLogs[i].events[j].senderId, processLogs[i].events[j].seqNumber);
			}*/
		}

		// Check the log of each process
		testLog(processLogs, nb_proc);
	} else
	{
		printf("Error while parsing the main arguments");
		exit(1);
	}

    exit(EXIT_SUCCESS);
}
