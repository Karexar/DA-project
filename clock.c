#include "clock.h"

static struct timespec tstart={0,0}, tend={0,0};

void set_start_time() {
	clock_gettime(CLOCK_MONOTONIC, &tstart);
}

void print_time() {
	clock_gettime(CLOCK_MONOTONIC, &tend);
	printf("%.5f seconds\n", ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
}