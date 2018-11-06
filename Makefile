
all: da_proc

da_proc: da_proc.c clock.c ack.c forward.c timer.c random.c delivered.c socket.c log.c utils.c udp.c algo.c msg_sent.c process.c
	gcc -Wall -std=c99 -D_POSIX_C_SOURCE=199309L -o da_proc clock.c ack.c forward.c timer.c random.c delivered.c socket.c log.c process.c msg_sent.c algo.c udp.c utils.c da_proc.c -I.

clean:
	rm da_proc
