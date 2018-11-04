
#da_proc: da_proc.c utils.o utils.h
	#  gcc -Wall -std=c99 -o da_proc utils.o da_proc.c

#utils.o: da_proc.c utils.h utils.c
#	gcc -Wall -std=c99 utils.c -o utils.o


CC=gcc
CFLAGS=-I.

all: da_proc

# SANS flags
da_proc: da_proc.c ack.c forward.c timer.c random.c delivered.c socket.c log.c utils.c udp.c algo.c msg_sent.c process.c
	gcc -Wall -std=c99 -D_POSIX_C_SOURCE=199309L -o da_proc ack.c forward.c timer.c random.c delivered.c socket.c log.c process.c msg_sent.c algo.c udp.c utils.c da_proc.c -I.

# AVEC flag je pense que ça marche
#da_proc: da_proc.c utils.c
#	$(CC) -Wall -std=c99 -o da_proc utils.c da_proc.c

clean:
	rm da_proc
